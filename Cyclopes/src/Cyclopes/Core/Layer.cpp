#include "Cycpch.h"
#include "Cycdefs.h"

#include "Layer.h"

namespace cyc {

	Layer::Layer(const CycString& name):
		m_Name(name)
	{
	}

	void Layer::OnInit() {}
	void Layer::OnUpdate() {}
	void Layer::OnEvent(Event& e) { e.propagate = true; }
	void Layer::OnDestroy() {}
	void Layer::OnImGuiRender() {}

	const CycString& Layer::GetName() const
	{
		return m_Name;
	}

	void Layer::SetWindow(Window* win)
	{
		window = win;
	}
}
