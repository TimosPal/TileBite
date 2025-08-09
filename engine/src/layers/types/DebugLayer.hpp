#ifndef DEBUG_LAYER_HPP
#define DEBUG_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class DebugLayer : public Layer {
public:
	DebugLayer();

	void onAttach() override;

	static std::string getName() { return "DebugLayer"; }
};

} // Engine

#endif // !DEBUG_LAYER_HPP
