#pragma once
#include <vector>
#include "Tissue.h"
#include "Gillispie.h"
#include "Timer.h"

namespace csim {

	struct SimConfig
	{

	};

	struct Simulator
	{
		Simulator();

		Tissue<3, 3, 3> tissue{ };
		Gillispie gillispie;
		Reactions reactions;
		void RunGilliepie();
		void InitGilliepie();
		Timer timer;
		float runtime = 0.0f;
	};
}