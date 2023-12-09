#include "GillispieStats.h"

GillispieStats::GillispieStats(const csim::Simulator* const sim) :
	cyc::Layer{ "GillispieStats "}, sim{ sim }
{
}

void GillispieStats::OnImGuiRender()
{
	ImGui::Begin("Simulation Stats");
	ImGui::Text(sim->gillispie.completed ? "Simulation complete" : "Simulation running");
	ImGui::ProgressBar(sim->gillispie.progress);
	ImGui::Text("Gellispie Cell Iterations: %d\n"
		"Simulation Time (Minutes): % f\n"
		"Timestep Time (Seconds): % f\n", 
		sim->gillispie.iterations, 
		sim->runtime / 60.0f, 
		sim->gillispie.timeStep);
	ImGui::End();
}
