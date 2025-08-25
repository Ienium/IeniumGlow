#pragma once

#include "ienium/glow/core/Vector.hpp"

namespace ienium::glow
{
    class Camera2D
    {
        public:
        void SetPosition (const Vector2& position);
        void SetAngle (float angle);
        // void SetZoom (float zoom);

        const Vector2& GetPosition () const;
        float GetAngle () const;
    };
}
