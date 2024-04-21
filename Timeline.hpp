#pragma once
#include <inttypes.h>
#include <memory>
#include <vector>
#include <assert.h>
#include <string>
#include <cstdlib>
#include "Tissue.hpp"

//theese are only used in this file and undefed at the end
#if defined(__GNUC__) || defined(__clang__) 
	#define LIKELY(expr) (__builtin_expect(!!(expr), 1))
	#define UNLIKELY(expr) (__builtin_expect(!!(expr), 0))

#elif defined(_MSC_VER) 
	#define LIKELY(expr) (expr)[[likely]]
	#define UNLIKELY(expr) (expr)[[unlikely]]

#else
	#define LIKELY(expr) (expr)
	#define UNLIKELY(expr) (expr)
#endif

//TODO:HANDLE ERRORS 

//////////////////////////////////////////////////////////////////////////////
////Dynamic buffer of generic bytes that is used to hold the timeline data. //
////The data that can be pushed to this buffer can be one of theese types:  //
////ValueType, TimestepType or IDType. Can only pushback no remove		    //
//////////////////////////////////////////////////////////////////////////////

template<class ValueType, class TimestepType, class IDType>
class DynamicDataBuffer
{
private:
	//align to the size to largest type in the buffer
	template <class ValueType, class TimestepType, class IDType>
	struct AlignmentReqImpl //alignment requirement
	{
		using Type =
			typename std::conditional_t<(sizeof(ValueType) > sizeof(TimestepType)), ValueType,
			typename std::conditional_t<(sizeof(TimestepType) > sizeof(IDType)), TimestepType, IDType>>;
		static constexpr size_t Size = sizeof(Type) * 8;
	};

public:
	using AlignmentReq = AlignmentReqImpl<ValueType, TimestepType, IDType>;

public:
	DynamicDataBuffer() 
	{

	}

	DynamicDataBuffer(size_t cap) 
	{ 
		Alloc(cap); 
	}

	DynamicDataBuffer(const DynamicDataBuffer& other)
	{
		m_Buffer = static_cast<uint8_t*>(Alloc(other.m_Capacity));
		if LIKELY((m_Buffer))
		{
			memcpy(m_Buffer, other.m_Buffer, other.m_Size);
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;
		}
	}

	DynamicDataBuffer(DynamicDataBuffer&& other) noexcept
	{
		//steal
		m_Buffer = other.m_Buffer;
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;

		//byebye
		other.m_Buffer = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}

	DynamicDataBuffer& operator=(const DynamicDataBuffer& other)
	{
		if LIKELY((this != &other))
		{
			void* newBuffer = Realloc(other.m_Capacity);
			if LIKELY((newBuffer))
			{
				std::memcpy(newBuffer, other.m_Buffer, other.m_Size);
				Free(m_Buffer);
				m_Buffer = static_cast<uint8_t*>(newBuffer);
				m_Size = other.m_Size;
				m_Capacity = other.m_Capacity;
			}
		}
		return *this;
	}

	DynamicDataBuffer& operator=(DynamicDataBuffer&& other) noexcept
	{
		if LIKELY((this != &other))
		{
			Free(m_Buffer);

			//steal
			m_Buffer = other.m_Buffer;
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;

			//byebye
			other.m_Buffer = nullptr;
			other.m_Size = 0;
			other.m_Capacity = 0;
		}
		return *this;
	}

	~DynamicDataBuffer()
	{ 
		Free(m_Buffer); 
	};

public:
	//Add a data of type T (T can be ValueType, TimestepType or IDType) to the buffer.
	template<class T>
	void PushBack(T data)
	{
		//if this static assert fails, there is a mismatch in type that is 
		//being given to Timeline::PushBack function.
		//Example: Creating CellTimelineDF and calling PushBack on it with
		//Astrocyte<float>.
		//(either it should be CellTimelineFF or Astrocyte<double>)
		static_assert(
			std::is_same_v<T, IDType> || std::is_same_v<T, ValueType> || std::is_same_v<T, TimestepType>,
			"Mismatched types"
		);

		if UNLIKELY(m_Size + sizeof(T) > m_Capacity)
		{
			m_Buffer = static_cast<uint8_t*>(Realloc(m_Size * 2));
		}
		memcpy(m_Buffer + m_Size, &data, sizeof(T));
		m_Size += sizeof(T);
	}

	template<class T>
	void Set(T data, uint8_t* ptr)
	{
		assert(ptr < m_Buffer + m_Size);
		memcpy(ptr, &data, sizeof(T));
	}

	template<class T>
	void Set(T data, size_t offset)
	{
		assert(m_Buffer + offset < m_Buffer + m_Size);
		memcpy(m_Buffer + offset, &data, sizeof(T));
	}

	//T is what to get the type as.
	//WARNING: If PushBack() or Set() had stored a double at ptr
	//and then Get<float> is called at that ptr, it is UNDEFINED BEHAVIOUR.
	//It is up to caller to make sure that it is retrieved as right type, 
	//the function does no safety checks.
	template <class T>
	T Get(uint8_t* ptr) const
	{
		//If this fails, there is something wrong in the timeline implementation classes
		static_assert(
			std::is_same_v<T, IDType> || std::is_same_v<T, ValueType> || std::is_same_v<T, TimestepType>,
			"Mismatched types"
		);

		assert(ptr < m_Buffer + m_Size);

		if constexpr (std::endian::native == std::endian::little)
		{
			return *reinterpret_cast<T*>(ptr);
		}
		else if (std::endian::native == std::endian::big)
		{
			T reverse = 0;
			for (size_t i = 0; i < sizeof(T); i++)
			{
				reverse = (reverse << 8) | ptr[i];
			}
			return reverse;
		}
	}

	//NOTE: giving this function 0 or NULL will be ambigious with Get(uint8_t*)
	template <class T>
	T Get(size_t offset) const
	{
		return Get<T>(m_Buffer + offset);
	}

	//how many bytes have been written to the buffer
	size_t BytesWritten() const
	{
		return m_Size;
	}

