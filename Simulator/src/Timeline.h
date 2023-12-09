#pragma once
#include "Tissue.h"
#include <memory>
#include <vector>
#include <assert.h>
#include <inttypes.h>
#include <fstream>
#include <string>

// CellTimeline is a class that stores a version of a cell at each timestep.
// Data in stored in a buffer in format:
// ID TIMESTEP VALUES ID TIMESTEP VALUES ID TIMESTEP VALUES ...
// ID (Index Descriptor) describes how many values are stored. It is an int
// and the position of the bit set tells us which property the values is.
// 00000000000000000000000000000011 means that the first an second properties
// in the cell changed. The changes are the VALUES. They are the new value of 
// properties. If 2 bits in ID are set, it is expected that there are 2 VALUES.
// TIMESTEP is the timestep.
// The last bit in ID set means it is the last cell in the tissue and second last bit
// in ID set means it is the last cell in the timeline

namespace csim {

	template<class ValueType, class TimestepType, class IDType, size_t NumProperties>
	class CellTimelineImpl
	{
	private:
		//align to the size of largest type in the buffer
		template <class ValueType, class TimestepType, class IDType>
		struct AlignmentReq //alignment requirement
		{
			using type =
				typename std::conditional_t<(sizeof(ValueType) > sizeof(TimestepType)), ValueType,
				typename std::conditional_t<(sizeof(TimestepType) > sizeof(IDType)), TimestepType, IDType>>;
			static constexpr int Size = sizeof(type) * 8;
		};

		template<class T> struct OneImpl;
		template<> struct OneImpl<uint64_t> { static constexpr uint64_t value = 1i64; };
		template<> struct OneImpl<uint32_t> { static constexpr uint32_t value = 1i32; };
		template<> struct OneImpl<uint16_t> { static constexpr uint16_t value = 1i16; };
		template<> struct OneImpl<uint8_t> { static constexpr uint8_t value = 1i8; };
		using One = OneImpl<IDType>;

	public:
		struct Version
		{
			ValueType timestep = 0;
			Cell cell{};
		};

		struct VersionProperty
		{
			TimestepType timestep = 0;
			ValueType value = 0;
		};

	private:
		struct Entry
		{
			Entry() { values.reserve(5); }
			IDType ID = 0; //Index Descriptor
			TimestepType timestep = 0;
			std::vector<ValueType> values;
		};

	public:
		CellTimelineImpl() :
			CellTimelineImpl(0)
		{
			m_WriteEnd = m_DataBuffer;
			m_LastEntryByte = m_DataBuffer;
		}

		//last is whether this is the last cell in the tissue
		CellTimelineImpl(const Cell& startingCell, TimestepType timestep, bool last) :
			CellTimelineImpl(0)
		{
			m_WriteEnd = m_DataBuffer;
			m_LastEntryByte = m_DataBuffer;

			//Add the starting cell entry to our buffer.
			//always assume we are the last cell in the timeline when adding cell
			//when next cell is added, it will set itself as last cell and unset this one

			IDType id = (IDType)((1i64 << NumProperties) - 1);
			id |= One::value << m_LastCellBit;
			SetIDLastCellInTissue(id, last);
			AddToBuffer(id);
			AddToBuffer(timestep);
			for (int i = 0; i < NumProperties; i++)
				AddToBuffer(startingCell.GetProperty(i));
			m_NumEntries++;

			//init last changed properties pointers to the first entry 
			//respective properties in the first entry
			for (int i = 0; i < NumProperties; i++)
			{
				m_LastPropertyChange[i] = m_DataBuffer + sizeof(IDType) + sizeof(TimestepType) + (i * sizeof(ValueType));
			}
		}

		//move constructor and asignment
		CellTimelineImpl(CellTimelineImpl&& other) noexcept
		{
			Move(std::move(other));
		}

		CellTimelineImpl& operator=(CellTimelineImpl&& other) noexcept
		{
			Move(std::move(other));
			return *this;
		}

		//write copy stuff later if copy needed

		~CellTimelineImpl()
		{
			FreeBuffer();
		}

	private:
		CellTimelineImpl(int unused)
		{
			ReallocBuffer(m_BufferCap);

			if (m_DataBuffer == nullptr)
				return;//TODO: error handeling
		}

	public:
		//iterator
		//Implement later if performance becomes a problem. Could be a lot faster than calling GetVersion
		//multiple times for each version since iterators can store where we have reached interally
		//so no need to start from the beginning each time we want to retrieve a version.
		//(Narrator: Performance did become a problem)

		//If iterating through timeline is needed USE ITERATOR. It is MASSIVELY faster than
		//using GetVersion multiple times. Use GetVersion for one off Gets.

