#pragma once

#include "ienium/glow/core/publictypes.hpp"

namespace ienium::glow
{
    class OpenGLStateManager
    {
        public:
        void SetShader (ResourceId shader_id);
        void SetTexture (ResourceId texture);

        void SetBlendMode (BlendFactor src, BlendFactor dst);
        void SetBlendState (bool enabled);
        void SetDepthTestState (bool enabled);

        void SetViewport (int x, int y, int width, int height);

        private:
        ResourceId currentShader;

        ResourceId currentTexture;
        int currentTextureSlot;                     // Unused

        BlendFactor currentSrcFactor;
        BlendFactor currentDstFactor;

        ResourceId currentVAO;
        ResourceId currentVBO;
        ResourceId currentEBO;

        bool depthTestEnabled;
        bool blendEnabled;
        BlendFactor blendSrc;
        BlendFactor blendDst;

        int viewportX = 0, viewportY = 0;
        int viewportWidth = 0, viewportHeight = 0;

    };
}