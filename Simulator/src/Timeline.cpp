//#include "Timeline.h"
//#include <inttypes.h>
//#include <fstream>
//#include <string>
//
//namespace csim {
//
//	/////////////////////////////////////////////////////////////////////////////
//	//							  CellTimeline                                 //
//
//	CellTimeline::CellTimeline(int unused)
//	{
//		ReallocBuffer(m_BufferCap);
//
//		if (m_DataBuffer == nullptr)
//			return;//TODO: error handeling
//	}
//
//	CellTimeline::CellTimeline() :
//		CellTimeline(0)
//	{
//		m_WriteEnd = m_DataBuffer;
//		m_LastEntryByte = m_DataBuffer;
//	}
//
//	CellTimeline::CellTimeline(const Cell& startingCell, float timestep, bool last) :
//		CellTimeline(0)
//	{
//		m_WriteEnd = m_DataBuffer;
//		m_LastEntryByte = m_DataBuffer;
//
//		//Add the starting cell entry to our buffer.
//		//always assume we are the last cell in the timeline when adding cell
//		//when next cell is added, it will set itself as last cell and unset this one
//		uint32_t id = 0xFFFFFFFF;
//		SetIDLastCellInTissue(id, last);
//		AddToBuffer(id);
//		AddToBuffer(timestep);
//		for (int i = 0; i < Cell::NumProperties; i++)
//			AddToBuffer(startingCell.GetProperty(i));
//		m_NumEntries++;
//
//		//init last changed properties pointers to the first entry 
//		//respective properties in the first entry
//		for (int i = 0; i < Cell::NumProperties; i++)
//		{
//			m_LastPropertyChange[i] = m_DataBuffer + sizeof(uint32_t) + sizeof(float) + (i * sizeof(float));
//		}
//	}
//
//	CellTimeline::CellTimeline(const char* path)
//	{
//		ReadFromFile(path);
//	}
//
//	CellTimeline::CellTimeline(CellTimeline&& other) noexcept
//	{
//		Move(std::move(other));
//	}
//
//	CellTimeline& CellTimeline::operator=(CellTimeline&& other) noexcept
//	{
//		Move(std::move(other));
//		return *this;
//	}
//
//	CellTimeline::~CellTimeline()
//	{
//		FreeBuffer();
//	}
//
//	CellTimeline::Iterator CellTimeline::begin()
//	{
//		return Iterator{ this, 0 };
//	}
//
//	CellTimeline::Iterator CellTimeline::begin(size_t index)
//	{
//		assert(index <= m_NumEntries); //IMP CHANGE: < to <= MAYBE NOT CORRECT
//		return Iterator{ this, index };
//	}
//
//	CellTimeline::Iterator CellTimeline::end()
//	{
//		return Iterator{ this, m_NumEntries };
//	}
//
//	void CellTimeline::Add(const Cell& cell, float timestep, bool last)
//	{
//		Entry newEntry = EntryFromCell(cell);
//
//		if (newEntry.values.empty())
//		{
//			newEntry.ID |= (1 << m_LastCellBit);
//			SetIDLastCellInTissue(newEntry.ID, last);
//			if (last)
//				UnSetIDPreviousLastCellInTissue();
//			AddToBuffer(newEntry.ID);
//			AddToBuffer(timestep);
//			UnSetIDPreviousLastCellInTimeline();
//			m_LastEntryByte = m_WriteEnd - sizeof(int) - sizeof(float);
//			m_NumEntries++;
//			return;
//		}
//
//		size_t toStartByte = m_WriteEnd - m_DataBuffer;
//
//		newEntry.ID |= (1 << m_LastCellBit);
//		//this is now the last cell in the timeline
//		SetIDLastCellInTissue(newEntry.ID, last);
//		if (last)
//			UnSetIDPreviousLastCellInTissue();
//		AddToBuffer(newEntry.ID);
//		AddToBuffer(timestep);
//
//		int idIndex = 0;
//		for (int i = 0; i < newEntry.values.size(); i++)
//		{
//			AddToBuffer(newEntry.values[i]);
//			while (!(newEntry.ID & (1 << idIndex)))
//				idIndex++;
//			m_LastPropertyChange[idIndex] = m_WriteEnd - sizeof(float);
//			idIndex++;
//		}
//		UnSetIDPreviousLastCellInTimeline();
//
//		m_LastEntryByte = m_WriteEnd - (sizeof(float) * newEntry.values.size()) - sizeof(float) - sizeof(uint32_t);
//		m_NumEntries++;
//	}
//
//	CellTimeline::Version CellTimeline::GetVersion(size_t index) const
//	{
//		Version version;
//		Entry entry = EntryFromIndex(index);
//		version.timestep = entry.timestep;
//
//		if (entry.values.empty())
//		{
//			version.cell = RetrieveNoChangesCell(index);
//			return version;
//		}
//
//		int idIndex = 0;
//		bool set[Cell::NumProperties] = { 0 };
//		for (int i = 0; i < entry.values.size(); i++)
//		{
//			while (!(entry.ID & (1 << idIndex)))
//				idIndex++;
//			version.cell.SetProperty(idIndex, entry.values[i]);
//			set[idIndex] = true;
//			idIndex++;
//		}
//		for (int i = 0; i < Cell::NumProperties; i++)
//		{
//			if (!set[i])
//			{
//				version.cell.SetProperty(i, JoinBytes<float>(m_LastPropertyChange[i]));
//			}
//		}
//		return version;
//	}
//
//	uint32_t CellTimeline::VersionChanges(size_t index) const
//	{
//		return JoinBytes<uint32_t>(EntryOffsetFromIndex(index));
//	}
//
//	uint32_t CellTimeline::LastChanges() const
//	{
//		return JoinBytes<uint32_t>(m_LastEntryByte);
//	}
//
//	CellTimeline::Entry CellTimeline::EntryFromCell(const Cell& cell) const
//	{
//		Entry e;
//		for (int i = 0; i < Cell::NumProperties; i++)
//		{
//			e.ID &= ~(1 << i);
//			if (cell.GetProperty(i) != JoinBytes<float>(m_LastPropertyChange[i]))
//			{
//				e.ID |= 1 << i;
//				e.values.push_back(cell.GetProperty(i));
//			}
//		}
//		return e;
//	}
//
//	CellTimeline::Version CellTimeline::LastVersion() const
//	{
//		Version version;
//		version.timestep = JoinBytes<float>(m_LastEntryByte + sizeof(uint32_t));
//		for (int i = 0; i < Cell::NumProperties; i++)
//			version.cell.SetProperty(i, JoinBytes<float>(m_LastPropertyChange[i]));
//		return version;
//	}
//
//	CellTimeline::Version CellTimeline::FirstVersion() const
//	{
//		Version version;
//		uint8_t* start = m_DataBuffer + sizeof(uint32_t);
//		version.timestep = JoinBytes<float>(start);
//		start += sizeof(float);
//		for (int i = 0; i < Cell::NumProperties; i++)
//			version.cell.SetProperty(i, JoinBytes<float>(start + i * sizeof(float)));
//		return version;
//	}
//
//	CellTimeline::Entry CellTimeline::EntryFromIndex(size_t index) const
//	{
//		uint8_t* e = EntryOffsetFromIndex(index);
//		if (e == nullptr)
//			return Entry{};
//		return MakeEntry(e);
//	}
//
//	uint8_t* CellTimeline::EntryOffsetFromIndex(size_t index) const
//	{
//		assert(index <= m_NumEntries && "Index out of range");
//
//		if (index == 0)
//			return m_DataBuffer;
//
//		uint32_t id = JoinBytes<uint32_t>(m_DataBuffer);
//		size_t current = 0;
//		uint8_t* offset = m_DataBuffer + sizeof(uint32_t) + sizeof(float);
//		while (current != index)
//		{
//			id &= ~((1 << m_LastTissueBit) | (1 << m_LastCellBit));
//			offset += std::popcount(id) * sizeof(float);
//			id = JoinBytes<uint32_t>(offset);
//			offset += sizeof(uint32_t) + sizeof(float);
//			current++;
//		}
//		//above loop steps past the ID and timestep of the entry we want, so go back
//		return offset - sizeof(uint32_t) - sizeof(float);
//	}
//
//	CellTimeline::Entry CellTimeline::MakeEntry(uint8_t* offset) const
//	{
//		Entry e;
//		e.ID = JoinBytes<uint32_t>(offset);
//		offset += sizeof(uint32_t);
//		e.timestep = JoinBytes<float>(offset);
//		offset += sizeof(float);
//		uint32_t id = e.ID;
//		id &= ~((1 << m_LastTissueBit) | (1 << m_LastCellBit));
//		int i = 0;
//		for (int i = 0; i < std::popcount(id); i++)
//			e.values.push_back(JoinBytes<float>(offset + i * sizeof(float)));
//
//		return e;
//	}
//
//	void CellTimeline::SetIDLastCellInTissue(uint32_t& id, bool last)
//	{
//		//set last bit if last (last cell in tissue)
//		if (last)
//			id |= (1 << m_LastTissueBit);
//		else
//			id &= ~(1 << m_LastTissueBit);
//	}
//
//	void CellTimeline::UnSetIDPreviousLastCellInTimeline()
//	{
//		//last entry is no longer the last cell
//		uint32_t lastID = JoinBytes<uint32_t>(m_LastEntryByte);
//		lastID &= ~(1 << m_LastCellBit);
//		memcpy(m_LastEntryByte, &lastID, sizeof(uint32_t));
//	}
//
//	void CellTimeline::UnSetIDPreviousLastCellInTissue()
//	{
//		uint32_t lastID = JoinBytes<uint32_t>(m_LastEntryByte);
//		lastID &= ~(1 << m_LastTissueBit);
//		memcpy(m_LastEntryByte, &lastID, sizeof(uint32_t));
//	}
//
//
//	Cell CellTimeline::RetrieveNoChangesCell(size_t index) const
//	{
//		assert(index != 0 && "Cannot have first cell without changes");
//		assert(index <= m_NumEntries - 1 && "Index out of range");
//
//		uint32_t id = JoinBytes<uint32_t>(m_DataBuffer);
//		size_t current = 0;
//		uint8_t* offset = m_DataBuffer + sizeof(uint32_t) + sizeof(float);
//
//		Cell cell;
//
//		while (current <= index - 1)
//		{
//			id &= ~((1 << m_LastTissueBit) | (1 << m_LastCellBit));
//
//			//logic to handle retrieval 
//			uint8_t* i = offset;
//			for (int idIndex = 0; i < offset + std::popcount(id) * sizeof(float); i += sizeof(float), idIndex++)
//			{
//				while (!(id & (1 << idIndex)))
//					idIndex++;
//				cell.SetProperty(idIndex, JoinBytes<float>(i));
//			}
//
//			//logic to go to next entry
//			offset = i;
//			id = JoinBytes<uint32_t>(offset);
//			offset += sizeof(uint32_t) + sizeof(float);
//			current++;
//		}
//		return cell;
//	}
//
//	void CellTimeline::Move(CellTimeline&& other) noexcept
//	{
//		if (this != &other)
//		{
//			FreeBuffer();
//			m_DataBuffer = other.m_DataBuffer;
//			other.m_DataBuffer = nullptr;
//
//			m_NumEntries = other.m_NumEntries;
//			m_BufferCap = other.m_BufferCap;
//			m_BufferSize = other.m_BufferSize;
//			m_LastEntryByte = other.m_LastEntryByte;
//			m_WriteEnd = other.m_WriteEnd;
//			for (int i = 0; i < Cell::NumProperties; i++)
//				m_LastPropertyChange[i] = other.m_LastPropertyChange[i];
//		}
//	}
//
//	void CellTimeline::FreeBuffer()
//	{
//#ifdef _WIN32
//		_aligned_free(m_DataBuffer);
//#else
//		std::free(m_DataBuffer);
//#endif
//	}
//
//	void CellTimeline::ReallocBuffer(size_t cap)
//	{
//		uint8_t* newBuf = (uint8_t*)_aligned_malloc(cap, m_DataBufferAlignment);
//		if (!newBuf)
//			return;//do error handle
//		memcpy(newBuf, m_DataBuffer, m_BufferSize);
//		FreeBuffer();
//		m_DataBuffer = newBuf;
//		m_BufferCap = cap;
//	}
//
//	void CellTimeline::ReAdjustPointers(uint8_t* old)
//	{
//		m_LastEntryByte = m_DataBuffer + (m_LastEntryByte - old);
//		m_WriteEnd = m_DataBuffer + (m_WriteEnd - old);
//
//		for (int i = 0; i < Cell::NumProperties; i++)
//		{
//			m_LastPropertyChange[i] = m_DataBuffer + (m_LastPropertyChange[i] - old);
//		}
//	}
//
//	size_t CellTimeline::Size() const
//	{
//		return m_NumEntries;
//	}
//
//	void CellTimeline::WriteToFile(const char* path) const
//	{
//		std::ofstream file{ std::string{ path } + ".ctl", std::ios::binary };
//		if (!file)
//			return; //TODO: REPORT ERROR
//		file.write((char*)m_DataBuffer, m_BufferSize);
//	}
//
//	void CellTimeline::ReadFromFile(const char* path)
//	{
//
//		std::ifstream file{ path + std::string{ ".ctl" }, std::ios::binary };
//		if (!file)
//			return;//TODO: REPORT ERROR
//
//		uint32_t id;
//		while (file.read(reinterpret_cast<char*>(&id), sizeof(id)))
//		{
//			AddToBuffer(id);
//			id &= ~((1 << m_LastTissueBit) | (1 << m_LastCellBit));
//			int idIndex = 0;
//			int i = 0;
//			for (int idIndex = 0; i < std::popcount(id); i++, idIndex++)
//			{
//				while (!(id & (1 << idIndex)))
//					idIndex++;
//				float value;
//				file.read(reinterpret_cast<char*>(&value), sizeof(float));
//				m_LastPropertyChange[idIndex] = m_WriteEnd;
//				AddToBuffer(value);
//			}
//			m_NumEntries++;
//		}
//	}
//}