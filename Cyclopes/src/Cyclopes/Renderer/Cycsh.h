#pragma once

#include <fstream>

namespace cyc {

	struct Cycsh
	{
		std::string vertex;
		std::string fragment;
	};

	Cycsh LoadCycsh(const char* path);
}