	void Reserve(size_t cap)
	{
		Realloc(cap);
	}

private:
	void* Alloc(size_t cap)
	{
		void* ptr = nullptr;

#ifdef _WIN32 // windows
		if LIKELY((ptr = _aligned_malloc(cap, AlignmentReq::Size)))
		{
			m_Capacity = cap;
		}

#elif defined(__APPLE__) // macos and ios
		if LIKELY((posix_memalign(&ptr, AlignmentReq::Size, cap) == 0))
		{
			m_Capacity = cap;
		}

#else // linux and other unix-like systems
		if LIKELY((posix_memalign(&ptr, AlignmentReq::Size, cap) == 0))
		{
			m_Capacity = cap;
		}
#endif
		return ptr;
	}

	void* Realloc(size_t cap)
	{
		if UNLIKELY(!m_Buffer)
		{
			return Alloc(cap);
		}

		void* newPtr = Alloc(cap);
		if LIKELY(newPtr)
		{
			memcpy(newPtr, m_Buffer, m_Size);
			Free(m_Buffer);
		}

		return newPtr;
	}

	void Free(void* ptr) 
	{
#ifdef _WIN32 // Windows
		_aligned_free(ptr);

#else // macOS, iOS, Linux, and other Unix-like systems
		free(ptr);
#endif
	}

private:
	size_t m_Size = 0;
	size_t m_Capacity = 0;
	uint8_t* m_Buffer = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Implementation class for the CellTimeline		.									  //	
// CellTimeline is a class that stores a version of a cell at each timestep.		      //
// Data in stored in a buffer in format:											      //
// ID TIMESTEP VALUES ID TIMESTEP VALUES ID TIMESTEP VALUES ...						      //
// ID (Index Descriptor) describes how many values are stored. It is an int			      //
// and the position of the bit set tells us which property the values is.			      //
// 00000000000000000000000000000011 means that the first an second properties		      //
// in the cell changed. The changes are the float VALUES. They are the new value of       //
// properties. If 2 bits in ID are set, it is expected that there are 2 VALUES.		      //
// TIMESTEP is the timestep.														      //
// The last bit in ID set means it is the last cell in the tissue and second last bit     //
// in ID set means it is the last cell in the timeline								      //
//NOTE: ID isn't always int. It is choosen based on number of properties the cell has.    //
//VALUES and TIMESTEP types can be specified to be any numeric type					      //
//																					      //
//Some memory benchmark numbers for Timeline: 										      //
//Values here represent the percent of the original value.							      //
//Example: 25% means the memory consumption is 25% of the original 					      //
//When NumProperties == 30 and 5 properties change: 23.38% 							      //
//^^This one is generally what happens in our case									      //
//When NumProperties == 60 and 5 properties change: 14.01% 							      //
//When NumProperties == 60 and 40 properties change: 70.81% 						      //
//When NumProperties == 5 and 2 properties change: 54.66% 							      //
//When NumProperties == num properties changed: 104.16% 							      //
//^^DO NOT USE TIMELINE if all the properties change every time. In this case		      //
//we end up using around 4% more memory then we should. 							      //
//Best case: We have least amount of properties and least amount of changes			      //
//Worst case: We have most amount of properties and most amount of changes			      //
//																					      //
//Speed benchmark numbers(With 100 million Cell and timestep, 						      //
//ValueType and TimestepType = float, float; Release mode)								  //
//Processor: AMD Ryzen 9 5900X 12-Core Processor 3.70 GHz							      //
//Ram: 32GB																			      //
//PushBack:																			      //
//When NumProperties == 30 and 5 properties change: 8 seconds						      //
//When NumProperties == 60 and 5 properties change: 11 seconds						      //
//When NumProperties == 5 and 5 properties change: 5 seconds						      //
//When NumProperties == 30 and 10 properties change: 11 seconds						      //
//When NumProperties == 30 and 20 properties change: 15 seconds						      //
//Iterating through the whole timeline using Iterator:								      //
//When NumProperties == 30 and 5 properties change: 2 seconds						      //
//Iterating through the whole timeline using GetVersion:							      //
//When NumProperties == 30 and 5 properties change: > hour(i stopped after that)	      //
//Point: USE ITERATOR if you need to retrieve more than 1 version					      //
//For retrieving a single entry, GetVersion will be slightly faster and use less memory   //
//than Iterator																		      //
////////////////////////////////////////////////////////////////////////////////////////////

//WARNING: Cell timeline can only store one specific type of cell that is provided to it as 
//template argument. It can accept any cell type as PushBack argument but only the cell that 
//is specified as the template argument should be pushed. If any other type of cell is pushed, 
//the class will do things that cause undefined behaviour. It will probably crash. Even if it 
//doesn't, it isn't the intended use case.

template<class Valuetype, class Timesteptype, class IDtype, class Cell>
class CellTimelineImpl
{
	static_assert(std::is_arithmetic_v<Valuetype>);
	static_assert(std::is_arithmetic_v<Timesteptype>);
	static_assert(std::is_base_of_v<StemCell<Valuetype>, Cell>);
	
public:
	using ValueType = Valuetype;
	using TimestepType = Timesteptype;
	using IDType = IDtype;

	using BaseCellType = StemCell<ValueType>;
	using CellType = Cell;//this is the type we retrieve the cell as
	static constexpr size_t NumProperties = CellType::NumProperties;
	static constexpr uint8_t LastCellBit = sizeof(IDType) * 8 - 2;
	static constexpr uint8_t LastTissueBit = sizeof(IDType) * 8 - 1;
	

private:
	template<class T> struct OneImpl;
	template<> struct OneImpl<uint64_t> { static constexpr uint64_t Value = 1i64; };
	template<> struct OneImpl<uint32_t> { static constexpr uint32_t Value = 1i32; };
	template<> struct OneImpl<uint16_t> { static constexpr uint16_t Value = 1i16; };
	template<> struct OneImpl<uint8_t> { static constexpr uint8_t Value = 1i8; };

public:
	using One = OneImpl<IDType>;

public:
	struct Version
	{
		ValueType timestep = 0;
		CellType cell{};
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
		std::vector<ValueType> values{};
	};

public:
	CellTimelineImpl()
	{
	}

