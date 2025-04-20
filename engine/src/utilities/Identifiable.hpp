#ifndef IDENTIFIABLE_HPP
#define IDENTIFIABLE_HPP

#include "utilities/IDGenerator.hpp"
#include "utilities/Concepts.hpp"

namespace Engine {

// Use this macro to setup ids in a given class
#define SETUP_ID(Base, Sub)                                        \
public:                                                            \
    const Engine::ID getInstanceID() override					   \
	{                                                              \
        return m_id;                                               \
    }                                                              \
    const Engine::ID getTypeID() override						   \
	{                                                              \
        return GET_TYPE_ID(Base, Sub);                             \
    }                                                              \
private:                                                           \
    const Engine::ID m_id = GET_INSTANCE_ID(Base, Sub);            \

// Use this interface if you want an item to be identified by an ID.
// Methods should be implemented by macro SETUP_ID
class Identifiable {
public:
	virtual const ID getInstanceID() = 0;
	virtual const ID getTypeID() = 0;
};

} // Engine

#endif // !IDENTIFIABLE_HPP
