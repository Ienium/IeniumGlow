#include "ienium/glow/vertexbuffermanager.hpp"

#include <GL/glew.h>
#include <string>
#include <vector>

#include "ienium/glow/memorymanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

#define VERTEX_SIZE 2 // 2 Diemnsions

using namespace ienium::glow;
namespace ienium
{
    void VertexBufferManager::Initialize()
    {
        // Create spirte VAO
        glGenVertexArrays(1, &spriteVAOId);
        glBindVertexArray(spriteVAOId);

        CreateBuffers(spriteVBOId, spriteEBOId);

        glBindBuffer(GL_ARRAY_BUFFER, spriteVBOId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);

        // data order : [x,y,u,v]->[x,y,u,v]
        glVertexAttribPointer(
                0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                (void*)nullptr); // X,Y => stride = 8, offset = 0   NOLINT(cppcoreguidelines-pro-type-cstyle-cast, performance-no-int-to-ptr)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                (void*)(2 *
                        sizeof(float))); // U,V => stride = 8, offset = 2   NOLINT(cppcoreguidelines-pro-type-cstyle-cast, performance-no-int-to-ptr)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void*)(4 * sizeof(float))); // R,G,B,A => stride = 8, offset = 4 NOLINT(cppcoreguidelines-pro-type-cstyle-cast,
                                                           // performance-no-int-to-ptr)
        glEnableVertexAttribArray(2);

        // Unbind to be safe
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferManager::CreateBuffers(ResourceId& vbo_id, ResourceId& ebo_id) // NOLINT(readability-make-member-function-const)
    {
        glGenBuffers(1, &vbo_id); // 1 as we only want to crate a single vertex buffer at a time
        glGenBuffers(1, &ebo_id);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);

        glBindBuffer(GL_ARRAY_BUFFER, spriteVBOId);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void VertexBufferManager::FillSpriteBuffer(MemoryChunk* vbo, MemoryChunk* ebo) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, spriteVBOId);

        glBufferSubData(GL_ARRAY_BUFFER, 0, vbo->usedSize, static_cast<char*>(vbo->blockData) + vbo->startIndex);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteEBOId);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ebo->usedSize, static_cast<char*>(ebo->blockData) + ebo->startIndex);
    }

    void VertexBufferManager::DrawSpriteBuffer() const
    {
        glBindVertexArray(spriteVAOId);
        glDrawElements(GL_TRIANGLES, spriteQuadCount * 6, GL_UNSIGNED_INT, nullptr);
    }

    void VertexBufferManager::DrawSpriteBuffer(unsigned int sprite_count) const
    {
        glBindVertexArray(spriteVAOId);
        glDrawElements(GL_TRIANGLES, sprite_count * 6, GL_UNSIGNED_INT, nullptr);
    }
} // namespace ienium