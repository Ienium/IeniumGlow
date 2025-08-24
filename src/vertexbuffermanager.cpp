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
        glGenVertexArrays (1, &vertexArrayId);
        glBindVertexArray (vertexArrayId);
    }

    VertBufferId VertexBufferManager::CreateBuffer (std::size_t initial_vertex_count)
    {
        VertBufferId new_vert_buffer_id;
        glGenBuffers (1, &new_vert_buffer_id); // 1 as we only want to crate a single vertex buffer at a time
        glBindBuffer (GL_ARRAY_BUFFER, new_vert_buffer_id);

        auto vert_buffer = new std::vector<float> (initial_vertex_count * 2);

        vertexBuffers.emplace (new_vert_buffer_id, vert_buffer);
        return new_vert_buffer_id;
    }

    void VertexBufferManager::FillBuffer (VertBufferId handle, std::vector<Vector2> vertices)
    {
        auto logger = utils::LoggerManager::GetInstance().GetLogger();
        auto vert_buffer = vertexBuffers.at(handle);

        std::println("{} != {}", vertices.size (), vert_buffer->size ());
        if (vertices.size () * VERTEX_SIZE != vert_buffer->size ())
        {
            logger->Log(utils::IENIUM_WARNING, "Tried to change buffer size for vertexbuffer with ID: " + std::to_string (handle) + "\nVertices not uploaded. Call ResizeBuffer (VertBufferId handle, size_t new_size) first");
            glBufferData (GL_ARRAY_BUFFER, vert_buffer->size () * sizeof(float), vert_buffer->data(), GL_STATIC_DRAW);
            return;
        }

        for (size_t i = 0; i < vertices.size (); ++i)
        {
            (*vert_buffer)[i*2] = vertices[i].X ();
            (*vert_buffer)[i*2 + 1] = vertices[i].Y ();
        }


        glBufferData (GL_ARRAY_BUFFER, vert_buffer->size () * sizeof(float), vert_buffer->data(), GL_STATIC_DRAW);
    }

    void VertexBufferManager::DrawBuffer (VertBufferId handle)
    {
        glEnableVertexAttribArray (0);
        glBindBuffer (GL_ARRAY_BUFFER, handle);
        glVertexAttribPointer (
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );

        glDrawArrays (GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray (0);
    }
}