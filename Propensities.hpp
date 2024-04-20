#pragma once
#include <vector>
#include "Cell.hpp"

struct Propensitites
{
	static constexpr int NumReactions = 28;
	//note: propensities themselves are allowed the change the cells properties as well
	using prop_t = float(*)(const Cell&);
	Propensitites();
	std::vector<prop_t> propenstities; //28 reactions
	std::vector<float> stoichiometrySelf; //how to execute reaction for current cell
	std::vector<float> stoichiometryNeighbours; //how to execute reactions for neighbouring cells
};

