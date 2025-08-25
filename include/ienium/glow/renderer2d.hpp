#pragma once

#include <memory>
#include <vector>

#include "ienium/glow/core/publictypes.hpp"
#include "ienium/glow/core/Vector.hpp"
#include "ienium/utils/color/ieniumcolor.hpp"
#include "ienium/glow/camera_2d.hpp"

namespace ienium::glow
{
    enum Alignment {
        LEFT,
        TOP,
        RIGHT,
        BOTTOM,
        CENTER,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    class Renderer2D
    {
        public:
            Renderer2D ();
            ~Renderer2D ();

            bool Initialize ();
            void Shutdown ();

            void Clear (const ienium::utils::Color& color = ienium::utils::Color::Black ());

            void BeginFrame ();
            void EndFrame ();

            void SetCamera (const Camera2D& camera);

            void SetLayer (int layer);

            void DrawLine       (const Vector2& start, const Vector2& end, const utils::Color& color = utils::Color::Blue (), float thickness = 1.f) const;
            void DrawLine       (const Vector2& start, const Vector2& end, const std::vector<utils::Color>& color_list, float thickness = 1.f) const;

            void DrawText       (const std::string& text, ResourceId font, const Vector2& position, const utils::Color& color = utils::Color::White (), Alignment alignment = CENTER) const;

            void DrawCircle     (float radius, const Vector2& position, const utils::Color& color = utils::Color::Blue ()) const;

            void DrawTriangle   (const Vector2& size, const Vector2& position, const utils::Color& color = utils::Color::Blue (), float angle = 0) const;
            void DrawTriangle   (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle = 0) const;

            void DrawRectangle  (const Vector2& size, const Vector2& position, const utils::Color& color = utils::Color::Blue (), float angle = 0) const;
            void DrawRectangle  (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle = 0) const;

            void DrawEllipse    (const Vector2& size, const Vector2& position, const utils::Color& color = utils::Color::Blue (), float angle = 0) const;
            void DrawEllipse    (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle = 0) const;

            void DrawSprite     (const Vector2& position, ResourceId texture_id) const;
            void DrawSprite     (const Vector2& size, const Vector2& position, ResourceId texture_id, const utils::Color& tint = utils::Color::White (), const Vector2& texture_scale = Vector2::One (), const Vector2& texture_offset = Vector2::Zero (), float angle = 0) const;

        private:
            class Impl;
            std::unique_ptr<Impl> impl;
    };
}
