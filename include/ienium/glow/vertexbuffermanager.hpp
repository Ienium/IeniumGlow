#pragma once

#include "ienium/glow/core/Vector.hpp"

#include <vector>

#define MAX_VERTICES (size_t) MAX_QUADS * 4

const size_t MAX_QUADS      = 16384;        // per Batch
const size_t MAX_VERTiCES   = 4 * 16384;    // per Batch
const size_t MAX_INDICES    = 6 * 16384;    // per Batch

namespace ienium
{
    typedef unsigned int GLid;
    class VertexBufferManager
    {
        public:
        void Initialize ();

        
        void FillSpriteBuffer (const std::vector<Vector2>& vertices, const std::vector<Vector2>& uvs);
        void DrawSpriteBuffer ();

        private:
        GLid spriteVBOId;
        GLid spriteVAOId;
        GLid spriteEBOId;
        unsigned int spriteQuadCount;

        GLid lineVAOId;
        GLid lineVBOId;
        GLid lineEBOId;
        //... More VAOs as needed (one pre object type (sprite, line, triangles)

        void CreateBuffers (GLid& vbo, GLid& ebo) const;
        
    };
}



/*
    VBM hat map von handle und vert2[]
    VBM hat map mit meshInstanceId und Pair(offset, length)
*/
