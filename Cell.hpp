#pragma once

#include <stdint.h>
#include <assert.h>
#include <initializer_list>
#include <cmath>
#include "StaticHeapArray.hpp"

template<class T>
class StemCell;

//Convenience. 
#define CELL_DERIVED_COMMONS(name, T, N) \
	using ValueType = T; \
	static constexpr size_t NumProperties = N; \
	/*default constrcutor*/\
	name(T* buffer = nullptr):\
		StemCell<T>{ buffer, NumProperties }\
	{\
	}\
	/*initializer list constrcutor*/\
	name(const std::initializer_list<T>& properties, T* buffer = nullptr):\
		StemCell<T>{ buffer, NumProperties }\
	{\
		int i = 0;\
		for (T v : properties)\
		{\
			this->Set(i++, v);\
		}\
	}\
	/*fill constrcutor*/\
	name(T v, T* buffer = nullptr):\
		StemCell<T>{ buffer, NumProperties }\
	{\
		for (size_t i = 0; i < NumProperties; i++)\
		{\
			this->Set(i, v);\
		}\
	}\
	constexpr size_t GetNumProperties() const { return NumProperties; } 

//Base cell class
template<class ValueType>
class StemCell
{
public:
	StemCell(ValueType* buffer, size_t numProps) :
		m_Properties{ buffer, numProps}
	{
	}

public:
	virtual const char* GetNameFromIndex(size_t index) = 0;

public:
	ValueType& operator[](size_t index)
	{
		return m_Properties[index]; 
	}

	const ValueType& operator[](size_t index) const
	{
		return m_Properties[index];
	}

	ValueType At(size_t index) const
	{
		return m_Properties.At(index);
	}

	void SetAt(size_t index, ValueType v)
	{
		m_Properties[index] = v; 
	}


private:
	StaticHeapArray<ValueType> m_Properties;
};

template<class T>
class Astrocyte : public StemCell<T>
{
	static_assert(std::is_arithmetic_v<T>);
public:
	CELL_DERIVED_COMMONS(Astrocyte, T, 30)

public:
	const char* GetNameFromIndex(size_t index) override
	{
		return "";
	}
};

//this one is just for testing the system. 
template<class T>
class SmoothMuscle : public StemCell<T>
{
	static_assert(std::is_arithmetic_v<T>);
public:
	CELL_DERIVED_COMMONS(SmoothMuscle, T, 5)

public:
	const char* GetNameFromIndex(size_t index) override
	{
		return "";
	}
};

#undef CELL_DERIVED_COMMONS