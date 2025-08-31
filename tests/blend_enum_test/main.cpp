#include <cassert>
#include <cmath>
#include <print>

#include "GL/glew.h"
#include "ienium/glow/core/publictypes.hpp"

using namespace ienium;

int main ()
{
    static_assert (BLEND_ZERO == GL_ZERO, "Value of BLEND_Zero doesent Match OpenGl verion");
    static_assert (BLEND_ONE == GL_ONE, "Value of BLEND_One doesent Match OpenGl verion");
    static_assert (BLEND_SRC_COLOR == GL_SRC_COLOR, "Value of BLEND_SrcColor doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_SRC_COLOR == GL_ONE_MINUS_SRC_COLOR, "Value of BLEND_OneMinusSrcColor doesent Match OpenGl verion");
    static_assert (BLEND_SRC_ALPHA == GL_SRC_ALPHA, "Value of BLEND_SrcAlpha doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_SRC_ALPHA == GL_ONE_MINUS_SRC_ALPHA, "Value of BLEND_OneMinusSrcAlpha doesent Match OpenGl verion");
    static_assert (BLEND_DST_ALPHA == GL_DST_ALPHA, "Value of BLEND_DstAlpha doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_DST_ALPHA == GL_ONE_MINUS_DST_ALPHA, "Value of BLEND_OneMinusDstAlpha doesent Match OpenGl verion");
    static_assert (BLEND_DST_COLOR == GL_DST_COLOR, "Value of BLEND_DstColor doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_DST_COLOR == GL_ONE_MINUS_DST_COLOR, "Value of BLEND_OneMinusDstColor doesent Match OpenGl verion");
    static_assert (BLEND_CONSTANT_COLOR == GL_CONSTANT_COLOR, "Value of BLEND_ConstantColor doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_CONSTANT_COLOR == GL_ONE_MINUS_CONSTANT_COLOR, "Value of BLEND_OneMinusConstantColor doesent Match OpenGl verion");
    static_assert (BLEND_CONSTANT_ALPHA == GL_CONSTANT_ALPHA, "Value of BLEND_ConstantAlpha doesent Match OpenGl verion");
    static_assert (BLEND_ONE_MINUS_CONSTANT_ALPHA == GL_ONE_MINUS_CONSTANT_ALPHA, "Value of BLEND_OneMinusConstantAlpha doesent Match OpenGl verion");
}