	//can be used to preallocate buffer in bytes
	//TODO: CRASHES WHEN USING THIS CONSTRUCTOR. CHECK WHY
	/*CellTimelineImpl(size_t reserve) :
		m_Buffer{ reserve }
	{
	}*/

	//last is whether this is the last cell in the tissue
	CellTimelineImpl(const BaseCellType& startingCell, TimestepType timestep, bool last) 
	{
		PushBack(startingCell, timestep, last);
	}

	CellTimelineImpl(Version version, bool last)
	{
		PushBack(version.cell, version.timestep, last);
	}

	CellTimelineImpl(const std::initializer_list<Version>& entries, bool last)
	{
		for (auto& e : entries)
		{
			PushBack(e.cell, e.timestep, last);
		}
	}

	CellTimelineImpl(const CellTimelineImpl& other)
	{
		m_LastEntryOffset = other.m_LastEntryOffset;
		for (size_t i = 0; i < NumProperties; i++)
			m_LastPropChangeOffset[i] = other.m_LastPropChangeOffset[i];
		m_Size = other.m_Size;
		m_Buffer = other.m_Buffer;
	}

	CellTimelineImpl(CellTimelineImpl&& other) noexcept
	{
		m_LastEntryOffset = other.m_LastEntryOffset;
		for (size_t i = 0; i < NumProperties; i++)
			m_LastPropChangeOffset[i] = other.m_LastPropChangeOffset[i];
		m_Size = other.m_Size;
		m_Buffer = std::move(other.m_Buffer);
	}

	CellTimelineImpl& operator=(const CellTimelineImpl& other)
	{
		if LIKELY((this != &other))
		{
			m_LastEntryOffset = other.m_LastEntryOffset;
			for (size_t i = 0; i < NumProperties; i++)
				m_LastPropChangeOffset[i] = other.m_LastPropChangeOffset[i];
			m_Size = other.m_Size;
			m_Buffer = other.m_Buffer;
		}
	}

	CellTimelineImpl operator=(CellTimelineImpl&& other) noexcept
	{
		if LIKELY((this != &other))
		{
			m_LastEntryOffset = other.m_LastEntryOffset;
			for (size_t i = 0; i < NumProperties; i++)
				m_LastPropChangeOffset[i] = other.m_LastPropChangeOffset[i];
			m_Size = other.m_Size;
			m_Buffer = std::move(other.m_Buffer);
		}
	}

public:
	void PushBack(const BaseCellType& cell, TimestepType timestep, bool last)
	{
		PushBackImpl(cell, timestep, last);
	}

	
	void PushBack(const CellType& cell, TimestepType timestep, bool last)
	{
		PushBackImpl(cell, timestep, last);
	}

	//gets the given version of the cell from the timeline
	Version GetVersion(size_t index) const
	{
		Version version;
		Entry entry = EntryFromIndex(index);
		version.timestep = entry.timestep;

		if UNLIKELY((entry.values.empty()))
		{
			version.cell = RetrieveNoChangesCell(index);
			return version;
		}

		size_t idIndex = 0;
		bool set[NumProperties] = { 0 };
		for (size_t i = 0; i < entry.values.size(); i++)
		{
			while (!(entry.ID & (One::Value << idIndex)))
				idIndex++;
			version.cell[idIndex] = entry.values[i];
			set[idIndex] = true;
			idIndex++;
		}
		for (size_t i = 0; i < NumProperties; i++)
		{
			if (!set[i])
			{
				version.cell[idIndex] = m_Buffer.Get<ValueType>(m_LastPropChangeOffset[i]);
			}
		}
		return version;
	}

	Version operator[](size_t index) const
	{
		return GetVersion(index);
	}

	//get the changes in the last cell
	IDType GetLastChanges() const
	{
		return m_Buffer.Get<IDType>(m_LastEntryOffset);
	}

	//get the last cell from the timeline
	Version GetLastVersion() const
	{
		Version version;
		version.timestep = m_Buffer.Get<TimestepType>(m_LastEntryOffset + sizeof(IDType));
		for (size_t i = 0; i < NumProperties; i++)
			version.cell[i] = m_Buffer.Get<ValueType>(m_LastPropChangeOffset[i]);
		return version;
	}

	//get the first cell from the timeline
	Version GetFirstVersion() const
	{
		Version version;
		size_t start = sizeof(IDType);
		version.timestep = m_Buffer.Get<TimestepType>(start);
		start += sizeof(TimestepType);
		for (size_t i = 0; i < NumProperties; i++)
			version.cell[i] = m_Buffer.Get<ValueType>(start + i * sizeof(ValueType));
		return version;
	}

	//get the changes in the given version
	IDType GetVersionChanges(size_t index) const
	{
		return m_Buffer.Get<IDType>(EntryOffsetFromIndex(index));
	}
	
	void Reserve(size_t reserve)
	{
		return m_Buffer.Reserve(reserve);
	}

	size_t Size() const
	{
		return m_Size;
	}

	bool Empty() const
	{
		return !m_Size;
	}

	size_t SizeInBytes() const
	{
		return m_Buffer.BytesWritten();
	}

public:
	//If iterating through timeline is needed USE ITERATOR. It is MASSIVELY faster than
	//using GetVersion multiple times. Use GetVersion for one off Gets.
	//It is a standard iterator and works with most (std::)algorithms and functions.
	//There are being() and end() overloads of functions returning this iterator that
	//take index as argument as well. Not really standard but convenient. 
	//NOTE: This iterator isn't lightweight unlike most other iterators so try avoid 
	//copying. operator+ makes a copy.
	class Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag; 
		using value_type = Version; 
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

	public:
		Iterator() {}

