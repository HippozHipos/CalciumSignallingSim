#pragma once

#include <stdint.h>
#include <cmath>
#include "Probabilities.h"

namespace csim {

	class StemCell
	{
	public:
		constexpr virtual int NumProperties() = 0;
		virtual const char* GetNameFromIndex(size_t index) = 0;
	};

	class Astrocytes : public StemCell
	{
	public:
		constexpr virtual int NumProperties() override;
		const char* GetNameFromIndex(size_t index) override;

		float sigma1 = 100.0f;
		float y = 0.0f;
		float sigma0 = 0.05f;
		float bigSigmaM2 = 15.0f;
		float ca = 0.1f;  //calcium content in cytosol
		float n = 2.02f;
		float k2 = 0.1f;
		float sigmaEM3 = 4000.0f;
		float ko = 50.0f; //calcium leaving cytosol into extra cellular space    
		float ea = 1.5f; //calcium content in endoplasmic reticulum
		float kf = 50.0f;
		float kp = 0.3f;
		float kd = 8.0f;
		float bigSigmaP = 0.05f;
		float kC1 = 0.15f;
		float kC2 = 0.15f;
		float ki = 0.1f;
		float ia = 0.1f; //calcium content in reciever 
		float q = 4.0f;
		float w = 0.0f;
		float a = 0.0f;
		float kia = 0.5f;
		float d = 122500.0f; // diffusion coefficient (adapted) - media is the intracellular astrocyte
		float l = (3.1416f * std::pow((3.0f / 2.0f), 2.0f)); // NOTE: 3 IS CELL LENGTH, SHOULD NOT BE MAGIC NUMBER
		float k = 0.0006f;
		float ka = 2.5f;
		float m = 2.2f;
		float phh = Probabilities::phh[0]; //probability of gap junction being high-high
		float phl = Probabilities::phl[0]; //probability of gap junction being high-low
		float plh = Probabilities::plh[0]; //probability of gap junction being low-high

		float GetProperty(uint8_t prop) const;
		float SetProperty(uint8_t prop, float value);
	};

	struct Cell
	{
		static constexpr int NumProperties = 30;
		
		float sigma1 = 100.0f;
		float y = 0.0f; 
		float sigma0 = 0.05f;
		float bigSigmaM2 = 15.0f;
		float ca = 0.1f;  //calcium content in cytosol
		float n = 2.02f;
		float k2 = 0.1f;
		float sigmaEM3 = 4000.0f;
		float ko = 50.0f; //calcium leaving cytosol into extra cellular space    
		float ea = 1.5f; //calcium content in endoplasmic reticulum
		float kf = 50.0f;
		float kp = 0.3f;
		float kd = 8.0f;
		float bigSigmaP = 0.05f;
		float kC1 = 0.15f;
		float kC2 = 0.15f;
		float ki = 0.1f;
		float ia = 0.1f; //calcium content in reciever 
		float q = 4.0f;
		float w = 0.0f;
		float a = 0.0f;
		float kia = 0.5f;
		float d = 122500.0f; // diffusion coefficient (adapted) - media is the intracellular astrocyte
		float l = (3.1416f * std::pow((3.0f / 2.0f), 2.0f)); // NOTE: 3 IS CELL LENGTH, SHOULD NOT BE MAGIC NUMBER
		float k = 0.0006f;
		float ka = 2.5f;
		float m = 2.2f;
		float phh = Probabilities::phh[0]; //probability of gap junction being high-high
		float phl = Probabilities::phl[0]; //probability of gap junction being high-low
		float plh = Probabilities::plh[0]; //probability of gap junction being low-high
		
		
		//note: no error check
		float GetProperty(uint8_t prop) const;
		float SetProperty(uint8_t prop, float value);
		static const char* GetNameFromIndex(uint8_t index);
	};
}


//formula variables in the equations
//static constexpr uint8_t EQ_SIGMA_1 = 0;
//static constexpr uint8_t EQ_Y = 1;
//static constexpr uint8_t EQ_SIGMA_0 = 2;
//static constexpr uint8_t EQ_BIG_SIGMA_M2 = 3;
//static constexpr uint8_t EQ_CA = 4;
//static constexpr uint8_t EQ_N = 5;
//static constexpr uint8_t EQ_K2 = 6;
//static constexpr uint8_t EQ_SIGMA_EM3 = 7;
//static constexpr uint8_t EQ_KO = 8;
//static constexpr uint8_t EQ_EA = 9;
//static constexpr uint8_t EQ_KF = 10;
//static constexpr uint8_t EQ_KP = 11;
//static constexpr uint8_t EQ_KD = 12;
//static constexpr uint8_t EQ_BIG_SIGMA_P = 13;
//static constexpr uint8_t EQ_KC_1 = 14;
//static constexpr uint8_t EQ_KC_2 = 15;
//static constexpr uint8_t EQ_KI = 16;
//static constexpr uint8_t EQ_IA = 17;
//static constexpr uint8_t EQ_Q = 18;
//static constexpr uint8_t EQ_W = 19;
//static constexpr uint8_t EQ_A = 20;
//static constexpr uint8_t EQ_KIA = 21;
//static constexpr uint8_t EQ_D = 22;
//static constexpr uint8_t EQ_L = 23;
//static constexpr uint8_t EQ_K = 24;
//static constexpr uint8_t EQ_KA = 25;
//static constexpr uint8_t EQ_M = 26;
//static constexpr uint8_t EQ_PHH = 27;
//static constexpr uint8_t EQ_PHL = 28;
//static constexpr uint8_t EQ_PLH = 29;
