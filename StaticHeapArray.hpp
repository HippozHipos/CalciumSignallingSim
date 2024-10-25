#pragma once
#include <assert.h>
#include <vector>

//Exactly as the name says. Its a static heap array. 
//The sole reason for its existance is to be able to allocate memory 
//of the heap buffer from outside the class. std::vector doesnt let us do that. 
//It is probably bad design but there is a good reason for it. 
//Basically we need to allocate memory so that multiple StaticHeapArrays
//point to contigious memory. There seems to be ways to do this with std::vector
//too with custom allocators but this seems like the simplest option to me.
 
//NOTE: IF A VALID POINTER IS PASSED TO THE CONSTRUCTOR, IT IS EXPECTED THAT
//THE POINTER IS PLACEMENT-NEWD SO IT IS NOT DELETED BY THE CLASS. IF NULLPTR
//IS PASSED AS ARGUMENT, IT WILL NEW MEMORY AND DELETE IT. IF COPY CONSTRUCTOR
//IS CALLED, IT WILL ALSO NEW MEMORY AND DELETE IT. IN OTHER WORDS, IT DELETES
//MEMORY IF IT NEWS IT, OTHERWISE IT DOESNT. IT NEWS MEMORY WHEN NULL POINTER
//IS PASSED TO CONSTRUCTOR OR COPY CONSTRUCTOR IS CALLED.

//NOTE: COPY ASIGNMENT DOES NOT RE-ALLOCATE MEMORY.
//Since this class is strictly only used by StemCell, it doesnt make sense
//to reallocate memory when copy asigning since the size cannot change.
//In kinda makes sense since its a static array but still something to 
//keep in mind.

template<class Valuetype>
class StaticHeapArray
{
public:
	using ValueType = Valuetype;
	StaticHeapArray(ValueType* buffer, size_t size) :
		m_Size{ size }, m_Buffer{ buffer }
	{
		if (buffer == nullptr)
		{
			m_Buffer = new ValueType[size];
			m_OwnPtr = true;
		}
	}

	~StaticHeapArray() { if (m_OwnPtr) delete m_Buffer; }

	StaticHeapArray(const StaticHeapArray& other)
	{
		m_Buffer = new ValueType[other.m_Size];
		m_OwnPtr = true;
		m_Size = other.Size();
		for (size_t i = 0; i < other.m_Size; i++)
		{
			m_Buffer[i] = other[i];
		}
	}

	StaticHeapArray& operator=(const StaticHeapArray& other)
	{
		if (this != &other)
		{
			for (size_t i = 0; i < other.m_Size; i++)
			{
				m_Buffer[i] = other[i];
			}
		}
	}

	StaticHeapArray(StaticHeapArray&& other) noexcept
	{
		m_Buffer = other.m_Buffer;
		other.m_Buffer = nullptr;
		m_Size = other.m_Size;
		other.m_Size = 0;
	}

	StaticHeapArray& operator=(StaticHeapArray&& other) noexcept
	{
		if (this != &other)
		{
			m_Buffer = other.m_Buffer;
			other.m_Buffer = nullptr;
			m_Size = other.m_Size;
			other.m_Size = 0;
		}
		return *this;
	}

	const ValueType& operator[](size_t i) const { assert(i < m_Size); return m_Buffer[i]; }
	ValueType& operator[](size_t i) { assert(i < m_Size); return m_Buffer[i]; }
	ValueType At(size_t i) const { assert(i < m_Size); return m_Buffer[i];}
	void SetAt(size_t i, ValueType value) const { assert(i < m_Size); m_Buffer[i] = value; }
	size_t Size() const { return m_Size; }

private:
	bool m_OwnPtr = false;
	Valuetype* m_Buffer = nullptr;
	size_t m_Size = 0;
};