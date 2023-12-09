#include "Simulator.h"
#include <iostream>

namespace csim {

	Simulator::Simulator()
	{
		tissue.ConstructFromSample(Cell{});
	}

	void Simulator::InitGilliepie()
	{
		timer.Reset();
		gillispie.Reset();
	}

	void Simulator::RunGilliepie()
	{
		gillispie.Simulate(tissue, reactions, 1, 0.00025);
		if (!gillispie.completed)
			runtime = timer.Time();
	}

}