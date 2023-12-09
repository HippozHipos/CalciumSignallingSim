#include "CalciumOverTimeGraph.h"
#include <iostream>

CalciumOverTimeGraph::CalciumOverTimeGraph(const csim::Simulator* const sim,
	csim::TissueTimelineFF<3, 3, 3, csim::Cell::NumProperties>& timeline) :
	Layer{ "GraphDrawer" }, sim{ sim }, timeline{ timeline }
{
	//fuck loops
	cells = {
		"Cell 1", "Cell 2", "Cell 3", "Cell 4", "Cell 5",
		"Cell 6", "Cell 7", "Cell 8", "Cell 9", "Cell 10",
		"Cell 11", "Cell 12", "Cell 13", "Cell 14", "Cell 15",
		"Cell 16", "Cell 17", "Cell 18", "Cell 19", "Cell 20",
		"Cell 21", "Cell 22", "Cell 23", "Cell 24", "Cell 25",
		"Cell 26", "Cell 27"
	};

	colors[0] = ImVec4(0.850f, 0.325f, 0.098f, 1.0f); // Red
	colors[1] = ImVec4(0.929f, 0.694f, 0.125f, 1.0f); // Gold
	colors[2] = ImVec4(0.494f, 0.184f, 0.556f, 1.0f); // Purple
	colors[3] = ImVec4(0.466f, 0.674f, 0.188f, 1.0f); // Green
	colors[4] = ImVec4(0.301f, 0.745f, 0.933f, 1.0f); // Light Blue
	colors[5] = ImVec4(0.635f, 0.078f, 0.184f, 1.0f); // Dark Red
	colors[6] = ImVec4(0.082f, 0.396f, 0.717f, 1.0f); // Dark Blue
	colors[7] = ImVec4(1.000f, 0.498f, 0.000f, 1.0f); // Orange
	colors[8] = ImVec4(0.173f, 0.627f, 0.173f, 1.0f); // Dark Green
	colors[9] = ImVec4(0.839f, 0.153f, 0.157f, 1.0f); // Dark Red
	colors[10] = ImVec4(0.580f, 0.404f, 0.725f, 1.0f); // Lavender
	colors[11] = ImVec4(0.498f, 0.498f, 0.498f, 1.0f); // Gray
	colors[12] = ImVec4(0.737f, 0.741f, 0.133f, 1.0f); // Olive
	colors[13] = ImVec4(0.255f, 0.667f, 0.608f, 1.0f); // Teal
	colors[14] = ImVec4(1.000f, 0.925f, 0.302f, 1.0f); // Yellow
	colors[15] = ImVec4(0.820f, 0.410f, 0.117f, 1.0f); // Brown
	colors[16] = ImVec4(0.572f, 0.698f, 0.847f, 1.0f); // Sky Blue
	colors[17] = ImVec4(0.584f, 0.388f, 0.188f, 1.0f); // Sienna
	colors[18] = ImVec4(0.992f, 0.753f, 0.525f, 1.0f); // Peach
	colors[19] = ImVec4(0.686f, 0.482f, 0.925f, 1.0f); // Purple
	colors[20] = ImVec4(0.965f, 0.620f, 0.862f, 1.0f); // Pink
	colors[21] = ImVec4(0.678f, 0.847f, 0.901f, 1.0f); // Light Blue
	colors[22] = ImVec4(0.918f, 0.655f, 0.278f, 1.0f); // Light Orange
	colors[23] = ImVec4(0.235f, 0.701f, 0.443f, 1.0f); // Dark Green
	colors[24] = ImVec4(0.494f, 0.760f, 0.972f, 1.0f); // Light Blue
	colors[25] = ImVec4(0.875f, 0.365f, 0.294f, 1.0f); // Salmon
	colors[26] = ImVec4(0.635f, 0.078f, 0.184f, 1.0f); // Dark Red
	colors[27] = ImVec4(0.976f, 0.450f, 0.023f, 1.0f); // Dark Orange
	colors[28] = ImVec4(0.557f, 0.824f, 0.408f, 1.0f); // Lime Green
	colors[29] = ImVec4(0.388f, 0.282f, 0.024f, 1.0f); // Brown


}

void CalciumOverTimeGraph::OnImGuiRender()
{
	PlotGraph();
	DrawPlotCheckbox();
	DrawCellSelect();
}

void CalciumOverTimeGraph::PlotGraph()
{
	ImGui::Begin("CacliumOverTimeGraph", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	if (ImPlot::BeginPlot("Calcium Concentration Over Time",
		"Time Step", "Calcium Concentration", ImVec2(-1, -1), ImPlotAxisFlags_NoInitialFit, ImPlotAxisFlags_NoInitialFit))
	{
		for (uint8_t i = 0; i < csim::Cell::NumProperties; i++)
		{
			ImPlot::SetNextLineStyle(colors[i]);
			if (m_Plots[i])
			{
				Points pts = GetPoints(i);
				ImPlot::PlotLine(csim::Cell::GetNameFromIndex(i),
					pts.xs.data(),
					pts.ys.data(),
					pts.ys.size()
				);
			}
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void CalciumOverTimeGraph::DrawPlotCheckbox()
{
	ImGui::Begin("Plot", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	for (int i = 0; i < csim::Cell::NumProperties; i++)
	{
		ImGui::Checkbox(csim::Cell::GetNameFromIndex(i), &m_Plots[i]);
	}
	ImGui::End();
}

void CalciumOverTimeGraph::DrawCellSelect()
{
	ImGui::Begin("Sample Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	if (ImGui::BeginCombo("Select Cell", currentCell, ImGuiComboFlags_NoArrowButton))
	{
		for (int i = 0; i < timeline.Width() * timeline.Height() * timeline.Depth(); i++)
		{
			bool is_selected = (currentCell == cells[i]);
			if (ImGui::Selectable(cells[i], is_selected))
			{
				selectedCell = i;
				currentCell = cells[i];
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Timeline size: %d", timeline.Size());
	ImGui::SliderInt("Sample Every N Cells", &sampleEvery, 1, 500);
	ImGui::End();
}

CalciumOverTimeGraph::Points CalciumOverTimeGraph::GetPoints(uint8_t properteyy)
{
	Points pts;

	auto end = timeline.end();
	for (auto it = timeline.begin(); it < end; it += sampleEvery)
	{
		auto v = it.GetCellVersionProperty(selectedCell, properteyy);
		pts.ys.push_back(v.value);
		pts.xs.push_back(v.timestep);
	}
	return pts;
}