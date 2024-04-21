#pragma once

#include "TissueCellTypeList.hpp"
#include "Cell.hpp"

template<class T>
using PackedTissueCellTypes = 
TissueCellTypeList
<
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	SmoothMuscle<T>,
	SmoothMuscle<T>,
	SmoothMuscle<T>,
	SmoothMuscle<T>,
	SmoothMuscle<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	SmoothMuscle<T>,
	Astrocyte<T>,
	Astrocyte<T>,
	SmoothMuscle<T>,
	Astrocyte<T>
>;

