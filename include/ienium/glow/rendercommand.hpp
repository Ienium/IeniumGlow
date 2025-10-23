#pragma once

#include "ienium/glow/core/Vector.hpp"
#include "ienium/utils/color/ieniumcolor.hpp"

namespace ienium::glow
{
    struct RenderCommand
    {
    };

    struct SpriteRenderCommand : public RenderCommand
    {
        Vector2      size;
        Vector2      position;
        utils::Color tint;
        Vector2      textureScale;
        Vector2      textureOffset;
        float        angle;

        char dummy[40]{}; // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)

        SpriteRenderCommand(const Vector2& size, const Vector2& position, const utils::Color& tint, const Vector2& texture_scale,
                            const Vector2& texture_offset, float angle)
            : size(size), position(position), tint(tint), textureScale(texture_scale), textureOffset(texture_offset), angle(angle)
        {
        }
    };
} // namespace ienium::glow