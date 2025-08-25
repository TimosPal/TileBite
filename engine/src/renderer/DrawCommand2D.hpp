#ifndef DRAW_COMMAND_2D_HPP
#define DRAW_COMMAND_2D_HPP

#include "renderer/SpriteQuad.hpp"

namespace TileBite {

// [UNSUED]

enum class DrawCommand2DType {
    Quad,
    Line
};

struct DrawCommand2D {
    DrawCommand2DType type;
    union {
        SpriteQuad spriteQuad;
    };
};

} // TileBite

#endif // !DRAW_COMMAND_2D_HPP