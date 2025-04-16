#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <concepts>

namespace Engine {

template<typename Derived, typename Base>
concept DerivedFrom = std::is_base_of<Base, Derived>::value;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

} // Engine

#endif // !CONCEPTS_HPP