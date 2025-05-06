#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"

namespace Engine {

class GraphicsLayer : public Layer {
public:
	GraphicsLayer(World& world);

	void onAttach() override;
};

} // Engine

#endif // !GRAPHICS_LAYER_HPP
