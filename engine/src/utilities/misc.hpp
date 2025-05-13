#ifndef MISC_HPP
#define MISC_HPP

#include "core/pch.hpp"
#include "utilities/Logger.hpp"

namespace Engine {

template<typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

template<typename T>
bool removeElement(std::vector<T>& vec, int index)
{
    // Swap remove idiom.
    if (index >= 0 && index < vec.size())
    {
        std::swap(vec[index], vec.back()); 
        vec.pop_back();
        return true;
    }
    return false;
}

template<typename ResourceTypeHandle>
bool logResourceValidity(ResourceTypeHandle& handle, const char* name)
{
	if (handle.isValid())
	{
		LOG_INFO("Valid resource: {}", name);
		return true;
	}
	else
	{
		LOG_ERROR("Invalid resource: {}", name);
		return false;
	}
}


} // Engine

#endif // !MISC_HPP
