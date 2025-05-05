#ifndef RENDERER_2D_HPP
#define RENDERER_2D_HPP

#include "core/pch.hpp"

namespace Engine {

class Renderer2D {
public:
	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

    virtual void clearScreen() {}
    virtual void render() {}

	static std::unique_ptr<Renderer2D> createRenderer2D();
};

} // Engine

#endif // !RENDERER_2D_HPP