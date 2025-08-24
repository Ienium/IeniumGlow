#pragma once

#include "ienium/glow/core/Vector.hpp"

#include <vector>

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
        GLid lineVAOId;
        //... More VAOs as needed (one pre object type (sprite, line, triangles)

        GLid CreateBuffer ();
        
    };
}



/*
    VBM hat map von handle und vert2[]
    VBM hat map mit meshInstanceId und Pair(offset, length)
*/
