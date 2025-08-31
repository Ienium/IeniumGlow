#include "GL/glew.h"

#include "ienium/glow/openglstatemanager.hpp"

namespace ienium::glow
{
    void OpenGLStateManager::SetTexture (ResourceId texture)
    {
        if (texture != currentTextureId)
        {
            currentTextureId = texture;
            glBindTexture(GL_TEXTURE_2D, texture);
        }
    }

    void OpenGLStateManager::SetShader (ResourceId shader_id)
    {
        if (shader_id != currentShaderId)
        {
            currentShaderId = shader_id;
            glUseProgram (shader_id);
        }
    }

    void OpenGLStateManager::SetBlendMode (BlendFactor src, BlendFactor dst)
    {
        if (src != currentSrcFactor || dst != currentDstFactor)
        {
            currentSrcFactor = src;
            currentDstFactor = dst;
            glBlendFunc (src, dst);
        }
        if (!blendEnabled)
        {
            glEnable (GL_BLEND);
            blendEnabled = true;
        }
    }

    void OpenGLStateManager::DisableBlend ()
    {
        if (blendEnabled)
            glDisable (GL_BLEND);
        blendEnabled = false;
    }

    void OpenGLStateManager::SetDepthTestState (bool enabled)
    {
        if (depthTestEnabled != enabled)
        {
            if (enabled)
                glEnable (GL_DEPTH_TEST);
            else 
                glDisable (GL_DEPTH_TEST);
            
            depthTestEnabled = enabled;
        }
    }

    void OpenGLStateManager::SetBuffers (ResourceId vao_id, ResourceId vbo_id, ResourceId ebo_id)
    {
        if (vao_id != currentVAOId || vbo_id != currentVBOId || ebo_id != currentEBOId)
        {
            glBindVertexArray (vao_id);
            glBindBuffer (GL_ARRAY_BUFFER, vbo_id);
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo_id);

            currentVAOId = vao_id;
            currentVBOId = vbo_id;
            currentEBOId = ebo_id;
        }
    }

    void OpenGLStateManager::SetViewport (int x, int y, int width, int height)
    {
        if (x != currentViewportX || y != currentViewportY || width != currentViewportWidth || height != currentViewportHeight)
        {
            currentViewportX = x;
            currentViewportY = y;
            currentViewportWidth = width;
            currentViewportHeight = height;

            glViewport(x, y, width, height);
        }
    }

    void OpenGLStateManager::Reset ()
    {
        currentShaderId = INVALID_RESOURCE;
        currentTextureId = INVALID_RESOURCE;
        currentSrcFactor = BLEND_ONE;
        currentDstFactor = BLEND_ONE;
        currentVAOId = INVALID_RESOURCE;
        currentVBOId = INVALID_RESOURCE;
        currentEBOId = INVALID_RESOURCE;
        depthTestEnabled = false;
        blendEnabled = false;
        currentViewportX = 0, currentViewportY = 0;
        currentViewportWidth = 0, currentViewportHeight = 0;
    }

}