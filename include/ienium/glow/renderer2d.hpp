#pragma once

#include "ienium/utils/color/ieniumcolor.hpp"
#include <memory>

namespace ienium::glow
{
    class Renderer2D
    {
        public:
            Renderer2D ();
            ~Renderer2D ();

            bool Initialize ();
            void Shutdown ();

            void Clear (const ienium::utils::Color& color = ienium::utils::Color::Black ());
            void DrawTriangle ();
            // void DrawSprite (const Vec2& pos, const ienium::utils::Color& color);
        private:
            class Impl;
            std::unique_ptr<Impl> impl;
    };
}
