#include "Timeline.hpp"
#include "Tissue.hpp"
#include <iostream>
#include <initializer_list>

int main()
{
	PackedTissueF tissue{ 3, 3, 3 };
	auto cell = tissue.GetCellAt(26);
	std::cout << cell->At(22);
}