		//Quirks left in for the sake of speed(This iterator mainly is here to improve speed not convenience):
		//1. If operator++ is called, dereference operator must be called
		//before ++ is called again. Otherwise, it wont give
		//correct values. If you need to skip multiple cells without
		//dereferending in between, use operator+=
		//2. end() is not 1 past the end of the last item. It IS the last item.
		//Quirk left in for convenience:
		//99% of the time just use it <= end as stop condition. It stops ++ and += from going past the 
		//end (even when += increments with values greater than 1). ++ and += dont assert or throw when it 
		//detects increment beyond last item, they just return the last item so using them with <= makes 
		//everything convenient.
		class Iterator
		{
		public:
			Iterator() {}

			//iterator does NOT modify timeline
			Iterator(CellTimelineImpl* timeline, size_t index) :
				m_Entry{ timeline->m_DataBuffer }, timeline{ timeline }
			{
				m_Index = index;
				IDType id = timeline->JoinBytes<IDType>(m_Entry);
				size_t current = 0;
				uint8_t* offset = m_Entry + sizeof(IDType) + sizeof(TimestepType);

				while (current < index)
				{
					id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
					m_Entry = offset - sizeof(IDType) - sizeof(TimestepType);
					//logic to update last changed properties 
					uint8_t* i = offset;
					int nbits = std::popcount(id);
					for (int idIndex = 0; i < offset + nbits * sizeof(ValueType); i += sizeof(ValueType), idIndex++)
					{
						while (!(id & (One::value << idIndex)))
							idIndex++;
						m_LastChangedProp[idIndex] = i;
					}

					//logic to go to next entry
					offset = i;
					id = timeline->JoinBytes<IDType>(offset);
					offset += sizeof(IDType) + sizeof(TimestepType);
					current++;
				}
			}

		public:
			Version operator*()
			{
				Version v;
				v.timestep = timeline->JoinBytes<TimestepType>(m_Entry + sizeof(IDType));
				IDType id = timeline->JoinBytes<IDType>(m_Entry);
				id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
				int idIndex = 0;
				bool set[NumProperties] = { 0 };
				for (int i = 0; i < std::popcount(id); i++)
				{
					while (!(id & (One::value << idIndex)))
						idIndex++;
					uint8_t* valurAdr = m_Entry + sizeof(IDType) + sizeof(TimestepType) + i * sizeof(ValueType);
					ValueType value = timeline->JoinBytes<float>(valurAdr);
					v.cell.SetProperty(idIndex, value);
					m_LastChangedProp[idIndex] = valurAdr;
					set[idIndex] = true;
					idIndex++;
				}
				for (int i = 0; i < NumProperties; i++)
				{
					if (!set[i])
						v.cell.SetProperty(i, timeline->JoinBytes<ValueType>(m_LastChangedProp[i]));
				}
				return v;
			}

			VersionProperty GetVersionProperty(int prop)
			{
				VersionProperty vp;
				vp.timestep = timeline->JoinBytes<TimestepType>(m_Entry + sizeof(IDType));
				IDType id = timeline->JoinBytes<IDType>(m_Entry);
				id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
				int idIndex = 0;
				bool found = false;
				for (int i = 0; i < std::popcount(id); i++)
				{
					while (!(id & (One::value << idIndex)))
						idIndex++;
					uint8_t* valurAdr = m_Entry + sizeof(IDType) + sizeof(TimestepType) + i * sizeof(ValueType);
					ValueType value = timeline->JoinBytes<ValueType>(valurAdr);
					if (idIndex == prop)
					{
						vp.value = value;
						found = true;//we have to let the loop finish so all last updated properties are updated
					}
					m_LastChangedProp[idIndex] = valurAdr;
					idIndex++;
				}

				if (found)
					return vp;

				for (int i = 0; i < NumProperties; i++)
				{
					if (i == prop)
					{
						vp.value = timeline->JoinBytes<ValueType>(m_LastChangedProp[i]);
						return vp;
					}
				}
				assert(false && "Sum ting wong. Shouldn't ever reach here");
			}

			IDType VersionChanges()
			{
				return timeline->JoinBytes<IDType>(m_Entry);
			}

			Iterator& operator++()
			{
				if (m_Index == timeline->m_NumEntries)
					return *this;
				IDType id = timeline->JoinBytes<IDType>(m_Entry);
				id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
				m_Entry += sizeof(IDType) + sizeof(TimestepType) + std::popcount(id) * sizeof(ValueType);
				m_Index++;
				return *this;
			}

