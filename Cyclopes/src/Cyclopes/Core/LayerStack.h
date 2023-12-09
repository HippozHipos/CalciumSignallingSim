#pragma once

#include <vector>
#include <memory>

#include "Layer.h"
#include "Cyclopes/Event/Event.h"

namespace cyc {

	class LayerStack
	{
	public:
		LayerStack(Window* window);

	public:
		//client  side
		void PushLayer(Layer* layer, int index, bool overlay = false);
		void PushFrontLayer(Layer* layer, bool overlay = false);
		void PushBackLayer(Layer* layer, bool overlay = false);

	public:
		//you can look but cannot touch
		void _OnInit();
		void _OnEvent(Event& e);
		void _OnUpdate();
		void _OnDestroy();
		void _OnImGuiRender();

	private:
		CycVector<Layer*> m_Layers;
		Window* window = nullptr;
		int m_NOverlays = 0; //number of overlay layers
		int m_NNonOverlays = 0; //number of non-overlay layers
	};

}
