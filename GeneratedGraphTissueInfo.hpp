#pragma once

#include "TissueCellTypeList.hpp"
#include "Cell.hpp"

template<class T>
using GraphTissueCellTypes =
TissueCellTypeList
<
	Astrocyte<T>,
	Astrocyte<T>,
	SmoothMuscle<T>,
	Astrocyte<T>
> ;

inline const std::initializer_list<std::initializer_list<size_t>> GraphTissueConnections =
{
	{ 1, 2, 3 },
	{ 0, 2, 3 },
	{ 0, 1, 3 },
	{ 0, 1, 2 }
};

inline const std::initializer_list<std::initializer_list<size_t>> GraphTissueGapJunctions =
{
	{ 2, 2, 3 },
	{ 2, 2, 3 },
	{ 3, 1, 3 },
	{ 2, 1, 2 }
};