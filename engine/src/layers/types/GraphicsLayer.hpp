#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class GraphicsLayer : public Layer {
public:
	GraphicsLayer(std::shared_ptr<Renderer2D> renderer2D);

	void onAttach() override;

	static std::string getName() { return "GraphicsLayer"; }
private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !GRAPHICS_LAYER_HPP
