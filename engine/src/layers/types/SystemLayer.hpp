#ifndef SYSTEM_LAYER_HPP
#define SYSTEM_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"

namespace Engine {

// Layer for handling system level events (eg window callbacks)
class SystemLayer : public Layer {
public:
	SystemLayer(std::function<void()> stopAppCallback, World& world);

	void onAttach() override;

private:
	std::function<void()> m_stopAppCallback;
};
} // Engine

#endif // !SYSTEM_LAYER_HPP
