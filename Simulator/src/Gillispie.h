#include <vector>
#include "Tissue.h"
#include "Reactions.h"
#include <random>

namespace csim {

	class Gillispie
	{
	public:
		static constexpr int NNeighbours = 6;

	public:
		void Reset();
		//Simulates changes in a single cell (and its neighbours) over time
		void Simulate(Cell& cell, 
			Cell* neighbours[NNeighbours],
			const Reactions& reactions, float duration, float controlFactor, float& tw);
		//Simulates changes in a tissue over time
		void Simulate(Tissue<3, 3, 3>& tissue, const Reactions& reactions, float duration, float controlFactor);
		float progress = 0;
		bool completed = false;
		int iterations = 0;
		float timeStep = 0;
		bool IterationComplete = false;

	private:
		//note: weights.size() is the number of reactions
		int RandomTransition(const std::vector<float>& weights, int totalWeights);
		int RandInt(int l, int h);
		float Random();

	private:
		std::random_device m_RandomDevice;
		std::mt19937 m_Rng{ m_RandomDevice() };

		int tissueX = 0;
		int tissueY = 0;
		int tissueZ = 0;
	};
}