			Iterator& operator++(int)
			{
				Iterator& tmp = *this;
				++* this;
				return tmp;
			}

			Iterator& operator+=(int v)
			{
				for (int i = 0; i <= v; i++)
				{
					//reminder: dont assert here just set some flag to saying it happened,
					//will make working with it easier (It wont go past because <= will stop
					//it from going past)
					if (v + m_Index > timeline->m_NumEntries)
					{
						//Set iterator to the end. m_Index is irrelevant now since we cant go back
						m_Entry = timeline->EntryOffsetFromIndex(timeline->m_NumEntries);
						return *this;//report out of range error
					}

					//update last changed so we dont miss anything when skipping entries
					IDType id = timeline->JoinBytes<IDType>(m_Entry);
					id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
					int idIndex = 0;
					int nBits = std::popcount(id);
					for (int j = 0; j < nBits; j++)
					{
						while (!(id & (One::value << idIndex)))
							idIndex++;
						m_LastChangedProp[idIndex] = m_Entry + sizeof(IDType) + sizeof(TimestepType) + j * sizeof(ValueType);
						idIndex++;
					}

					++(*this);
				}

				return *this;
			}

			friend bool operator==(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Entry == i2.m_Entry;
			}

			friend bool operator!=(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Entry != i2.m_Entry;
			}

			friend bool operator<(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Entry < i2.m_Entry;
			}
			friend bool operator<=(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Entry <= i2.m_Entry;
			}

		private:
			size_t m_Index = 0;
			uint8_t* m_Entry = nullptr;
			//pointer to property that last changed
			uint8_t* m_LastChangedProp[NumProperties]{};
			CellTimelineImpl* timeline = nullptr;
		};

	public:
		CellTimelineImpl::Iterator begin()
		{
			return Iterator{ this, 0 };
		}

		CellTimelineImpl::Iterator begin(size_t index)
		{
			assert(index <= m_NumEntries); //IMP CHANGE: < to <= MAYBE NOT CORRECT
			return Iterator{ this, index };
		}

		CellTimelineImpl::Iterator end()
		{
			return Iterator{ this, m_NumEntries };
		}

	public:
		//adds the given cell to the timeline
		void Add(const Cell& cell, TimestepType timestep, bool last)
		{
			Entry newEntry = EntryFromCell(cell);

			if (newEntry.values.empty())
			{
				newEntry.ID |= (One::value << m_LastCellBit);
				SetIDLastCellInTissue(newEntry.ID, last);
				if (last)
					UnSetIDPreviousLastCellInTissue();
				AddToBuffer(newEntry.ID);
				AddToBuffer(timestep);
				UnSetIDPreviousLastCellInTimeline();
				m_LastEntryByte = m_WriteEnd - sizeof(IDType) - sizeof(TimestepType);
				m_NumEntries++;
				return;
			}

			newEntry.ID |= (One::value << m_LastCellBit);
			//this is now the last cell in the timeline
			SetIDLastCellInTissue(newEntry.ID, last);
			if (last)
				UnSetIDPreviousLastCellInTissue();
			AddToBuffer(newEntry.ID);
			AddToBuffer(timestep);

			int idIndex = 0;
			for (int i = 0; i < newEntry.values.size(); i++)
			{
				AddToBuffer(newEntry.values[i]);
				while (!(newEntry.ID & (One::value << idIndex)))
					idIndex++;
				m_LastPropertyChange[idIndex] = m_WriteEnd - sizeof(ValueType);
				idIndex++;
			}
			UnSetIDPreviousLastCellInTimeline();

			m_LastEntryByte = m_WriteEnd - (sizeof(ValueType) * newEntry.values.size()) - sizeof(TimestepType) - sizeof(IDType);
			m_NumEntries++;
		}

		//gets the given version of the from the timeline
		CellTimelineImpl::Version GetVersion(size_t index) const
		{
			Version version;
			Entry entry = EntryFromIndex(index);
			version.timestep = entry.timestep;

			if (entry.values.empty())
			{
				version.cell = RetrieveNoChangesCell(index);
				return version;
			}

			int idIndex = 0;
			bool set[NumProperties] = { 0 };
			for (int i = 0; i < entry.values.size(); i++)
			{
				while (!(entry.ID & (One::value << idIndex)))
					idIndex++;
				version.cell.SetProperty(idIndex, entry.values[i]);
				set[idIndex] = true;
				idIndex++;
			}
			for (int i = 0; i < NumProperties; i++)
			{
				if (!set[i])
				{
					version.cell.SetProperty(i, JoinBytes<ValueType>(m_LastPropertyChange[i]));
				}
			}
			return version;
		}