		//iterator does NOT modify timeline
		Iterator(CellTimelineImpl* timeline, size_t index) :
			timeline{ timeline }
		{
			assert(index <= timeline->m_Size);

			m_Index = index;
			IDType id = timeline->m_Buffer.Get<IDType>(m_Entry);
			size_t current = 0;
			size_t offset = m_Entry + sizeof(IDType) + sizeof(TimestepType);

			while (current < index)
			{
				id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
				m_Entry = offset - sizeof(IDType) - sizeof(TimestepType);
				//logic to update last changed properties 
				size_t i = offset;
				int nbits = std::popcount(id);
				for (size_t idIndex = 0; i < offset + nbits * sizeof(ValueType); i += sizeof(ValueType), idIndex++)
				{
					while (!(id & (One::Value << idIndex)))
						idIndex++;
					m_LastChangedProp[idIndex] = i;
				}

				//logic to go to next entry
				offset = i;
				id = timeline->m_Buffer.Get<IDType>(offset);
				offset += sizeof(IDType) + sizeof(TimestepType);
				current++;
			}
		}

	public:
		Version operator*()
		{
			Version v;
			v.timestep = timeline->m_Buffer.Get<TimestepType>(m_Entry + sizeof(IDType));
			IDType id = timeline->m_Buffer.Get<IDType>(m_Entry);
			id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
			int idIndex = 0;
			bool set[NumProperties] = { 0 };
			for (size_t i = 0; i < std::popcount(id); i++)
			{
				while (!(id & (One::Value << idIndex)))
					idIndex++;
				size_t valueOffset = m_Entry + sizeof(IDType) + sizeof(TimestepType) + i * sizeof(ValueType);
				ValueType value = timeline->m_Buffer.Get<float>(valueOffset);
				v.cell[idIndex] = value;
				set[idIndex] = true;
				idIndex++;
			}
			for (size_t i = 0; i < NumProperties; i++)
			{
				if (!set[i])
					v.cell[i] = timeline->m_Buffer.Get<ValueType>(m_LastChangedProp[i]);
			}
			return v;
		}

		Version GetVersion(size_t index)
		{
			return operator*(index);
		}

		VersionProperty GetVersionProperty(size_t prop)
		{
			VersionProperty vp;
			vp.timestep = timeline->m_Buffer.Get<TimestepType>(m_Entry + sizeof(IDType));
			IDType id = timeline->m_Buffer.Get<IDType>(m_Entry);
			id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
			size_t idIndex = 0;
			for (size_t i = 0; i < std::popcount(id); i++)
			{
				while (!(id & (One::Value << idIndex)))
					idIndex++;
				size_t valueOffset = m_Entry + sizeof(IDType) + sizeof(TimestepType) + i * sizeof(ValueType);
				ValueType value = timeline->m_Buffer.Get<ValueType>(valueOffset);
				if (idIndex == prop)//case where every property changed
				{
					vp.value = value;
					return vp;
				}
				idIndex++;
			}
			for (size_t i = 0; i < NumProperties; i++)
			{
				if (i == prop)
				{
					vp.value = timeline->m_Buffer.Get<ValueType>(m_LastChangedProp[i]);
					return vp;
				}
			}
			assert(false && "Sum ting wong. Shouldn't ever reach here");
		}

		IDType GetVersionChanges()
		{
			return timeline->m_Buffer.Get<IDType>(m_Entry);
		}

		Iterator& operator++()
		{
			assert(m_Index <= timeline->m_Size && "Cant go past the end of the iterator");

			//update last changed so we dont miss anything when skipping entries
			IDType id = timeline->m_Buffer.Get<IDType>(m_Entry);
			id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
			size_t idIndex = 0;
			int nBits = std::popcount(id);
			for (size_t j = 0; j < nBits; j++)
			{
				while (!(id & (One::Value << idIndex)))
					idIndex++;
				m_LastChangedProp[idIndex] = m_Entry + sizeof(IDType) + sizeof(TimestepType) + j * sizeof(ValueType);
				idIndex++;
			}
			m_Index++;
			m_Entry += sizeof(IDType) + sizeof(TimestepType) + nBits * sizeof(ValueType);
			return *this;
		}

		Iterator& operator++(int)
		{
			Iterator& tmp = *this;
			++*this;
			return tmp;
		}

		Iterator& operator+=(size_t v)
		{
			for (size_t i = 0; i <= v; i++)
			{
				++*this;
			}
			return *this;
		}

		/*Iterator operator+(size_t v)
		{
			Iterator it = *this;
			for (size_t i = 0; i <= v; i++)
			{
				++it;
			}
			return it;
		}*/

		friend bool operator==(const Iterator& i1, const Iterator& i2)
		{
			return i1.m_Index == i2.m_Index;
		}

		friend bool operator!=(const Iterator& i1, const Iterator& i2)
		{
			return i1.m_Index != i2.m_Index;
		}

		friend bool operator<(const Iterator& i1, const Iterator& i2)
		{
			return i1.m_Index < i2.m_Index;
		}
		friend bool operator<=(const Iterator& i1, const Iterator& i2)
		{
			return i1.m_Index <= i2.m_Index;
		}

	private:
		size_t m_Index = 0;
		size_t m_Entry = 0;
		//offset to property that last changed
		size_t m_LastChangedProp[NumProperties] = { 0 };
		CellTimelineImpl* timeline = nullptr;
	};

public:
	CellTimelineImpl::Iterator begin() const
	{
		return Iterator{ this, 0 };
	}

	CellTimelineImpl::Iterator begin(size_t index) const
	{
		assert(index <= m_Size); 
		return Iterator{ this, index };
	}

	CellTimelineImpl::Iterator end(size_t index) const
	{
		assert(index <= m_Size);
		return Iterator{ this, index };
	}
	 
