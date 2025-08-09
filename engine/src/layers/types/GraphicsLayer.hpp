#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"

namespace Engine {

class GraphicsLayer : public Layer {
public:
	GraphicsLayer();

	void onAttach() override;

	static std::string getName() { return "GraphicsLayer"; }
};

} // Engine

#endif // !GRAPHICS_LAYER_HPP
