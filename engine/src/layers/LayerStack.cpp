#include "layers/LayerStack.hpp"

namespace Engine {

std::shared_ptr<Layer> LayerStack::getLayerByName(const std::string& name) const {
	for (const auto& layer : m_layers)
	{
		if (layer->getName() == name)
		{
			return layer;
		}
	}
	return nullptr;
}

void LayerStack::pushLayer(std::shared_ptr<Layer> layer)
{
	layer->onAttach();
	m_layers.emplace(m_layers.begin() + m_overlaysIndex, layer);
	++m_overlaysIndex;
}

void LayerStack::pushOverlay(std::shared_ptr<Layer> overlay)
{
	overlay->onAttach();
	m_layers.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer)
{
	auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_overlaysIndex,
		[layer](const std::shared_ptr<Layer>& ptr) { return ptr.get() == layer; });
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
		[overlay](const std::shared_ptr<Layer>& ptr) { return ptr.get() == overlay; });
	if (it != m_layers.end())
	{
		overlay->onDetach();
		m_layers.erase(it);
	}
}

void LayerStack::dispatchEventToLayers(Event& event)
{
	for (auto& layer : m_layers)
	{
		// Halt event propagation if an event is handled
		if (event.isHandled())
			break;
		layer->onEvent(event);
	}
}

void LayerStack::onUpdate(float deltaTime)
{
	for (auto& layer : m_layers)
	{
		layer->onUpdate(deltaTime);
	}
}

} // Engine