		//get the changes in this particular version
		IDType VersionChanges(size_t index) const
		{
			return JoinBytes<IDType>(EntryOffsetFromIndex(index));
		}

		//get the changes in the last cell
		IDType LastChanges() const
		{
			return JoinBytes<IDType>(m_LastEntryByte);
		}

		//get the last cell from the timeline
		CellTimelineImpl::Version LastVersion() const
		{
			Version version;
			version.timestep = JoinBytes<TimestepType>(m_LastEntryByte + sizeof(IDType));
			for (int i = 0; i < NumProperties; i++)
				version.cell.SetProperty(i, JoinBytes<ValueType>(m_LastPropertyChange[i]));
			return version;
		}

		//get the last cell from the timeline
		CellTimelineImpl::Version FirstVersion() const
		{
			Version version;
			uint8_t* start = m_DataBuffer + sizeof(IDType);
			version.timestep = JoinBytes<TimestepType>(start);
			start += sizeof(TimestepType);
			for (int i = 0; i < NumProperties; i++)
				version.cell.SetProperty(i, JoinBytes<ValueType>(start + i * sizeof(ValueType)));
			return version;
		}

		//number of cells in the timeline
		size_t Size() const
		{
			return m_NumEntries;
		}

	private:
		//adds either an int or float to the buffer
		//start is the local pointer initialized in Add
		//which needs to be updated too if buffer changes 
		//size
		template<class T>
		void AddToBuffer(T data)
		{
			//if this static assert fails, there is a mismatch in type that is being given to Add function.
			//Example: Creating CellTimelineDF and giving it value of type float.
			static_assert(std::is_same_v<T, IDType> || std::is_same_v<T, ValueType> || std::is_same_v<T, TimestepType>);
			if (m_BufferSize + sizeof(T) > m_BufferCap)
			{
				uint8_t* tmp = m_DataBuffer;
				ReallocBuffer(m_BufferSize * 2);
				ReAdjustPointers(tmp);
			}
			memcpy(m_WriteEnd, &data, sizeof(T));
			m_BufferSize += sizeof(T);
			m_WriteEnd += sizeof(T);
		}

		//reallocate memory for buffer when we run out of it
		//allocation aligned to 4 bytes
		void ReallocBuffer(size_t cap)
		{
			uint8_t* newBuf = (uint8_t*)_aligned_malloc(cap, AlignmentReq<ValueType, TimestepType, IDType>::Size);
			if (!newBuf)
				return;//do error handle
			memcpy(newBuf, m_DataBuffer, m_BufferSize);
			FreeBuffer();
			m_DataBuffer = newBuf;
			m_BufferCap = cap;
		}

		//when ReallocBuffer is called, it will invalidate our pointers into the
		//old buffer memory if _aligned_realloc decides to move the memory block.
		//this function readjusts pointers so they point to the correct locations
		//into the new memory block.
		void ReAdjustPointers(uint8_t* old)
		{
			m_LastEntryByte = m_DataBuffer + (m_LastEntryByte - old);
			m_WriteEnd = m_DataBuffer + (m_WriteEnd - old);

			for (int i = 0; i < NumProperties; i++)
			{
				m_LastPropertyChange[i] = m_DataBuffer + (m_LastPropertyChange[i] - old);
			}
		}

		//joins 4 bytes to either make an int or float
		//IMPORTANT NOTE: it assumes that the offset is 4-byte aligned, 
		//otherwise we have undefined behaviout.
		//for this reason, m_DataBuffer is aligned to 4 bytes
		template <class T>
		T JoinBytes(uint8_t* offset) const
		{
			static_assert(std::is_same_v<T, IDType> || std::is_same_v<T, ValueType> || std::is_same_v<T, TimestepType>);
			if constexpr (std::endian::native == std::endian::little)
			{
				return *reinterpret_cast<T*>(offset);
			}
			else if (std::endian::native == std::endian::big)
			{
				T reverse = 0;
				for (size_t i = 0; i < sizeof(T); i++)
				{
					reverse = (reverse << 8) | offset[i];
				}
				return reverse;
			}
			else
			{
				return T{};
			}
		}

		//grabs an entry from the buffer based on index and returns it. index is the
		//position of the entry in the buffer.
		CellTimelineImpl::Entry EntryFromIndex(size_t index) const
		{
			uint8_t* e = EntryOffsetFromIndex(index);
			if (e == nullptr)
				return Entry{};
			return MakeEntry(e);
		}

