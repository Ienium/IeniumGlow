# include "ienium/glow/renderer2d.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"

#include <GL/glew.h>
#include <memory>

using namespace ienium::utils;

namespace ienium::glow
{
    class Renderer2D::Impl
    {


        public:
        

        private:
        VertexBufferManager vertexBufferManager;


        public:
        // Public API implementation

        void Initialize ()
        {
            vertexBufferManager.Initialize ();
        }

        void Shutdown ()
        {

        }

        void Clear (const ienium::utils::Color& color)
        {
            NOT_YET_IMPLEMENTED;
        }    

        void BeginFrame ()
        {
            NOT_YET_IMPLEMENTED;
        }

        void EndFrame ()
        {
            NOT_YET_IMPLEMENTED;
        }    

        void SetCamera (const Camera2D& camera)
        {
            NOT_YET_IMPLEMENTED;
        }    

        void SetLayer (int layer)
        {
            NOT_YET_IMPLEMENTED;
        } 

        void DrawLine (const Vector2& start, const Vector2& end, const utils::Color& color, float thickness)
        {
            NOT_YET_IMPLEMENTED;
        }

        void DrawLine (const Vector2& start, const Vector2& end, const std::vector<utils::Color>& color_list, float thickness)
        {
            NOT_YET_IMPLEMENTED;
        } 
        
        void DrawText (const std::string& text, ResourceId font, const Vector2& position, const utils::Color& color, Alignment alignment)
        {
            NOT_YET_IMPLEMENTED;
        } 

        void DrawCircle (float radius, const Vector2& position, const utils::Color& color)
        {
            NOT_YET_IMPLEMENTED;
        } 

        void DrawTriangle (const Vector2& size, const Vector2& position, const utils::Color& color, float angle)
        {
            NOT_YET_IMPLEMENTED;
        }

        void DrawTriangle (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle)
        {
            NOT_YET_IMPLEMENTED;
        } 

        void DrawRectangle (const Vector2& size, const Vector2& position, const utils::Color& color, float angle)
        {
            NOT_YET_IMPLEMENTED;
        }

        void DrawRectangle (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle)
        {
            NOT_YET_IMPLEMENTED;
        } 


        void DrawEllipse (const Vector2& size, const Vector2& position, const utils::Color& color, float angle)
        {
            NOT_YET_IMPLEMENTED;
        }

        void DrawEllipse (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle)
        {
            NOT_YET_IMPLEMENTED;
        } 

        void DrawSprite (const Vector2& position, ResourceId texture_id)
        {
            NOT_YET_IMPLEMENTED;
            LOGGER->Log (utils::IENIUM_WARNING, "Sprite functionality not yet implemented. Only redners single colord quad!");

            std::vector<Vector2> mesh = {
                Vector2 (-0.5,-0.5) + position,
                Vector2 (0.5, -0.5) + position,
                Vector2 (0.5, 0.5) + position,
                Vector2 (-0.5, 0.5) + position,
            };

            vertexBufferManager.FillSpriteBuffer (mesh, mesh);
            vertexBufferManager.DrawSpriteBuffer ();
        }

        void DrawSprite (const Vector2& size, const Vector2& position, ResourceId texture_id, const utils::Color& tint, const Vector2& texture_scale, const Vector2& texture_offset, float angle) const
        {
            NOT_YET_IMPLEMENTED;
        }
    };

    Renderer2D::Renderer2D ()
    {

    }

    Renderer2D::~Renderer2D()
    {

    }

    bool Renderer2D::Initialize ()
    {
        impl = std::make_unique<Impl>();
        impl->Initialize ();
        return true;
    }

    void Renderer2D::Shutdown ()
    {
        
    }    

    void Renderer2D::Clear (const ienium::utils::Color& color)
    {
        impl->Clear (color);
    }    

    void Renderer2D::BeginFrame ()
    {
        impl->BeginFrame ();
    }

    void Renderer2D::EndFrame ()
    {
        impl->EndFrame ();
    }    

    void Renderer2D::SetCamera (const Camera2D& camera)
    {
        impl->SetCamera (camera);
    }    

    void Renderer2D::SetLayer (int layer)
    {
        impl->SetLayer (layer);
    } 

    void Renderer2D::DrawLine (const Vector2& start, const Vector2& end, const utils::Color& color, float thickness) const
    {
        impl->DrawLine (start, end, color, thickness);
    }

    void Renderer2D::DrawLine (const Vector2& start, const Vector2& end, const std::vector<utils::Color>& color_list, float thickness) const
    {
        impl->DrawLine (start, end, color_list, thickness);
    } 
       
    void Renderer2D::DrawText (const std::string& text, ResourceId font, const Vector2& position, const utils::Color& color, Alignment alignment) const
    {
        impl->DrawText (text, font, position, color, alignment);
    } 

    void Renderer2D::DrawCircle (float radius, const Vector2& position, const utils::Color& color) const
    {
        impl->DrawCircle (radius, position, color);
    } 

    void Renderer2D::DrawTriangle (const Vector2& size, const Vector2& position, const utils::Color& color, float angle) const
    {
        impl->DrawTriangle (size, position, color, angle);
    }

    void Renderer2D::DrawTriangle (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle) const
    {
        impl->DrawTriangle (size, position, color_list, angle);
    } 

    void Renderer2D::DrawRectangle (const Vector2& size, const Vector2& position, const utils::Color& color, float angle) const
    {
        impl->DrawRectangle (size, position, color, angle);
    }

    void Renderer2D::DrawRectangle (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle) const
    {
        impl->DrawRectangle (size, position, color_list, angle);
    } 

    void Renderer2D::DrawEllipse (const Vector2& size, const Vector2& position, const utils::Color& color, float angle) const
    {
        impl->DrawEllipse (size, position, color, angle);
    }

    void Renderer2D::DrawEllipse (const Vector2& size, const Vector2& position, const std::vector<utils::Color>& color_list, float angle) const
    {
        impl->DrawEllipse (size, position, color_list, angle);
    } 

    void Renderer2D::DrawSprite (const Vector2& position, ResourceId texture_id) const
    {
        impl->DrawSprite (position, texture_id);
    }

    void Renderer2D::DrawSprite (const Vector2& size, const Vector2& position, ResourceId texture_id, const utils::Color& tint, const Vector2& texture_scale, const Vector2& texture_offset, float angle) const
    {
        impl-> DrawSprite(size, position, texture_id, tint, texture_scale, texture_offset, angle);
    }
}