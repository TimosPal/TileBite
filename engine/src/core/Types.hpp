#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

namespace TileBite {

using ID = uint32_t;
constexpr ID INVALID_ID = -1;

struct Component; // Used for ID groupping only.

} // TileBite

#endif // !TYPES_HPP