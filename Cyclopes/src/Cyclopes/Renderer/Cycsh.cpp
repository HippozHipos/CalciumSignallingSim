#include "Cycpch.h"
#include "Cycsh.h"

#include "Cyclopes/Core/Assert.h"

namespace cyc {

	Cycsh LoadCycsh(const char* path)
	{
		std::ifstream file(path);
		std::string err = std::string{ "Could not open " } + path;
		CYC_ASSERT_NOBREAK(file.is_open(), err.c_str());

		Cycsh shader;
		std::string* readingInto = nullptr;

		for (std::string line; std::getline(file, line);)
		{
			if (line == "#vertex shader")
			{
				readingInto = &shader.vertex; continue;
			}
			if (line == "#fragment shader")
			{
				readingInto = &shader.fragment; continue;
			}
			if (readingInto != nullptr)
				readingInto->append(line + '\n');
		}

		file.close();
		return shader;
	}

}