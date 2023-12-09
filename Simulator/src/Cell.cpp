#include "Cell.h"

namespace csim {

	float Astrocytes::GetProperty(uint8_t prop) const
	{
		return *(&sigma1 + prop); 
	}

	float Astrocytes::SetProperty(uint8_t prop, float value)
	{
		return  *(&sigma1 + prop) = value;
	}

	constexpr int Astrocytes::NumProperties()
	{
		return 30;
	}

	const char* Astrocytes::GetNameFromIndex(size_t index)
	{
		static const char* t[] 
		{
			"Sigma1",		            "Y",                     "Sigma0",
			"BigSigmaM2",		        "Ca",                    "N",
			"K2",		                "SigmaEM3",		         "Ko",
			"Ea",		                "Kf",		             "Kp",
			"Kd",		                "BigSigmaP",		     "kC1",
			"kC2",		                "kI1",		             "Ia",
			"Q",		                "W",		             "A",
			"kia",		                "D",		             "L",
			"K",		                "ka",		             "M",
			"PHH",		                "PHL",		             "PLH"
		};
		return t[index];
	}

	float Cell::GetProperty(uint8_t prop) const
	{
		return *(&sigma1 + prop);
	}

	float Cell::SetProperty(uint8_t prop, float value)
	{
		return  *(&sigma1 + prop) = value;
	}

	const char* Cell::GetNameFromIndex(uint8_t index)
	{
		static const char* t[]
		{
			"Sigma1",		            "Y",                     "Sigma0",
			"BigSigmaM2",		        "Ca",                    "N",
			"K2",		                "SigmaEM3",		         "Ko",
			"Ea",		                "Kf",		             "Kp",
			"Kd",		                "BigSigmaP",		     "kC1",
			"kC2",		                "kI1",		             "Ia",
			"Q",		                "W",		             "A",
			"kia",		                "D",		             "L",
			"K",		                "ka",		             "M",
			"PHH",		                "PHL",		             "PLH"
		};
		return t[index];
	}
}