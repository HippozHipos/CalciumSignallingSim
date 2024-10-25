#pragma once
#include <vector>
#include "Cell.hpp"

//Used to define reactions with different precision i.e. float, double etc
#define DEFINE_REACTIONS(T) \
	T Reaction0(const StemCell<T>& cell) \
	{ \
		return cell[0] * cell[1]; \
	} \
	T Reaction1(const StemCell<T>& cell) \
	{ \
		return cell[2]; \
	} \
	T Reaction2(const StemCell<T>& cell) \
	{ \
		T kC1_n = std::pow(cell[14], cell[5]); \
		T kC2_n = std::pow(cell[15], cell[5]); \
		T Ca_n = std::pow(cell[4], cell[5]); \
		T Ia_m = std::pow(cell[17], cell[26]); \
		T kI_m = std::pow(cell[16], cell[26]); \
		T numerator1 = 4 * cell[7] * (kC1_n * Ca_n); \
		T denominator1 = (Ca_n + kC1_n) * (Ca_n + kC2_n); \
		T numerator2 = Ia_m; \
		T denominator2 = kI_m + Ia_m; \
		T right = numerator1 / denominator1; \
		T left = (numerator2 / denominator2) * (cell[9] - cell[4]); \
		return right * left; \
	} \
	T Reaction3(const StemCell<T>& cell) \
	{ \
		float ca_2 = std::powf(cell[4], 2); \
		return cell[3] * (ca_2 / (ca_2 + std::powf(cell[6], 2))); \
	} \
	T Reaction4(const StemCell<T>& cell) \
	{ \
		return cell[10] * cell[9]; \
	} \
	T Reaction5(const StemCell<T>& cell) \
	{ \
		return cell[10] * cell[4]; \
	} \
	T Reaction6(const StemCell<T>& cell) \
	{ \
		return cell[8] * cell[4]; \
	} \
	T Reaction7(const StemCell<T>& cell) \
	{ \
		float ca_2 = std::powf(cell[4], 2); \
		return cell[13] * (ca_2 / (ca_2 + std::powf(cell[11], 2))); \
	} \
	T Reaction8(const StemCell<T>& cell) \
	{ \
		return cell[12] * cell[17]; \
	} \
	T Reaction9(const StemCell<T>& cell) \
	{ \
		return (cell[6] - cell[7]) / (cell[8] * cell[9] * cell[10]); \
	} \
	T Reaction10(const StemCell<T>& cell) \
	{ \
		return cell[11] / (cell[12] + cell[13] - cell[14]); \
	} \
	T Reaction11(const StemCell<T>& cell) \
	{ \
		return cell[15] * cell[16] * (cell[17] - cell[18]) / cell[19]; \
	} \
	T Reaction12(const StemCell<T>& cell) \
	{ \
		return (cell[20] + cell[21]) / (cell[22] * cell[23] / cell[24]); \
	} \
	T Reaction13(const StemCell<T>& cell) \
	{ \
		return cell[25] * (cell[26] - cell[27] / cell[28]); \
	} \
	T Reaction14(const StemCell<T>& cell) \
	{ \
		return (cell[29] + cell[0]) / (cell[1] * cell[2] - cell[3]); \
	} \
	T Reaction15(const StemCell<T>& cell) \
	{ \
		return cell[4] * cell[5] / (cell[6] + cell[7] * cell[8]); \
	} \
	T Reaction16(const StemCell<T>& cell) \
	{ \
		return (cell[9] - cell[10]) * cell[11] / cell[12]; \
	} \
	T Reaction17(const StemCell<T>& cell) \
	{ \
		return cell[13] / (cell[14] * cell[15] - cell[16] + cell[17]); \
	} \
	T Reaction18(const StemCell<T>& cell) \
	{ \
		return cell[18] * (cell[19] + cell[20]) / (cell[21] - cell[22]); \
	} \
	T Reaction19(const StemCell<T>& cell) \
	{ \
		return (cell[23] - cell[24]) / (cell[25] * cell[26] + cell[27]); \
	} \
	T Reaction20(const StemCell<T>& cell) \
	{ \
		return cell[28] * cell[29] * (cell[0] / cell[1] - cell[2]); \
	} \
	T Reaction21(const StemCell<T>& cell) \
	{ \
		return (cell[3] + cell[4]) / (cell[5] * cell[6] * cell[7]); \
	} \
	T Reaction22(const StemCell<T>& cell) \
	{ \
		return cell[8] * (cell[9] - cell[10] / cell[11]); \
	} \
	T Reaction23(const StemCell<T>& cell) \
	{ \
		return (cell[12] - cell[13]) / (cell[14] + cell[15] * cell[16]); \
	} \
	T Reaction24(const StemCell<T>& cell) \
	{ \
		return cell[17] * cell[18] / (cell[19] - cell[20] + cell[21]); \
	} \
	T Reaction25(const StemCell<T>& cell) \
	{ \
		return (cell[22] + cell[23]) * cell[24] / (cell[25] - cell[26]); \
	} \
	T Reaction26(const StemCell<T>& cell) \
	{ \
		return cell[27] / (cell[28] * cell[29] + cell[0] - cell[1]); \
	} \
	T Reaction27(const StemCell<T>& cell) \
	{ \
		return (cell[2] - cell[3]) * cell[4] / (cell[5] + cell[6] / cell[7]); \
	}