	CellTimelineImpl::Iterator end() const
	{
		return Iterator{ this, m_Size };
	}

private:
	template<class Cell>
	void PushBackImpl(const Cell& cell, TimestepType timestep, bool last)
	{
		static_assert(std::is_same_v<CellType, Cell> || std::is_base_of_v<BaseCellType, Cell>, 
			"Can only push back the cell type stored in this class or pointer to base"
			"that is pointing to that type");

		//FIRST ENTRY
		if UNLIKELY((m_Size == 0))
		{
			IDType id = (IDType)((1i64 << NumProperties) - 1);
			//this is now the last cell in the timeline
			id |= One::Value << LastCellBit;
			//set last bit if last (last cell in tissue)
			if UNLIKELY((last))
				id |= (One::Value << LastTissueBit);
			else
				id &= ~(One::Value << LastTissueBit);
			m_Buffer.PushBack(id);
			m_Buffer.PushBack(timestep);
			for (size_t i = 0; i < NumProperties; i++)
				m_Buffer.PushBack(cell.At(i));
			//init last changed properties pointers to the first entry 
			//respective properties in the first entry
			for (size_t i = 0; i < NumProperties; i++)
			{
				m_LastPropChangeOffset[i] = sizeof(IDType) + sizeof(TimestepType) + (i * sizeof(ValueType));
			}
			m_Size++;
			return;
		}

		//GENERAL CASE
		Entry newEntry = EntryFromCell(cell);

		//this is now the last cell in the timeline
		newEntry.ID |= One::Value << LastCellBit;
		IDType lastID = m_Buffer.Get<IDType>(m_LastEntryOffset);
		//set last bit if last (last cell in tissue) and unset last one
		if UNLIKELY((last))
		{
			newEntry.ID |= (One::Value << LastTissueBit);
			lastID &= ~(One::Value << LastTissueBit);
			m_Buffer.Set(lastID, m_LastEntryOffset);
		}
		else
		{
			newEntry.ID &= ~(One::Value << LastTissueBit);
		}

		//unset the last cell and last tissue bits in the previous entry
		if UNLIKELY((last))
		{
			lastID &= ~(One::Value << LastTissueBit);
		}
		lastID &= ~(One::Value << LastCellBit);

		m_Buffer.PushBack(newEntry.ID);
		m_Buffer.PushBack(timestep);

		if UNLIKELY(newEntry.values.empty())
		{
			m_LastEntryOffset = m_Buffer.BytesWritten() - sizeof(IDType) - sizeof(TimestepType);
			m_Size++;
			return;
		}

		size_t idIndex = 0;
		for (size_t i = 0; i < newEntry.values.size(); i++)
		{
			m_Buffer.PushBack(newEntry.values[i]);
			while (!(newEntry.ID & (One::Value << idIndex)))
				idIndex++;
			m_LastPropChangeOffset[idIndex] = m_Buffer.BytesWritten() - sizeof(ValueType);
			idIndex++;
		}

		m_LastEntryOffset = m_Buffer.BytesWritten() - (sizeof(ValueType) * newEntry.values.size()) -
			sizeof(TimestepType) - sizeof(IDType);
		m_Size++;
	}

	//grabs an entry from the buffer based on index and returns it. index is the
	//position of the entry in the buffer.
	Entry EntryFromIndex(size_t index) const
	{
		size_t e = EntryOffsetFromIndex(index);
		return MakeEntry(e);
	}

	//get the offset to the beginning of the given entry in the buffer
	size_t EntryOffsetFromIndex(size_t index) const
	{
		assert(index >= 0 && index < m_Size && "Index out of range");

		IDType id = m_Buffer.Get<IDType>((size_t)0);
		size_t current = 0;
		size_t offset = sizeof(IDType) + sizeof(TimestepType);
		while (current != index)
		{
			id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
			offset += std::popcount(id) * sizeof(ValueType);
			id = m_Buffer.Get<IDType>(offset);
			offset += sizeof(IDType) + sizeof(TimestepType);
			current++;
		}
		//above loop steps past the ID and timestep of the entry we want, so go back
		return offset - sizeof(IDType) - sizeof(TimestepType);
	}

	//looks at the difference between the given cell and the last cell.
	//constructs an entry with correct id and values that were changed.
	CellTimelineImpl::Entry EntryFromCell(const BaseCellType& cell) const
	{
		Entry e;
		const ValueType epsilon = static_cast<ValueType>(0.001);
		for (size_t i = 0; i < NumProperties; i++)
		{
			e.ID &= ~(One::Value << i);
			ValueType v = std::abs(cell.At(i) - m_Buffer.Get<ValueType>(m_LastPropChangeOffset[i]));
			if (v > epsilon)
			{
				e.ID |= One::Value << i;
				e.values.push_back(cell.At(i));
			}
		}
		return e;
	}

	//make an entry from offset. 
	//offset MUST be the start of an entry, i.e. an Index Descriptor
	CellTimelineImpl::Entry MakeEntry(size_t offset) const
	{
		Entry e;
		e.ID = m_Buffer.Get<IDType>(offset);
		offset += sizeof(IDType);
		e.timestep = m_Buffer.Get<TimestepType>(offset);
		offset += sizeof(TimestepType);
		IDType id = e.ID;
		id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));
		size_t i = 0;
		for (size_t i = 0; i < std::popcount(id); i++)
			e.values.push_back(m_Buffer.Get<ValueType>(offset + i * sizeof(ValueType)));

		return e;
	}

	//retrives the cell at certain index that had no changes.
	BaseCellType RetrieveNoChangesCell(size_t index) const
	{
		assert(index != 0 && "Cannot have first cell without changes");
		assert(index <= m_Size - 1 && "Index out of range");

		IDType id = m_Buffer.Get<IDType>((size_t)0);
		size_t current = 0;
		size_t offset = sizeof(IDType) + sizeof(TimestepType);

		BaseCellType cell;

		while (current <= index - 1)
		{
			id &= ~((One::Value << LastTissueBit) | (One::Value << LastCellBit));

			//logic to handle retrieval 
			size_t i = offset;
			for (size_t idIndex = 0; i < offset + std::popcount(id) * sizeof(ValueType); i += sizeof(ValueType), idIndex++)
			{
				while (!(id & (One::Value << idIndex)))
					idIndex++;
				cell[idIndex] = m_Buffer.Get<ValueType>(i);
			}

			//logic to go to next entry
			offset = i;
			id = m_Buffer.Get<IDType>(offset);
			offset += sizeof(IDType) + sizeof(TimestepType);
			current++;
		}
		return cell;
	}

