#pragma once

#include "ienium/glow/core/publictypes.hpp"

namespace ienium::glow
{
    class OpenGLStateManager
    {
      public:
        void SetShader(ResourceId shader_id);
        void SetTexture(ResourceId texture_id);

        void SetBlendMode(BlendFactor src, BlendFactor dst);
        void DisableBlend();
        void SetDepthTestState(bool enabled);

        void SetBuffers(ResourceId vao_id, ResourceId vbo_id, ResourceId ebo_id);

        void SetViewport(int pos_x, int pos_y, int width, int height);

        void Reset();

      private:
        ResourceId currentShaderId = INVALID_RESOURCE;

        ResourceId currentTextureId   = INVALID_RESOURCE;
        int        currentTextureSlot = -1; // Unused

        BlendFactor currentSrcFactor = BLEND_ONE;
        BlendFactor currentDstFactor = BLEND_ONE;

        ResourceId currentVAOId = INVALID_RESOURCE;
        ResourceId currentVBOId = INVALID_RESOURCE;
        ResourceId currentEBOId = INVALID_RESOURCE;

        bool depthTestEnabled = false;
        bool blendEnabled     = false;

        int currentViewportX = 0, currentViewportY = 0;
        int currentViewportWidth = 0, currentViewportHeight = 0;
    };
} // namespace ienium::glow