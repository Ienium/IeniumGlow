#pragma once

namespace ienium
{
    typedef unsigned int ResourceId;
    const ResourceId INVALID_RESOURCE = -1;

    enum BlendFactor {
        BLEND_ZERO  = 0,                            // GL_ZERO
        BLEND_ONE  = 1,                             // GL_ONE
        BLEND_SRC_COLOR  = 0x0300,                  // GL_SRC_COLOR
        BLEND_ONE_MINUS_SRC_COLOR  = 0x0301,        // GL_ONE_MINUS_SRC_COLOR
        BLEND_SRC_ALPHA  = 0x0302,                  // GL_SRC_ALPHA
        BLEND_ONE_MINUS_SRC_ALPHA  = 0x0303,        // GL_ONE_MINUS_SRC_ALPHA
        BLEND_DST_ALPHA  = 0x0304,                  // GL_DST_ALPHA
        BLEND_ONE_MINUS_DST_ALPHA  = 0x0305,        // GL_ONE_MINUS_DST_ALPHA
        BLEND_DST_COLOR  = 0x0306,                  // GL_DST_COLOR
        BLEND_ONE_MINUS_DST_COLOR  = 0x0307,        // GL_ONE_MINUS_DST_COLOR
        BLEND_CONSTANT_COLOR  = 0x8001,             // GL_CONSTANT_COLOR
        BLEND_ONE_MINUS_CONSTANT_COLOR  = 0x8002,   // GL_ONE_MINUS_CONSTANT_COLOR
        BLEND_CONSTANT_ALPHA  = 0x8003,             // GL_CONSTANT_ALPHA
        BLEND_ONE_MINUS_CONSTANT_ALPHA  = 0x8004,   // GL_ONE_MINUS_CONSTANT_ALPHA
    };
}