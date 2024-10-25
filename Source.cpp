#include "Timeline.hpp"
#include "Tissue.hpp"
#include <iostream>
#include "GeneratedPackedTissueInfo.hpp"
#include "GeneratedGraphTissueInfo.hpp"
#include "Simulation.hpp"

using MathPrecision = float;

int main()
{
	Simulation<MathPrecision> sim;

	PackedTissue<MathPrecision> tissue{
		generated::PackedTissueShape[0],
		generated::PackedTissueShape[1],
		generated::PackedTissueShape[2] 
	};

	//In this particular example test, we will track the first cell, 
	//which is an astrocyte.We can use CellTimelineFF for it since 
	//both values and timesteps are in floats in this example.
	CellTimeline<float, float, Astrocyte<float>> firstCellTimeline;
	
	while (sim.Simulate(tissue, Propensitites<MathPrecision>{}, 10, 0.005f))
	{
		std::cout << "Simulation progress: " << sim.progress << "%\n";
		std::cout << "Iterations: " << sim.iterations << '\n';
		firstCellTimeline.PushBack(*tissue.GetCellAt(0), sim.timeStep, false);
		std::system("cls");
	}

	std::cout << "Simulation finished\n";
}

