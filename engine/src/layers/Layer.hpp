#ifndef LAYER_HPP
#define LAYER_HPP

#include "core/pch.hpp"
#include "events/Event.hpp"

namespace Engine {

class Layer {
public:
	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate() {}
	virtual void onEvent(std::unique_ptr<Event> event) {}
};

} // Engine

#endif // !LAYER_HPP
