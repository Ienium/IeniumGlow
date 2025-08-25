#pragma once

#include "ienium/glow/core/Vector.hpp"
#include "ienium/glow/renderer2d.hpp"

#include <vector>

#define MAX_VERTICES (size_t) MAX_QUADS * 4

const size_t MAX_QUADS      = 16384;        // per Batch
const size_t MAX_VERTiCES   = 4 * 16384;    // per Batch
const size_t MAX_INDICES    = 6 * 16384;    // per Batch

namespace ienium
{
    class VertexBufferManager
    {
        public:
        void Initialize ();

        
        void FillSpriteBuffer (const std::vector<Vector2>& vertices, const std::vector<Vector2>& uvs);
        void DrawSpriteBuffer () const;

        private:
        ResourceId spriteVBOId;
        ResourceId spriteVAOId;
        ResourceId spriteEBOId;
        unsigned int spriteQuadCount;

        ResourceId lineVAOId;
        ResourceId lineVBOId;
        ResourceId lineEBOId;
        //... More VAOs as needed (one pre object type (sprite, line, triangles)

        void CreateBuffers (ResourceId& vbo, ResourceId& ebo);
        
    };
}



/*
    VBM hat map von handle und vert2[]
    VBM hat map mit meshInstanceId und Pair(offset, length)
*/
