#include "layers/LayerStack.hpp"

namespace Engine {

void LayerStack::pushLayer(std::unique_ptr<Layer> layer)
{
	layer->onAttach();
	m_layers.emplace(m_layers.begin() + m_overlaysIndex, std::move(layer));
	++m_overlaysIndex;
}

void LayerStack::pushOverlay(std::unique_ptr<Layer> overlay)
{
	overlay->onAttach();
	m_layers.emplace_back(std::move(overlay));
}

void LayerStack::popLayer(Layer* layer)
{
	auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_overlaysIndex,
		[layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });
	if (it != m_layers.begin() + m_overlaysIndex)
	{
		layer->onDetach();
		m_layers.erase(it);
		--m_overlaysIndex;
	}
}

void LayerStack::popOverlay(Layer* overlay)
{
	auto it = std::find_if(m_layers.begin() + m_overlaysIndex, m_layers.end(),
		[overlay](const std::unique_ptr<Layer>& ptr) { return ptr.get() == overlay; });
	if (it != m_layers.end())
	{
		overlay->onDetach();
		m_layers.erase(it);
	}
}

} // Engine