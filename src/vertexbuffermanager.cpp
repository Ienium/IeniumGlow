#include <GL/glew.h>
#include <cstddef>
#include <memory>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

#define VERTEX_SIZE 2   // 2 Diemnsions

namespace ienium
{
    void VertexBufferManager::Initialize ()
    {
        // Create spirte VAO
        glGenVertexArrays (1, &spriteVAOId);
        glBindVertexArray (spriteVAOId);

        spriteVBOId = CreateBuffer ();

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);

        // data order : [x,y,u,v]->[x,y,u,v]
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);                          // X,Y => stride = 4, offset = 0
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof (float)));      // U,V => stride = 4, offset = 2
        glEnableVertexAttribArray (0);
        glEnableVertexAttribArray (1);

        // Unbind to be safe
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLid VertexBufferManager::CreateBuffer ()
    {
        GLid new_vert_buffer_id;
        glGenBuffers (1, &new_vert_buffer_id);                              // 1 as we only want to crate a single vertex buffer at a time
        glBindBuffer (GL_ARRAY_BUFFER, new_vert_buffer_id);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return new_vert_buffer_id;
    }

    void VertexBufferManager::FillSpriteBuffer (const std::vector<Vector2>& vertices, const std::vector<Vector2>& uvs)
    {
        auto logger = utils::LoggerManager::GetInstance().GetLogger();
        
        if (vertices.size () != uvs.size ())
        {
            logger->Log(utils::IENIUM_WARNING, "Vertex vector and uv vector have different sized. Using vertex vector as base. Filling up usvs with border points if needed");
            // set uvs to size of vertices and fill up with border points
        }

        std::vector<float> buffer (vertices.size () * VERTEX_SIZE + uvs.size () * 2);
        for (unsigned long long i = 0; i < vertices.size (); ++i)
        {
            buffer[i * 4]       = vertices[i].X ();
            buffer[i * 4 + 1]   = vertices[i].Y ();
            buffer[i * 4 + 2]   = uvs[i].X ();
            buffer[i * 4 + 3]   = uvs[i].Y ();

        } 

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);
        glBufferData (GL_ARRAY_BUFFER, buffer.size () * sizeof(float), buffer.data(), GL_STATIC_DRAW);
    }

    void VertexBufferManager::DrawSpriteBuffer ()
    {
        glBindVertexArray (spriteVAOId);
        glDrawArrays (GL_TRIANGLES, 0, 3);
    }
}