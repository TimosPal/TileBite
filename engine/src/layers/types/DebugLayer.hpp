#ifndef DEBUG_LAYER_HPP
#define DEBUG_LAYER_HPP

#include "core/pch.hpp"
#include "layers/Layer.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class DebugLayer : public Layer {
public:
	DebugLayer(std::shared_ptr<Renderer2D> renderer2D);

	void onAttach() override;

	static std::string getName() { return "DebugLayer"; }
private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !DEBUG_LAYER_HPP