private:
	//offset to first byte of an entry in the buffer
	size_t m_LastEntryOffset = 0;
	//offset to property value that last changed in the buffer
	size_t m_LastPropChangeOffset[NumProperties] = { 0 }; 
	///number of entries in the buffer
	size_t m_Size = 0;
	DynamicDataBuffer<ValueType, TimestepType, IDType> m_Buffer;
};

//determine what type to use as IndexDescriptor based on number of properties we have
template <size_t NumProperties>
struct IndexDescTypeHelper
{
	//go 2 less than number of bits since we use 2 bits to indicate other things
	static_assert(NumProperties <= 62, "Maximum number of properties can only be 62");
	static_assert(NumProperties > 0, "Number of properties cannot be 0 or less");

	using Type =
		std::conditional_t<(NumProperties <= 6), uint8_t,
		std::conditional_t<(NumProperties <= 14), uint16_t,
		std::conditional_t<(NumProperties <= 30), uint32_t, uint64_t>>>;
};

////////////////////////////////////
////////////////////////////////////
////// Main CellTimeline class /////
////////////////////////////////////
////////////////////////////////////

template<class ValueType, class TimestepType, class Cell>
class CellTimeline :
	public CellTimelineImpl<ValueType, TimestepType,
	typename IndexDescTypeHelper<Cell::NumProperties>::Type,
	Cell>
{
public:
	using ImplType = CellTimelineImpl<ValueType, TimestepType,
		typename IndexDescTypeHelper<Cell::NumProperties>::Type, 
		Cell>;

	CellTimeline() {}

	//TODO: CHECK WHY THIS CAUSES CRASH
	/*CellTimeline(size_t reserve) :
		ImplType{ reserve }
	{
	}*/

	CellTimeline(const typename ImplType::CellType& startingCell,
		TimestepType timestep, bool last) :
		ImplType{ startingCell, timestep, last }
	{
	}

	CellTimeline(typename ImplType::Version version, bool last) :
		ImplType{ version, last }
	{
	}

	CellTimeline(const std::initializer_list<typename ImplType::Version>& entries, bool last) :
		ImplType{ entries, last }
	{
	}
};

//for convenience 
//values = float, timestep = float
template<template<class> class Cell>
using CellTimelineFF = CellTimeline<float, float, Cell<float>>;

//values = double, timestep = float
template<template<class> class Cell>
using CellTimelineDF = CellTimeline<double, float, Cell<double>>;

//values = double, timestep = double
template<template<class> class Cell>
using CellTimelineDD = CellTimeline<double, double, Cell<double>>;

//values = float, timestep = double
template<template<class> class Cell>
using CellTimelineFD = CellTimeline<float, double, Cell<float>>;


