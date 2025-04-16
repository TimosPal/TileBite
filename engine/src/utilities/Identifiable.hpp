#ifndef IDENTIFIABLE_HPP
#define IDENTIFIABLE_HPP

#include "utilities/IDGenerator.hpp"
#include "utilities/Concepts.hpp"

namespace Engine {

// This works with templated BaseTypes, if to be used with basic types (non templated classes)
// pass type std::type_identity_t 
template<template<typename> typename BaseType, typename SubType>
class Identifiable {
public:
	static ID getTypeID() { return s_typeID; }
	const ID getInstanceID() { return m_id; }
protected:
	Identifiable() = default;
private:
	using BaseTypeConcrete = BaseType<SubType>;

	inline static const ID s_typeID = IDGenerator<BaseTypeConcrete>::template getTypeID<SubType>();
	const ID m_id = IDGenerator<BaseTypeConcrete>::template getInstanceID<SubType>();
};

} // Engine

#endif // !IDENTIFIABLE_HPP