		//looks at the difference between the given cell and the last cell.
		//constructs and entry with correct id and values that were changed.
		CellTimelineImpl::Entry EntryFromCell(const Cell& cell) const
		{
			Entry e;
			for (int i = 0; i < NumProperties; i++)
			{
				e.ID &= ~(One::value << i);
				if (cell.GetProperty(i) != JoinBytes<ValueType>(m_LastPropertyChange[i]))
				{
					e.ID |= One::value << i;
					e.values.push_back(cell.GetProperty(i));
				}
			}
			return e;
		}

		uint8_t* EntryOffsetFromIndex(size_t index) const
		{
			assert(index <= m_NumEntries && "Index out of range");

			if (index == 0)
				return m_DataBuffer;

			IDType id = JoinBytes<IDType>(m_DataBuffer);
			size_t current = 0;
			uint8_t* offset = m_DataBuffer + sizeof(IDType) + sizeof(TimestepType);
			while (current != index)
			{
				id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
				offset += std::popcount(id) * sizeof(ValueType);
				id = JoinBytes<IDType>(offset);
				offset += sizeof(IDType) + sizeof(TimestepType);
				current++;
			}
			//above loop steps past the ID and timestep of the entry we want, so go back
			return offset - sizeof(IDType) - sizeof(TimestepType);
		}

		//make an entry from offset. 
		//offset MUST be the start of an entry, i.e. an Index Descriptor
		CellTimelineImpl::Entry MakeEntry(uint8_t* offset) const
		{
			Entry e;
			e.ID = JoinBytes<IDType>(offset);
			offset += sizeof(IDType);
			e.timestep = JoinBytes<TimestepType>(offset);
			offset += sizeof(TimestepType);
			IDType id = e.ID;
			id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));
			int i = 0;
			for (int i = 0; i < std::popcount(id); i++)
				e.values.push_back(JoinBytes<ValueType>(offset + i * sizeof(ValueType)));

