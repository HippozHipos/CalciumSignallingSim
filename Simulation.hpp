#pragma once

#include "Tissue.hpp"
#include "GeneratedPackedTissueInfo.hpp"

#include <vector>
#include "Tissue.hpp"
#include "Propensities.hpp"
#include <random>

#include <numeric>

template<class T>
class Simulation {
public:
	//In this particular simulation, we are using 26 neighbours (all the nebhouring cells in a 3d cubie). 
	//But it is flexible since tissue provides ways to access neightbours in different ways
	using TissueType = PackedTissue<T>;
	using NeighboursType = typename TissueType::Neighbours3D;
	static constexpr int NNeighbours = NeighboursType::Count;

public:
	void SimulateCell(StemCell<T>& cell, NeighboursType& neighbours,
		const Propensitites<T>& props, float duration, float controlFactor, float& tw)
	{
		std::vector<float> rates;
		rates.reserve(Propensitites<T>::NumReactions);

		bool noTransitions = true;
		for (auto& prop : props.propenstities)
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
		int index = RandomTransition(rates, totalWeights) * cell.GetNumProperties();

		for (int i = index; i < index + cell.GetNumProperties(); i++)
		{
			//current cell reaction execution
			float next = props.stoichiometrySelf[i] + cell[i - index];
			cell[i - index] = next < 0 ? next * -1 : next;

			//neigbouring cells reaction execution
			for (int j = 0; j < NNeighbours; j++)
			{
				if (neighbours[j] != nullptr)
				{
					StemCell<T>& ncell = *neighbours[j];
					if (ncell.GetNumProperties() > (i - index))
					{
						float next = props.stoichiometryNeighbours[i] + ncell[i - index];
						ncell[i - index] = next < 0 ? next * -1 : next;
					}
				}
			}
		}
	}
	
	bool Simulate(TissueType& tissue, const Propensitites<T>& props, float duration, float controlFactor)
	{
		// think about this
		float scaledDuration = duration; // *tissue.Width()* tissue.Height()* tissue.Depth();

		completed = timeStep > scaledDuration;

		if (!completed)
		{
			float totalWeights = 0;

			//"non-blocking" for loop
			if (tissueX < tissue.Width())
			{
				if (tissueY < tissue.Height())
				{
					if (tissueZ < tissue.Depth())
					{
						auto cell = tissue.GetCell((size_t)tissueX, (size_t)tissueY, (size_t)tissueZ);
						NeighboursType neighbours = tissue.GetNeighbours3D(tissueX, tissueY, tissueZ);
						SimulateCell(*cell, neighbours, props, duration, controlFactor, totalWeights);
						tissueZ++;

						iterationComplete = false;
						if (tissueX == generated::PackedTissueShape[0] - 1 
							&& tissueY == generated::PackedTissueShape[1] - 1 
							&& tissueZ == generated::PackedTissueShape[2])
						{
							tissueX = tissueY = tissueZ = 0;
							iterations++;
							//totalWeights big = dt small. prevent dt too small
							float control = totalWeights * controlFactor;
							timeStep += (-1.0f / totalWeights) * std::logf(Random()) * control;
							iterationComplete = true;
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

		return !completed;

	}

private:
	//weights.size() is the number of reactions
	int RandomTransition(const std::vector<float>& weights, int totalWeights)
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

	int RandInt(int l, int h)
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(l, h);
		return dist(m_Rng);
	}

	float Random()
	{
		std::uniform_real_distribution<> dist(0, 1);
		return dist(m_Rng);
	}

public:
	float progress = 0;
	bool completed = false;
	int iterations = 0;
	float timeStep = 0;
	bool iterationComplete = false;

private:
	std::random_device m_RandomDevice;
	std::mt19937 m_Rng{ m_RandomDevice() };

	int tissueX = 0;
	int tissueY = 0;
	int tissueZ = 0;
};