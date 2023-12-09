#include "analyzer/visuals/CalciumOverTimeGraph.h"
#include "analyzer/visuals/GillispieStats.h"
#include "../Timeline.h"
#include <iostream>
#include <bitset>
#include "Cyclopes/Core/EntryPoint.h"

class MyApp : public cyc::Application
{
public:
	MyApp() :
		cyc::Application(1200, 800)
	{
		window->SetTitle("Calcium Signaling");
		cyc::LayerStack& layers = window->GetLayerStack(); 
		layers.PushBackLayer(&calciumOverTimeGraph);
		layers.PushBackLayer(&gillispieStats);
		sim.InitGilliepie();
	}

	void OnUpdate() override
	{
		cyc::Graphics::Render::Clear(0.4f, 0.4f, 0.4f, 0.4f);
		sim.RunGilliepie();
		if (sim.gillispie.IterationComplete)
			timeline.Add(sim.tissue, sim.gillispie.timeStep);
		
	}

private:
	csim::Simulator sim;
	csim::TissueTimelineFF<3, 3, 3, csim::Cell::NumProperties> timeline{ csim::Tissue<3, 3, 3>{}, 0.0f };
	CalciumOverTimeGraph calciumOverTimeGraph{ &sim, timeline };
	GillispieStats gillispieStats{ &sim };
};

cyc::Application* cyc::CreateApplication()
{
	return new MyApp{};
}