			return e;
		}

		//set 32nd bit if last is true (last cell in tissue)
		void SetIDLastCellInTissue(IDType& id, bool last)
		{
			//set last bit if last (last cell in tissue)
			if (last)
				id |= (One::value << m_LastTissueBit);
			else
				id &= ~(One::value << m_LastTissueBit);
		}

		//unset the 31st bit of the last cell added to say it is no longer the last cell.
		//used when new cell is added
		void UnSetIDPreviousLastCellInTimeline()
		{
			//last entry is no longer the last cell
			IDType lastID = JoinBytes<IDType>(m_LastEntryByte);
			lastID &= ~(One::value << m_LastCellBit);
			memcpy(m_LastEntryByte, &lastID, sizeof(IDType));
		}

		//unset the 32st bit of the last cell added to say it is no longer the last cell in tissue.
		//used when new cell is by the TissueTimeline. 
		void UnSetIDPreviousLastCellInTissue()
		{
			IDType lastID = JoinBytes<IDType>(m_LastEntryByte);
			lastID &= ~(One::value << m_LastTissueBit);
			memcpy(m_LastEntryByte, &lastID, sizeof(IDType));
		}

		//retrives the cell at certain index that had no changes.
		Cell RetrieveNoChangesCell(size_t index) const
		{
			assert(index != 0 && "Cannot have first cell without changes");
			assert(index <= m_NumEntries - 1 && "Index out of range");

			IDType id = JoinBytes<IDType>(m_DataBuffer);
			size_t current = 0;
			uint8_t* offset = m_DataBuffer + sizeof(IDType) + sizeof(TimestepType);

			Cell cell;

			while (current <= index - 1)
			{
				id &= ~((One::value << m_LastTissueBit) | (One::value << m_LastCellBit));

				//logic to handle retrieval 
				uint8_t* i = offset;
				for (int idIndex = 0; i < offset + std::popcount(id) * sizeof(ValueType); i += sizeof(ValueType), idIndex++)
				{
					while (!(id & (One::value << idIndex)))
						idIndex++;
					cell.SetProperty(idIndex, JoinBytes<ValueType>(i));
				}

				//logic to go to next entry
				offset = i;
				id = JoinBytes<IDType>(offset);
				offset += sizeof(IDType) + sizeof(TimestepType);
				current++;
			}
			return cell;
		}

		//for move asign and constructor
		void Move(CellTimelineImpl&& other) noexcept
		{
			if (this != &other)
			{
				FreeBuffer();
				m_DataBuffer = other.m_DataBuffer;
				other.m_DataBuffer = nullptr;

				m_NumEntries = other.m_NumEntries;
				m_BufferCap = other.m_BufferCap;
				m_BufferSize = other.m_BufferSize;
				m_LastEntryByte = other.m_LastEntryByte;
				m_WriteEnd = other.m_WriteEnd;
				for (int i = 0; i < NumProperties; i++)
					m_LastPropertyChange[i] = other.m_LastPropertyChange[i];
			}
		}

	private:
		//free the aligned buffer
		void FreeBuffer()
		{
#ifdef _WIN32
			_aligned_free(m_DataBuffer);
#else
			std::free(m_DataBuffer);
#endif
		}

	private:
		//how many cell entries we have
		size_t m_NumEntries = 0;

	private:
		//raw buffer of bytes that stores our timeline.
		//is 4 byte aligned when allocated
		uint8_t* m_DataBuffer = nullptr;

		//data buffer is 4 byte aligned
		static constexpr int m_DataBufferAlignment = 32;

		//number of bytes that we have space for
		size_t m_BufferCap = 1024;

		//number of bytes written
		size_t m_BufferSize = 0;

		//first byte in the start of last entry. since finding a particular entry 
		//requires looking at all the entries before it, storing this helps.
		uint8_t* m_LastEntryByte = nullptr;

		//this is the last byte of last entry. this is where we can start writing 
		//new entries
		uint8_t* m_WriteEnd = nullptr;

		//this is the address of the property that was last changed.
		uint8_t* m_LastPropertyChange[NumProperties]{};

		static constexpr uint8_t m_LastCellBit = sizeof(IDType) * 8 - 2;
		static constexpr uint8_t m_LastTissueBit = sizeof(IDType) * 8 - 1;
	};

	template <size_t NumProperties>
	struct IndexDescTypeHelper
	{
		//go 2 less than number of bits since we use 2 bits to indicate other things
		static_assert(NumProperties <= 62, "Maximum number of properties can only be 62");
		static_assert(NumProperties > 0, "Number of properties cannot be 0 or less");

		using type =
			std::conditional_t<(NumProperties <= 6), uint8_t,
			std::conditional_t<(NumProperties <= 14), uint16_t,
			std::conditional_t<(NumProperties <= 30), uint32_t, uint64_t>>>;
	};

	template<class ValueType, class TimestepType, size_t NumProperties>
	class CellTimeline :
		public CellTimelineImpl<ValueType, TimestepType,
		typename IndexDescTypeHelper<NumProperties>::type, NumProperties>
	{
	public:
		using ImplT = CellTimelineImpl<ValueType, TimestepType,
			typename IndexDescTypeHelper<NumProperties>::type, NumProperties>;

		CellTimeline(const Cell& startingCell, TimestepType timestep, bool last) :
			ImplT{ startingCell, timestep, last }
		{
		}
	};

	//for convenience 
	//values = float, timestep = float
	template<size_t NumProperties>
	using CellTimelineFF = CellTimeline<float, float, NumProperties>;

	//values = double, timestep = float
	template<size_t NumProperties>
	using CellTimelineDF = CellTimeline<double, float, NumProperties>;

	//values = double, timestep = double
	template<size_t NumProperties>
	using CellTimelineDD = CellTimeline<double, double, NumProperties>;

	//values = float, timestep = double
	template<size_t NumProperties>
	using CellTimelineFD = CellTimeline<float, double, NumProperties>;


	/////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	// Baiscally a vector of CellTimeline. Works the same say as Tissue except that
	// instead of storing Cell, it stores CellTimeline in std::vector. See comment in Tissue.h
	// Downside: We are storing the same timesteps for every cell in tissue. 
	// So thats repeated data storage, but changing this would require entirely new implementation of 
	// TissueTimeline class that doesnt use CellTimeline and i aint doin that.

	template<size_t width, size_t height, size_t depth,
		class ValueType, class TimestepType, class IDType, size_t NumProperties>
	class TissueTimelineImpl
	{
	public:
		using CellTimelineType = CellTimelineImpl<ValueType, TimestepType, IDType, NumProperties>;

		struct Version
		{
			Version(size_t width, size_t height, size_t depth) {}
			TimestepType timestep = 0;
			Tissue<width, height, depth> tissue;
		};

		template<size_t vwidth, size_t vheight, size_t vdepth>
		struct VersionProperty
		{
			VersionProperty() {}
			float GetValue(int i) { return values[i]; }
			float SetValue(int i, float v) { values[i] = v; }
			float GetValue(int x, int y, int z) { return values[Index(x, y, z)]; }
			float SetValue(int x, int y, int z, float v) { values[Index(x, y, z)] = v; }
			float timestep = 0;

		private:
			size_t Index(size_t x, size_t y, size_t z)
			{
				assert((x <= vwidth - 1) && (y <= vheight - 1) && (z <= vdepth - 1) &&
					"Index out of range");
				return values[z * (vwidth * vheight) + (y * vwidth + x)];
			}

		private:
			float values[vwidth * vheight * vdepth];
		};

	public:
		TissueTimelineImpl(const Tissue<width, height, depth>& startingTissue, TimestepType timestep)
		{
			m_Timelines.resize(width * height * depth);
			for (int x = 0; x < startingTissue.Width(); x++)
			{
				for (int y = 0; y < startingTissue.Height(); y++)
				{
					for (int z = 0; z < startingTissue.Depth(); z++)
					{
						bool last = (x + 1) * (y + 1) * (z + 1) == width * height * depth;
						m_Timelines[Index(x, y, z)] =
							std::move(CellTimelineType{ startingTissue.GetCellAt(x, y, z), timestep, last });
					}
				}
			}
			m_Size++;
		}

	public:
		//iterator - just a wrapper around CellTimeline iterator
		class Iterator
		{
		public:
			Iterator() {}
			//Iterator does NOT modify timeline
			Iterator(TissueTimelineImpl* timeline, size_t index) :
				timeline{ timeline }
			{
				for (int x = 0; x < timeline->Width(); x++)
				{
					for (int y = 0; y < timeline->Height(); y++)
					{
						for (int z = 0; z < timeline->Depth(); z++)
						{
							m_Iterators[timeline->Index(x, y, z)] =
								timeline->m_Timelines[timeline->Index(x, y, z)].begin(index);
						}
					}
				}
			}

			Version operator*()
			{
				Version v{ timeline->Width(), timeline->Height(), timeline->Depth() };

				for (int x = 0; x < timeline->Width(); x++)
				{
					for (int y = 0; y < timeline->Height(); y++)
					{
						for (int z = 0; z < timeline->Depth(); z++)
						{
							typename CellTimelineType::Version cv = *m_Iterators[timeline->Index(x, y, z)];
							v.tissue.SetCellAt(x, y, z, cv.cell);
							v.timestep = cv.timestep;
						}
					}
				}
				return v;
			}

			VersionProperty<width, height, depth> GetVersionProperty(int prop)
			{
				VersionProperty<width, height, depth> vp{};
				for (int x = 0; x < timeline->Width(); x++)
				{
					for (int y = 0; y < timeline->Height(); y++)
					{
						for (int z = 0; z < timeline->Depth(); z++)
						{
							typename CellTimelineType::VersionProperty cv =
								m_Iterators[timeline->Index(x, y, z)].GetVersionProperty(prop);
							vp.SetValue(x, y, z, cv.value);
							vp.timestep = cv.timestep;
						}
					}
				}
				return vp;
			}

			CellTimelineType::VersionProperty GetCellVersionProperty(size_t index, int prop)
			{
				return m_Iterators[index].GetVersionProperty(prop);
			}

			CellTimelineType::VersionProperty GetCellVersionProperty(size_t x, size_t y, size_t z, int prop)
			{
				return m_Iterators[timeline->Index(x, y, z)].GetVersionProperty(prop);
			}

			uint32_t VersionChanges(size_t x, size_t y, size_t z)
			{
				return m_Iterators[timeline->Index(x, y, z)].VersionChanges();
			}

			Iterator& operator++()
			{
				for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
				{
					++m_Iterators[i];
				}
				return *this;
			}

			Iterator& operator++(int)
			{
				Iterator& tmp = *this;
				for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
				{
					++m_Iterators[i];
				}
				return tmp;
			}

			Iterator& operator+=(int v)
			{
				if (v <= 0)
					return *this;
				for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
				{
					m_Iterators[i] += v;
				}
				return *this;
			}

			// can compare any CellTimeline::Iterator for comparisions
			// since TissueTimeline gurantees every CellTimeline will 
			// have the same number of cells

			friend bool operator==(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Iterators[0] == i2.m_Iterators[0];
			}

			friend bool operator!=(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Iterators[0] != i2.m_Iterators[0];
			}

			friend bool operator<(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Iterators[0] < i2.m_Iterators[0];
			}

			friend bool operator<=(const Iterator& i1, const Iterator& i2)
			{
				return i1.m_Iterators[0] <= i2.m_Iterators[0];
			}

		private:
			CellTimelineType::Iterator m_Iterators[width * height * depth];
			TissueTimelineImpl* timeline = nullptr;
		};

	public:
		Iterator begin()
		{
			return Iterator{ this, 0 };
		}

		Iterator begin(size_t index)
		{
			assert(index <= m_Size);
			return Iterator{ this, index };
		}

		Iterator end()
		{
			return Iterator{ this, m_Size - 1 };
		}

	public:
		//adds the given tissue to the timeline
		void Add(const Tissue<width, height, depth>& tissue, TimestepType timestep)
		{
			for (int x = 0; x < tissue.Width(); x++)
			{
				for (int y = 0; y < tissue.Height(); y++)
				{
					for (int z = 0; z < tissue.Depth(); z++)
					{
						bool last = (x + 1) * (y + 1) * (z + 1) == width * height * depth;
						m_Timelines[Index(x, y, z)].Add(tissue.GetCellAt(x, y, z), timestep, last);
					}
				}
			}
			m_Size++;
		}

		//gets the given version of the tissue from the timeline
		Version GetVersion(size_t index) const
		{
			Version version{ Width(), Height(), Depth() };
			for (int x = 0; x < Width(); x++)
			{
				for (int y = 0; y < Height(); y++)
				{
					for (int z = 0; z < Depth(); z++)
					{
						typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].GetVersion(index);
						version.tissue.SetCellAt(x, y, z, v.cell);
						version.timestep = v.timestep;
					}
				}
			}
			return version;
		}

		//get the changes in this particular version
		int VersionChanges(size_t x, size_t y, size_t z, size_t index) const
		{
			return m_Timelines[Index(x, y, z)].VersionChanges(index);
		}

		//get the last tissue from the timeline
		Version LastVersion() const
		{
			Version version{ Width(), Height(), Depth() };
			for (int x = 0; x < Width(); x++)
			{
				for (int y = 0; y < Height(); y++)
				{
					for (int z = 0; z < Depth(); z++)
					{
						typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].LastVersion();
						version.tissue.SetCellAt(x, y, z, v.cell);
						version.timestep = v.timestep;
					}
				}
			}
			return version;
		}

		//get the first tissue from the timeline
		Version FirstVersion() const
		{
			Version version{ Width(), Height(), Depth() };
			for (int x = 0; x < Width(); x++)
			{
				for (int y = 0; y < Height(); y++)
				{
					for (int z = 0; z < Depth(); z++)
					{
						typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].FirstVersion();
						version.tissue.SetCellAt(x, y, z, v.cell);
						version.timestep = v.timestep;
					}
				}
			}
			return version;
		}

		//number of tissues in the timeline
		size_t Size() const
		{
			return m_Size;
		}

		//we give out cell timeline by const reference because we dont wanna let anyone
		//add to the timeline since we have to enfore the size rule of the tissue (if
		//a cell is added to one timeline, it has to be added to all the other timelines
		//to maintain the shape of the tissue)
		const CellTimelineType& GetCellTimeline(size_t x, size_t y, size_t z) const
		{
			return m_Timelines[Index(x, y, z)];
		}

	public:
		size_t Width() const { return width; }
		size_t Height() const { return height; }
		size_t Depth() const { return depth; }

	private:
		size_t Index(size_t x, size_t y, size_t z) const
		{
			assert((x <= width - 1) && (y <= height - 1) && (z <= depth - 1) &&
				"Index out of range");
			return z * (width * height) + (y * width + x);
		}

	private:
		size_t m_Size = 0;

	private:
		std::vector<CellTimelineType> m_Timelines;
	};

	template<size_t x, size_t y, size_t z, class ValueType, class TimestepType, size_t NumProperties>
	class TissueTimeline :
		public TissueTimelineImpl<x, y, z,
		ValueType, TimestepType,
		typename IndexDescTypeHelper<NumProperties>::type, NumProperties>
	{
	public:
		using ImplT = TissueTimelineImpl<x, y, z,
			ValueType, TimestepType,
			typename IndexDescTypeHelper<NumProperties>::type, NumProperties>;

		TissueTimeline(const Tissue<x, y, z>& startingTissue, TimestepType timestep) :
			ImplT{ startingTissue, timestep }
		{
		}
	};

	//for convenience 
	//values = float, timestep = float
	template<size_t x, size_t y, size_t z, size_t NumProperties>
	using TissueTimelineFF = TissueTimeline<x, y, z, float, float, NumProperties>;

	//values = double, timestep = float
	template<size_t x, size_t y, size_t z, size_t NumProperties>
	using TissueTimelineDF = TissueTimeline<x, y, z, double, float, NumProperties>;

	//values = double, timestep = double
	template<size_t x, size_t y, size_t z, size_t NumProperties>
	using TissueTimelineDD = TissueTimeline<x, y, z, double, double, NumProperties>;

	//values = float, timestep = double
	template<size_t x, size_t y, size_t z, size_t NumProperties>
	using TissueTimelineFD = TissueTimeline<x, y, z, float, double, NumProperties>;
}