//TissueTimelineImlp as it is, is VERY SLOW. It stores a vector of Celltimelines
//to store the cells. However, this is very inefficient for us since we generally
//work with all the cells in the tissue at a particular index. What this means:
//1. We load in buffer of first cell timeline into cache. 
//2. Get the cell at i-th index from the timeline. 
//3. Flush the cache and load in the second cell timeline
//4. Get the cell at i-th index from the timeline. 
//5. Flush the cache and load in the third cell timeline.
// and so on and so on.
//In other words, we have a cache miss each time we call GetVersion on tissue. This is 
//very slow. We want our memory layout so that we have 
//cell0Version0...cellnVersion0 cell0Version1...cellnVersion1 cell0Version2...cellnVersion2
//This way we have all the data that we access at once in cache. 
//However this means that TissueTimelineImpl can no longer be a simple wrapper around 
//CellTimeline and will have to be its own proper implementation. But it is also true that
//TissueTimeline speed will directly and proportionally affect the simulation speed. So 
//guess it will have to be implemented at some point. 
#pragma region OLD_PACKED_TISSUE_IMPLEMENTATION
//OLD PackedTissueTimeline implementation class.
/////////////////////////////////////////////////////////////////////////////////
////Wrapper over CellTimeline so that it provides nicer interface for tissues. //
/////////////////////////////////////////////////////////////////////////////////
//template<class ValueType, class TimestepType, class IDType, size_t NumProperties>
//class PackedTissueTimelineImpl
//{
//public:
//	using CellTimelineType = CellTimelineImpl<ValueType, TimestepType, IDType, NumProperties>;
//	using TissueType = PackedTissue<ValueType, NumProperties>;
//
//	struct Version
//	{
//		Version(size_t width, size_t height, size_t depth) :
//			width{ width }, height{ height }, depth{ depth }
//		{
//		}
//		TimestepType timestep = 0;
//		TissueType tissue{ width, height, depth };
//		size_t width, height, depth;
//	};
//
//private:
//	template<class VValueType, class VTimestepType>
//	struct VersionPropertyImpl
//	{
//		VersionPropertyImpl(size_t width, size_t height, size_t depth) :
//			width{ width }, height{ height }, depth{ depth }
//		{
//			values.resize(width* height* depth);
//		}
//		typename VValueType GetValue(int i) { return values[i]; }
//		void SetValue(int i, float v) { values[i] = v; }
//		typename VValueType GetValue(int x, int y, int z) { return values[Index(x, y, z)]; }
//		void SetValue(int x, int y, int z, float v) { values[Index(x, y, z)] = v; }
//		typename VTimestepType timestep = 0;
//
//	private:
//		size_t Index(size_t x, size_t y, size_t z)
//		{
//			assert((x <= width - 1) && (y <= height - 1) && (z <= depth - 1) &&
//				"Index out of range");
//			return values[z * (width * height) + (y * width + x)];
//		}
//
//	private:
//		size_t width, height, depth;
//		std::vector<typename VValueType> values;
//	};
//
//public:
//	using VersionProperty = VersionPropertyImpl<ValueType, TimestepType>;
//
//public:
//	PackedTissueTimelineImpl(size_t x, size_t y, size_t z, size_t reserve) :
//		m_Width{ x }, m_Height{ y }, m_Depth{ z }
//	{
//		m_Timelines.resize(Width()* Height()* Depth());
//		for (auto& timeline : m_Timelines)
//		{
//			timeline.Reserve(reserve);
//		}
//	}
//
//	PackedTissueTimelineImpl(size_t x, size_t y, size_t z) :
//		m_Width{ x }, m_Height{ y }, m_Depth{ z }
//	{
//		m_Timelines.resize(Width()* Height()* Depth());
//	}
//
//	PackedTissueTimelineImpl(size_t x, size_t y, size_t z, 
//		const TissueType& startingTissue, TimestepType timestep) :
//		m_Width{ x }, m_Height{ y }, m_Depth{ z }
//	{
//		m_Timelines.resize(Width()* Height()* Depth());
//		PushBack(startingTissue, timestep);
//	}
//
//	virtual ~PackedTissueTimelineImpl()
//	{
//	}
//
//public:
//	//iterator - just a wrapper around CellTimeline iterator
//	class Iterator
//	{
//	public:
//		using iterator_category = std::forward_iterator_tag;
//		using value_type = Version;
//		using difference_type = std::ptrdiff_t;
//		using pointer = value_type*;
//		using reference = value_type&;
//
//	public:
//		Iterator() {}
//		//Iterator does NOT modify timeline
//		Iterator(PackedTissueTimelineImpl* timeline, size_t index) :
//			timeline{ timeline }
//		{
//			m_Iterators.resize(timeline->Width() * timeline->Height() * timeline->Depth());
//			for (int x = 0; x < timeline->Width(); x++)
//			{
//				for (int y = 0; y < timeline->Height(); y++)
//				{
//					for (int z = 0; z < timeline->Depth(); z++)
//					{
//						m_Iterators[timeline->Index(x, y, z)] =
//							timeline->m_Timelines[timeline->Index(x, y, z)].begin(index);
//					}
//				}
//			}
//		}
//
//		Version operator*()
//		{
//			Version v{ timeline->Width(), timeline->Height(), timeline->Depth() };
//
//			for (int x = 0; x < timeline->Width(); x++)
//			{
//				for (int y = 0; y < timeline->Height(); y++)
//				{
//					for (int z = 0; z < timeline->Depth(); z++)
//					{
//						typename CellTimelineType::Version cv = *m_Iterators[timeline->Index(x, y, z)];
//						v.tissue.SetCellAt(x, y, z, cv.cell);
//						v.timestep = cv.timestep;
//					}
//				}
//			}
//			return v;
//		}
//
//		VersionProperty GetVersionProperty(int prop)
//		{
//			VersionProperty vp{ timeline->Width(), timeline->Height(), timeline->Depth() };
//			for (int x = 0; x < timeline->Width(); x++)
//			{
//				for (int y = 0; y < timeline->Height(); y++)
//				{
//					for (int z = 0; z < timeline->Depth(); z++)
//					{
//						typename CellTimelineType::VersionProperty cv =
//							m_Iterators[timeline->Index(x, y, z)].GetVersionProperty(prop);
//						vp.SetValue(x, y, z, cv.value);
//						vp.timestep = cv.timestep;
//					}
//				}
//			}
//			return vp;
//		}
//
//		CellTimelineType::VersionProperty GetCellVersionProperty(size_t index, int prop)
//		{
//			return m_Iterators[index].GetVersionProperty(prop);
//		}
//
//		CellTimelineType::VersionProperty GetCellVersionProperty(size_t x, size_t y, size_t z, int prop)
//		{
//			return m_Iterators[timeline->Index(x, y, z)].GetVersionProperty(prop);
//		}
//
//		uint32_t VersionChanges(size_t x, size_t y, size_t z)
//		{
//			return m_Iterators[timeline->Index(x, y, z)].GetVersionChanges();
//		}
//
//		Iterator& operator++()
//		{
//			for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
//			{
//				++m_Iterators[i];
//			}
//			return *this;
//		}
//
//		Iterator& operator++(int)
//		{
//			Iterator& tmp = *this;
//			for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
//			{
//				++m_Iterators[i];
//			}
//			return tmp;
//		}
//
//		Iterator& operator+=(int v)
//		{
//			if (v <= 0)
//				return *this;
//			for (int i = 0; i < timeline->Width() * timeline->Height() * timeline->Depth(); i++)
//			{
//				m_Iterators[i] += v;
//			}
//			return *this;
//		}
//
//		// can compare any CellTimeline::Iterator for comparisions
//		// since this class gurantees every CellTimeline will 
//		// have the same number of cells
//
//		friend bool operator==(const Iterator& i1, const Iterator& i2)
//		{
//			return i1.m_Iterators[0] == i2.m_Iterators[0];
//		}
//
//		friend bool operator!=(const Iterator& i1, const Iterator& i2)
//		{
//			return i1.m_Iterators[0] != i2.m_Iterators[0];
//		}
//
//		friend bool operator<(const Iterator& i1, const Iterator& i2)
//		{
//			return i1.m_Iterators[0] < i2.m_Iterators[0];
//		}
//
//		friend bool operator<=(const Iterator& i1, const Iterator& i2)
//		{
//			return i1.m_Iterators[0] <= i2.m_Iterators[0];
//		}
//
//	private:
//		std::vector<typename CellTimelineType::Iterator> m_Iterators;
//		PackedTissueTimelineImpl* timeline = nullptr;
//	};
//
//public:
//	Iterator begin() const
//	{
//		return Iterator{ this, 0 };
//	}
//
//	Iterator begin(size_t index) const
//	{
//		assert(index <= m_Size);
//		return Iterator{ this, index };
//	}
//
//	Iterator end() const
//	{
//		return Iterator{ this, m_Size };
//	}
//
//	Iterator end(size_t index) const
//	{
//		assert(index <= m_Size);
//		return Iterator{ this, m_Size };
//	}
//
//public:
//	//adds the given tissue to the timeline
//	void PushBack(const TissueType& tissue, TimestepType timestep)
//	{
//		assert(tissue.Width() == Width() && tissue.Height() == Height() && tissue.Depth() == Depth() &&
//		"Tissue shape and timeline shape dont match");
//		for (int x = 0; x < tissue.Width(); x++)
//		{
//			for (int y = 0; y < tissue.Height(); y++)
//			{
//				for (int z = 0; z < tissue.Depth(); z++)
//				{
//					bool last = (x + 1) * (y + 1) * (z + 1) == Width() * Height() * Depth();
//					m_Timelines[Index(x, y, z)].PushBack(tissue.GetCellAt(x, y, z), timestep, last);
//				}
//			}
//		}
//		m_Size++;
//	}
//
//	//gets the given version of the tissue from the timeline
//	Version GetVersion(size_t index) const
//	{
//		Version version{ Width(), Height(), Depth() };
//		for (int x = 0; x < Width(); x++)
//		{
//			for (int y = 0; y < Height(); y++)
//			{
//				for (int z = 0; z < Depth(); z++)
//				{
//					typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].GetVersion(index);
//					version.tissue.SetCellAt(x, y, z, v.cell);
//					version.timestep = v.timestep;
//				}
//			}
//		}
//		return version;
//	}
//
//	//get the changes in this particular version
//	int VersionChanges(size_t x, size_t y, size_t z, size_t index) const
//	{
//		return m_Timelines[Index(x, y, z)].GetVersionChanges(index);
//	}
//
//	//get the last tissue from the timeline
//	Version LastVersion() const
//	{
//		Version version{ Width(), Height(), Depth() };
//		for (int x = 0; x < Width(); x++)
//		{
//			for (int y = 0; y < Height(); y++)
//			{
//				for (int z = 0; z < Depth(); z++)
//				{
//					typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].GetLastVersion();
//					version.tissue.SetCellAt(x, y, z, v.cell);
//					version.timestep = v.timestep;
//				}
//			}
//		}
//		return version;
//	}
//
//	//get the first tissue from the timeline
//	Version FirstVersion() const
//	{
//		Version version{ Width(), Height(), Depth() };
//		for (int x = 0; x < Width(); x++)
//		{
//			for (int y = 0; y < Height(); y++)
//			{
//				for (int z = 0; z < Depth(); z++)
//				{
//					typename CellTimelineType::Version v = m_Timelines[Index(x, y, z)].GetFirstVersion();
//					version.tissue.SetCellAt(x, y, z, v.cell);
//					version.timestep = v.timestep;
//				}
//			}
//		}
//		return version;
//	}
//
//	//number of tissues in the timeline
//	size_t Size() const
//	{
//		return m_Size;
//	}
//
//	bool Empty() const
//	{
//		return !m_Size;
//	}
//
//	size_t SizeInBytes() const
//	{
//		size_t size = 0;
//		for (auto& t : m_Timelines)
//			size += t.SizeInBytes();
//		return size;
//	}
//
//	//we give out cell timeline by const reference because we dont wanna let anyone
//	//add to the timeline since we have to enfore the size rule of the tissue (if
//	//a cell is added to one timeline, it has to be added to all the other timelines
//	//to maintain the shape of the tissue)
//	const CellTimelineType& GetCellTimeline(size_t x, size_t y, size_t z) const
//	{
//		return m_Timelines[Index(x, y, z)];
//	}
//
//public:
//	size_t Width() const { return m_Width; }
//	size_t Height() const { return m_Height; }
//	size_t Depth() const { return m_Depth; }
//
//private:
//	size_t Index(size_t x, size_t y, size_t z) const
//	{
//		assert((x <= m_Width - 1) && (y <= m_Height - 1) && (z <= m_Depth - 1) &&
//			"Index out of range");
//		return z * (m_Width * m_Height) + (y * m_Width + x);
//	}
//
//private:
//	size_t m_Width;
//	size_t m_Height;
//	size_t m_Depth;
//	size_t m_Size = 0;
//
//private:
//	std::vector<CellTimelineType> m_Timelines;
//};

