#include "Cycpch.h"
#include "LayerStack.h"
#include "Window.h"

#include "Log.h"

#include "Assert.h"

namespace cyc {

	LayerStack::LayerStack(Window* window) :
		window(window)
	{
	}

	void LayerStack::_OnInit()
	{
		for (auto& it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			(*it)->OnInit();
		}
	}

	void LayerStack::_OnEvent(Event& e)
	{
		for (auto& it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (!e.propagate)
			{
				return;
			}
		}
	}

	void LayerStack::_OnUpdate()
	{
		for (auto& it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			(*it)->OnUpdate();
		}
	}

	void LayerStack::_OnDestroy()
	{
		for (auto& it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			(*it)->OnDestroy();
		}
	}

	void LayerStack::_OnImGuiRender()
	{
		for (auto& it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			(*it)->OnImGuiRender();
		}
	}

	void LayerStack::PushLayer(Layer* layer, int index, bool overlay)
	{
		layer->SetWindow(window);
		if (!overlay)
		{
			CYC_ASSERT(index <= m_NNonOverlays,
				"[LayerStack::PushLayer] [Layer = \"" + layer->GetName() + "\"] Index out of range."
			    " Valid Range = 0 to {0}", m_NNonOverlays);

			m_Layers.emplace(m_Layers.begin() + index, layer);
			m_NNonOverlays++;
		}
		else
		{
			CYC_ASSERT(index <= m_NOverlays,
				"[LayerStack::PushLayer] [Layer = \"" + layer->GetName() + "\"] Index out of range."
				" Valid Range = 0 to {0}", m_NOverlays);

			m_Layers.emplace(m_Layers.begin() + m_NNonOverlays + index, layer);
			m_NOverlays++;
		}
	}

	void LayerStack::PushFrontLayer(Layer* layer, bool overlay)
	{
		layer->SetWindow(window);
		if (!overlay)
		{
			m_Layers.emplace(m_Layers.begin(), layer);
			m_NNonOverlays++;
		}
		else
		{
			m_Layers.emplace(m_Layers.begin() + m_NNonOverlays, layer);
			m_NOverlays++;
		}
	}

	void LayerStack::PushBackLayer(Layer* layer, bool overlay)
	{
		layer->SetWindow(window);
		if (!overlay)
		{
			m_Layers.emplace(m_Layers.end() - m_NOverlays, layer);
			m_NNonOverlays++;
		}
		else
		{
			m_Layers.emplace_back(layer);
			m_NOverlays++;
		}
	}
}