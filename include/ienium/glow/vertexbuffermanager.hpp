#pragma once

#include "ienium/glow/core/publictypes.hpp"
#include "memorymanager.hpp"

const size_t MAX_QUADS    = 100000;        // per Batch
const size_t MAX_VERTICES = 4 * MAX_QUADS; // per Batch
const size_t MAX_INDICES  = 6 * MAX_QUADS; // per Batch

namespace ienium::glow
{
    class VertexBufferManager
    {
      public:
        void Initialize();

        void FillSpriteBuffer(MemoryChunk* vbo, MemoryChunk* ebo) const;
        void DrawSpriteBuffer() const;
        void DrawSpriteBuffer(uint32_t sprite_count) const;

      private:
        ResourceId spriteVBOId;
        ResourceId spriteVAOId;
        ResourceId spriteEBOId;
        uint32_t   spriteQuadCount;

        // ResourceId lineVAOId;
        // ResourceId lineVBOId;
        // ResourceId lineEBOId;
        //... More VAOs as needed (one pre object type (sprite, line, triangles)

        void CreateBuffers(ResourceId& vbo, ResourceId& ebo);
    };
} // namespace ienium::glow

/*
    VBM hat map von handle und vert2[]
    VBM hat map mit meshInstanceId und Pair(offset, length)
*/