namespace { DEFINE_REACTIONS(float) }

template<class T>
struct Propensitites
{
	static constexpr int NumReactions = 28;
	//note: propensities themselves are allowed the change the cells properties as well
	using prop_t = T(*)(const StemCell<T>& cell);

	Propensitites()
	{
		propenstities.resize(NumReactions);
		//0 - 9
		propenstities[0] = Reaction0;
		propenstities[1] = Reaction1;
		propenstities[2] = Reaction2;
		propenstities[3] = Reaction3;
		propenstities[4] = Reaction4;
		propenstities[5] = Reaction5;
		propenstities[6] = Reaction6;
		propenstities[7] = Reaction7;
		propenstities[8] = Reaction8;
		propenstities[9] = Reaction9;

		//10 - 19
		propenstities[10] = Reaction10;
		propenstities[11] = Reaction11;
		propenstities[12] = Reaction12;
		propenstities[13] = Reaction13;
		propenstities[14] = Reaction14;
		propenstities[15] = Reaction15;
		propenstities[16] = Reaction16;
		propenstities[17] = Reaction17;
		propenstities[18] = Reaction18;
		propenstities[19] = Reaction19;

		//20 - 27
		propenstities[20] = Reaction20;
		propenstities[21] = Reaction21;
		propenstities[22] = Reaction22;
		propenstities[23] = Reaction23;
		propenstities[24] = Reaction24;
		propenstities[25] = Reaction25;
		propenstities[26] = Reaction26;
		propenstities[27] = Reaction27;


		float v = 0.01; //amount by which calcium ions change in one timestep in gillispie
		float z = 0.0f; //zeroes for convenience to create table bellow 
		stoichiometrySelf =
		{                                                              //PROPERTY
				   // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  
			/* 0*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 1*/  z,  z,  z,  z,  v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//+Ca
			/* 2*/  z,  z,  z,  z,  v,  z,  z,  z,  z, -v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//+Ca, -Ea
			/* 3*/  z,  z,  z,  z, -v,  z,  z,  z,  z,  v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//-Ca, +Ea
			/* 4*/  z,  z,  z,  z,  v,  z,  z,  z,  v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//+Ca, -ko
			/* 5*/  z,  z,  z,  z, -v,  z,  z,  z,  z,  v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//-Ca, +Ea
			/* 6*/  z,  z,  z,  z, -v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//-Ca
			/* 7*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z, -v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//-Ia
			/* 8*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z, -v,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,//-Ia
			/* 9*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*10*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*11*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*12*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
/*REACTION*//*13*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*14*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*15*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*16*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*17*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*18*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*19*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*20*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*21*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*22*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*23*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*24*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*25*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*26*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*27*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
		};

		stoichiometryNeighbours =
		{                                                              //PROPERTY
				   // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  
			/* 0*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 1*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 2*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 3*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 4*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 5*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 6*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 7*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 8*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/* 9*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*10*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*11*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*12*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
/*REACTION*//*13*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*14*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*15*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*16*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*17*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*18*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*19*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*20*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*21*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*22*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*23*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*24*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*25*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*26*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
			/*27*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z
		};

		//^^^^  
		//Ca(4) - amount of calcium in cytosol
		//Ea(9) - amount of calcium in Endoplasmic Reticulum
		//ko(8) - amount of calcium leaving cytosol into extra cellular space 
		//Ia(17) - ?
    }

public:
	std::vector<prop_t> propenstities; //28 reactions
	std::vector<T> stoichiometrySelf; //how to execute reaction for current cell
	std::vector<T> stoichiometryNeighbours; //how to execute reactions for neighbouring cells
};

