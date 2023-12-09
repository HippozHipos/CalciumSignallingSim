#include "Reactions.h"

namespace csim {

	namespace
	{
		//??
		float Reaction0(const Cell& cell) 
		{	
			return cell.sigma1 * cell.y;
		}

		//??
		float Reaction1(const Cell& cell)
		{
			return cell.sigma0;
		}

		//Ca2+ flux from the endoplasmic reticulum to the cytosol via IP3 stimulation
		//what is IP3 stimulation?
		float Reaction2(const Cell& cell)
		{
			double kC1_n = std::pow(cell.kC1, cell.n);
			double kC2_n = std::pow(cell.kC2, cell.n);
			double Ca_n  = std::pow(cell.ca, cell.n);
			double Ia_m  = std::pow(cell.ia, cell.m);
			double kI_m  = std::pow(cell.ki, cell.m);

			double numerator1 = 4 * cell.sigmaEM3 * (kC1_n * Ca_n);
			double denominator1 = (Ca_n + kC1_n) * (Ca_n + kC2_n);

			double numerator2 = Ia_m;
			double denominator2 = kI_m + Ia_m;

			double right = numerator1 / denominator1;
			double left = (numerator2 / denominator2) * (cell.ea - cell.ca);

			return right * left;
		}

		float Reaction3(const Cell& cell)
		{
			float ca_2 = std::powf(cell.ca, 2);
			return cell.bigSigmaM2 * (ca_2 / (ca_2 + std::powf(cell.k2, 2)));
		}

		float Reaction4(const Cell& cell)
		{
			return cell.kf * cell.ea;
		}

		float Reaction5(const Cell& cell)
		{
			return cell.kf * cell.ca;
		}

		float Reaction6(const Cell& cell)
		{
			return cell.ko * cell.ca;
		}

		float Reaction7(const Cell& cell)
		{
			float ca_2 = std::powf(cell.ca, 2);
			return cell.bigSigmaP * (ca_2 / (ca_2 + std::powf(cell.kp, 2)));
		}

		float Reaction8(const Cell& cell)
		{
			return cell.kd * cell.ia;
		}

		float Reaction9(const Cell& cell)
		{
			return 0;
		}

		float DummyReaction(const Cell& cell) { return 0; }
	}

	Reactions::Reactions()
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
		propenstities[9] = DummyReaction;

		//10 - 19
		propenstities[10] = DummyReaction;
		propenstities[11] = DummyReaction;
		propenstities[12] = DummyReaction;
		propenstities[13] = DummyReaction;
		propenstities[14] = DummyReaction;
		propenstities[15] = DummyReaction;
		propenstities[16] = DummyReaction;
		propenstities[17] = DummyReaction;
		propenstities[18] = DummyReaction;
		propenstities[19] = DummyReaction;

		//20 - 27
		propenstities[20] = DummyReaction;
		propenstities[21] = DummyReaction;
		propenstities[22] = DummyReaction;
		propenstities[23] = DummyReaction;
		propenstities[24] = DummyReaction;
		propenstities[25] = DummyReaction;
		propenstities[26] = DummyReaction;
		propenstities[27] = DummyReaction;

		float v = 0.01; //amount by which calcium ions change in one timestep in gillispie
		float z = 0.0f; //zeroes for convenience to create table bellow 
		Reactions::stoichiometrySelf =
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
/*REACTION*/  /*13*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
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

		Reactions::stoichiometryNeighbours =
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
/*REACTION*/  /*13*/  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,  z,
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
}