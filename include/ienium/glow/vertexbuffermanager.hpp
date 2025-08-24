#pragma once

#include "ienium/glow/core/Vector.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <vector>

namespace ienium
{
    typedef unsigned int VertexArrayId;
    typedef unsigned int VertBufferId ;

    class VertexBufferManager
    {
        public:
        void Initialize ();

        VertBufferId CreateBuffer (std::size_t initial_size);
        void FillBuffer (VertBufferId handle, std::vector<Vector2> vertices);
        void DrawBuffer (VertBufferId handle);

        private:
        VertexArrayId vertexArrayId;
        std::map<VertBufferId, std::vector<float>*> vertexBuffers;
    };
}



/*
    VBM hat map von handle und vert2[]
    VBM hat map mit meshInstanceId und Pair(offset, length)
*/
