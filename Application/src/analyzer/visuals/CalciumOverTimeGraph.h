#pragma once

#include "../Simulator.h"
#include "../Timeline.h"
#include "Cyclopes.h"

class CalciumOverTimeGraph : public cyc::Layer
{
private:
	struct Points
	{
		std::vector<float> xs;
		std::vector<float> ys;
	};

public:
	CalciumOverTimeGraph(const csim::Simulator* const sim, csim::TissueTimelineFF<3, 3, 3, 
		csim::Cell::NumProperties>& timeline);
	void OnImGuiRender() override;

private:
	void PlotGraph();
	void DrawPlotCheckbox();
	void DrawCellSelect();

	Points GetPoints(uint8_t properteyy);

	bool m_Plots[csim::Cell::NumProperties] = { 0 };
	const csim::Simulator* const sim = nullptr;
	csim::TissueTimelineFF<3, 3, 3, csim::Cell::NumProperties>& timeline;

	std::vector<const char*> cells;
	const char* currentCell = "Cell 1";
	int selectedCell = 0;

	int sampleEvery = 10;

	ImVec4 colors[30];
};