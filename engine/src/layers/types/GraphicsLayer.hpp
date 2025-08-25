#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"

namespace TileBite {

class GraphicsLayer : public Layer {
public:
	GraphicsLayer();

	void onAttach() override;

	static std::string getName() { return "GraphicsLayer"; }
};

} // TileBite

#endif // !GRAPHICS_LAYER_HPP
