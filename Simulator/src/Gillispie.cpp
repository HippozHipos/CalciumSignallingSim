#include "Gillispie.h"
#include <numeric>
#include <assert.h>

namespace csim {

	void Gillispie::Reset()
	{
		completed = false;
		iterations = 0;
		timeStep = 0;

		tissueX = 0;
		tissueY = 0;
		tissueZ = 0;
	}

	void Gillispie::Simulate(Cell& cell, 
		Cell* neighbours[NNeighbours], //assumes references are valid
		const Reactions& reactions, float duration, float controlFactor, float& tw)
	{
			std::vector<float> rates;
			rates.reserve(Reactions::NumReactions);

			bool noTransitions = true;
			for (auto& prop : reactions.propenstities)
			{
				float rate = prop(cell);
				noTransitions &= rate <= 0; //cant have 0 or negative calcium in the whole cell
				rates.push_back(rate);
			}
			if (noTransitions)
			{
				completed = true;
				progress = 100;
				return;
			}

			float totalWeights = std::accumulate(rates.begin(), rates.end(), 0);
			tw = totalWeights;
			int index = RandomTransition(rates, totalWeights) * Cell::NumProperties;

			for (int i = index; i < index + Cell::NumProperties; i++)
			{
				//current cell reaction execution
				float next = reactions.stoichiometrySelf[i] + cell.GetProperty(i - index);
				cell.SetProperty(i - index, next < 0 ? next * -1 : next);

				//neigbouring cells reaction execution
				for (int j = 0; j < NNeighbours; j++)
				{
					if (neighbours[j] != nullptr)
					{
						float next = reactions.stoichiometryNeighbours[i] + neighbours[j]->GetProperty(i - index);
						neighbours[j]->SetProperty(i - index, next < 0 ? next * -1 : next);
					}
				}
			}
			

	}

	//NOTE: WE NEED TO CONSIDER THE WHOLE TISSUE WHEN CALCULATING PROBABILITY AND NOT JUST A CELL.
	void Gillispie::Simulate(Tissue<3, 3, 3>& tissue, const Reactions& reactions, float duration, float controlFactor)
	{
		                                // think about this
		float scaledDuration = duration; // *tissue.Width()* tissue.Height()* tissue.Depth();
		completed = timeStep > scaledDuration;

		//non-blocking while loop
		if (!completed)
		{
			Cell *left, *right, *up, *down, *front, *back;
			auto updateNeighbours = [&tissue, &left, &right, &up, &down, &front, &back](int x, int y, int z)
			{
				left = right = up = down = front = back = nullptr;
				if (x > 0)
					left = &tissue.GetCell(x - 1, y, z);
				else if (x < tissue.Width() - 1)
					right = &tissue.GetCell(x + 1, y, z);

				if (y > 0)
					up = &tissue.GetCell(x, y - 1, z);
				else if (y < tissue.Height() - 1)
					down = &tissue.GetCell(x, y + 1, z);

				if (z > 0)
					front = &tissue.GetCell(x, y, z - 1);
				else if (z < tissue.Depth() - 1)
					back = &tissue.GetCell(x, y, z + 1);
			};

			float totalWeights = 0;

			//non-blocking for loop
			if (tissueX < tissue.Width())
			{
				if (tissueY < tissue.Height())
				{
					if (tissueZ < tissue.Depth())
					{
						Cell& cell = tissue.GetCell(tissueX, tissueY, tissueZ);
						updateNeighbours(tissueX, tissueY, tissueZ);
						Cell* neighbours[NNeighbours] = { left, right, up, down, front, back };
						Simulate(cell, neighbours, reactions, duration, controlFactor, totalWeights);
						tissueZ++;

						IterationComplete = false;
						if (tissueX == 2 && tissueY == 2 && tissueZ == 3)
						{
							tissueX = tissueY = tissueZ = 0;
							iterations++;
							//totalWeights big = dt small. prevent dt too small
							float control = totalWeights * controlFactor;
							timeStep += (-1.0f / totalWeights) * std::logf(Random()) * control;
							IterationComplete = true;
							progress = (timeStep / scaledDuration);
						}
					}
					else
					{
						tissueY++; tissueZ = 0;
					}
				}
				else
				{
					tissueX++; tissueY = 0; tissueZ = 0;
				}
			}
		}
	}

	//weights.size() is the number of reactions
	int Gillispie::RandomTransition(const std::vector<float>& weights, int totalWeights)
	{	
		int random = RandInt(0, totalWeights);
		for (int i = 0; i < weights.size(); i++)
		{
			if (random < weights[i])
			{
				return i;
			}
			random -= weights[i];
		}
		assert(false);
	}

	int Gillispie::RandInt(int l, int h)
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(l, h);
		return dist(m_Rng);
	}

	float Gillispie::Random()
	{
		std::uniform_real_distribution<> dist(0, 1);
		return dist(m_Rng);
	}
}