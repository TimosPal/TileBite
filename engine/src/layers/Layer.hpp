#ifndef LAYER_HPP
#define LAYER_HPP

#include <memory>

#include "events/Event.hpp"

namespace Engine {

class Layer {
public:
	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate() {}
	virtual void onEvent(std::unique_ptr<IEvent> event) {}
};

} // Engine

#endif // !LAYER_HPP