#pragma endregion OLD_PACKED_TISSUE_IMPLEMENTATION

//////////////////////////////////////////////
//////////////////////////////////////////////
//////// Main PackedTissueTimeline class /////
//////////////////////////////////////////////
//////////////////////////////////////////////
//
//template<class ValueType, class TimestepType, size_t NumProperties>
//class PackedTissueTimeline :
//	public PackedTissueTimelineImpl<ValueType, TimestepType,
//	typename IndexDescTypeHelper<NumProperties>::Type, NumProperties>
//{
//public:
//	using ImplType = PackedTissueTimelineImpl<ValueType, TimestepType,
//		typename IndexDescTypeHelper<NumProperties>::Type, NumProperties>;
//
//	PackedTissueTimeline(size_t x, size_t y, size_t z, size_t reserve) :
//		ImplType{ x, y, z, reserve }
//	{
//	}
//
//	PackedTissueTimeline(size_t x, size_t y, size_t z) :
//		ImplType{ x, y, z }
//	{
//	}
//
//	PackedTissueTimeline(size_t x, size_t y, size_t z, 
//		typename const ImplType::TissueType& startingTissue, TimestepType timestep) :
//		ImplType{ x, y, z, startingTissue, timestep }
//	{
//	}
//};
//
////for convenience 
////values = float, timestep = float
//template<size_t NumProperties>
//using PackedTissueTimelineFF = PackedTissueTimeline<float, float, NumProperties>;
//
////values = double, timestep = float
//template<size_t NumProperties>
//using PackedTissueTimelineDF = PackedTissueTimeline<double, float, NumProperties>;
//
////values = double, timestep = double
//template<size_t NumProperties>
//using PackedTissueTimelineDD = PackedTissueTimeline<double, double, NumProperties>;
//
////values = float, timestep = double
//template<size_t NumProperties>
//using PackedTissueTimelineFD = PackedTissueTimeline<float, double, NumProperties>;

//dont be like Windows.h
#undef LIKELY
#undef UNLIKELY
