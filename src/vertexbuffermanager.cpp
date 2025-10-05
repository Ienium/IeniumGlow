#include <iostream>
#include <print>
#include <string>
#include <vector>

#include <GL/glew.h>

#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/glow/memorymanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

#define VERTEX_SIZE 2   // 2 Diemnsions

using namespace ienium::glow;
namespace ienium
{
    void VertexBufferManager::Initialize ()
    {
        // Create spirte VAO
        glGenVertexArrays (1, &spriteVAOId);
        glBindVertexArray (spriteVAOId);

        CreateBuffers (spriteVBOId, spriteEBOId);

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);

        // data order : [x,y,u,v]->[x,y,u,v]
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);                          // X,Y => stride = 8, offset = 0
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof (float)));       // U,V => stride = 8, offset = 2
        glEnableVertexAttribArray (1);
        glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof (float)));       // R,G,B,A => stride = 8, offset = 4
        glEnableVertexAttribArray (2);
        
        

        // Unbind to be safe
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferManager::CreateBuffers (ResourceId& vbo_id, ResourceId& ebo_id)
    {
        glGenBuffers (1, &vbo_id);                              // 1 as we only want to crate a single vertex buffer at a time
        glGenBuffers (1, &ebo_id);

        glBindBuffer (GL_ARRAY_BUFFER, vbo_id);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo_id);

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);
        glBufferData (GL_ARRAY_BUFFER, MAX_VERTiCES * 8 *  sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void VertexBufferManager::FillSpriteBuffer (const std::vector<Vector2>& vertices, const std::vector<Vector2>& uvs)
    {
        auto logger = utils::LoggerManager::GetInstance().GetLogger();
        
        if (vertices.size () != uvs.size ())
        {
            logger->Log(utils::IENIUM_WARNING, "Vertex vector and uv vector have different sized. Using vertex vector as base. Filling up uvs with border points if needed");
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

        spriteQuadCount = vertices.size () / 4; // 4 verts per quad
        unsigned int* ebo = new unsigned int[spriteQuadCount * 6];

        for (unsigned int i = 0; i < spriteQuadCount; i++)
        {
            ebo[i * 6 + 0] = i * 4;
            ebo[i * 6 + 1] = i * 4 + 1;
            ebo[i * 6 + 2] = i * 4 + 2;
            ebo[i * 6 + 3] = i * 4;
            ebo[i * 6 + 4] = i * 4 + 2;
            ebo[i * 6 + 5] = i * 4 + 3;
        }

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);
        glBufferSubData (GL_ARRAY_BUFFER, 0, buffer.size () * sizeof(float), buffer.data());

        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);
        glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, 0, spriteQuadCount * 6 * sizeof(unsigned int), ebo);

        delete[] ebo;
    }

    void VertexBufferManager::FillSpriteBuffer (MemoryChunk* vbo, MemoryChunk* ebo)
    {
        auto vbo_data = static_cast<float*>(vbo->blockData) + vbo->startIndex;
        auto ebo_data = static_cast<uint32_t*>(ebo->blockData) + ebo->startIndex;

        //std::println("#############################");
        //for (int i = 0; i < 32; i++)
        //{
        //    std::println("{}", vbo_data[i]);
        //}
        //std::println("#############################");
//
        //for (int i = 0; i < 6; i++)
        //{
        //    std::println("{}", ebo_data[i]);
        //}
        //std::println("#############################");

        glBindBuffer (GL_ARRAY_BUFFER, spriteVBOId);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
        //std::cout << vbo->usedSize << std::endl;
        glBufferSubData (GL_ARRAY_BUFFER, 0, vbo->usedSize, static_cast<char*>(vbo->blockData) + vbo->startIndex);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
        

        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
        glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, 0, ebo->usedSize, static_cast<char*>(ebo->blockData) + ebo->startIndex);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
    }

    void VertexBufferManager::DrawSpriteBuffer () const
    {
        glBindVertexArray (spriteVAOId);
        glDrawElements (GL_TRIANGLES, spriteQuadCount * 6,GL_UNSIGNED_INT, 0);
    }

    void VertexBufferManager::DrawSpriteBuffer (unsigned int sprite_count) const
    {
        glBindVertexArray (spriteVAOId);
        glDrawElements (GL_TRIANGLES, sprite_count * 6,GL_UNSIGNED_INT, 0);
    }
}