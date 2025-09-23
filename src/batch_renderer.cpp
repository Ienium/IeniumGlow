#include "ienium/glow/batch_renderer.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/glow/core/publictypes.hpp"
#include "ienium/glow/rendercommand.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <print>
#include <string>
namespace ienium::glow
{
    void BatchingSystem::BeginFrame ()
    {
        ResetAllBatches ();
    }

    void BatchingSystem::EndFrame ()
    {
        ProcessBatches ();
        DrawBatches ();
        //SortBatches ();
    }

    void BatchingSystem::ResetAllBatches ()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            batch.memoryChunksVBO.clear ();
            batch.memoryChunksEBO.clear ();
            batch.draw_count = 0;
        }
    }

    RenderBatch& BatchingSystem::RequestBatch (int layer, ResourceId texture_id, RenderType type)
    {
        size_t hash = CalculateHash (layer, texture_id, type);
        if (!batches.contains (hash))
        {
            auto batch = RenderBatch (layer, texture_id, type);
            batches.emplace(hash, batch);
        }

        auto& batch = batches.at (hash);

        if (batch.memoryChunksVBO.empty () && batch.lastMemorySize != INVALID_MEMORY_SIZE)
            batch.memoryChunksVBO.push_back (memoryManager.RequestMemoryChunk(batch.lastMemorySize));
        return batch;
    }

    size_t BatchingSystem::CalculateHash (int layer, ResourceId texture_id, RenderType type)
    {
        size_t h1 = std::hash<uint32_t> {} (layer);
        size_t h2 = std::hash<uint32_t> {} (texture_id);
        size_t h3 = std::hash<uint8_t> {} (type);

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }


    void BatchingSystem::AddSpriteRenderCommand (RenderBatch& batch, const Vector2& size, const Vector2& position, ResourceId texture_id, const utils::Color& tint, const Vector2& texture_scale, const Vector2& texture_offset, float angle)
    {
        AddRenderCommand<SpriteRenderCommand>(batch, size, position, tint, texture_scale, texture_offset, angle);
    }

    bool operator<(const RenderBatch& a, const RenderBatch& b)
    {
        if (a.layer != b.layer) return a.layer < b.layer;
        if (a.textureId != b.textureId) return a.textureId < b.textureId;
        return a.type < b.type;
    }

    void BatchingSystem::ProcessBatches ()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            
            switch (batch.type)
            {
                case SPRITE:
                    ProcessSpriteBatch (batch);
                    break;
                default:
                    break;
            }
        }
    }

    void BatchingSystem::DrawBatches ()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            
            switch (batch.type)
            {
                case SPRITE:
                    DrawSpriteBatch (batch);
                    break;
                default:
                    break;
            }
        }
    }

    void BatchingSystem::ProcessSpriteBatch (RenderBatch& batch)
    {
        batch.lastMemorySize = 0;

        for (auto memory_chunk : batch.memoryChunksVBO)
        {
            auto memory_as_render_command = static_cast<SpriteRenderCommand*>(memory_chunk->data);
            auto memory_as_buffer = static_cast<float*>(memory_chunk->data);

            size_t draw_count_in_chunk = memory_chunk->currentSize / ELEMENT_SIZE_MAP.at (SPRITE);
            size_t required_ebo_size = 6 * draw_count_in_chunk * sizeof(unsigned int);

            MemoryChunk* ebo_chunk = memoryManager.RequestMemoryChunk ( required_ebo_size);

            auto ebo_chunk_as_ebo = static_cast<unsigned int*>(ebo_chunk->data);
            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                ebo_chunk_as_ebo[index * 6 + 1] = (unsigned int)(index) * 4 + 1;
                ebo_chunk_as_ebo[index * 6 + 2] = (unsigned int)(index) * 4 + 2;
                ebo_chunk_as_ebo[index * 6 + 3] = (unsigned int)(index) * 4;
                ebo_chunk_as_ebo[index * 6 + 4] = (unsigned int)(index) * 4 + 2;
                ebo_chunk_as_ebo[index * 6 + 5] = (unsigned int)(index) * 4 + 3;
                ebo_chunk_as_ebo[index * 6 + 0] = (unsigned int)(index) * 4;
            }

            ebo_chunk->currentSize = required_ebo_size;

            batch.memoryChunksEBO.push_back (ebo_chunk);


            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                SpriteRenderCommand command = memory_as_render_command[index];
                memory_as_buffer[index * 32 + 0] = command.position.X () - command.size.X () / 2.f;
                memory_as_buffer[index * 32 + 1] = command.position.Y () - command.size.Y () / 2.f;
                memory_as_buffer[index * 32 + 2] = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_buffer[index * 32 + 3] = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_buffer[index * 32 + 4] = command.tint.r ();
                memory_as_buffer[index * 32 + 5] = command.tint.g ();
                memory_as_buffer[index * 32 + 6] = command.tint.b ();
                memory_as_buffer[index * 32 + 7] = command.tint.a ();

                memory_as_buffer[index * 32 + 8] = command.position.X () + command.size.X () / 2.f;
                memory_as_buffer[index * 32 + 9] = command.position.Y () - command.size.Y () / 2.f;
                memory_as_buffer[index * 32 + 10] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_buffer[index * 32 + 11] = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_buffer[index * 32 + 12] = command.tint.r ();
                memory_as_buffer[index * 32 + 13] = command.tint.g ();
                memory_as_buffer[index * 32 + 14] = command.tint.b ();
                memory_as_buffer[index * 32 + 15] = command.tint.a ();

                memory_as_buffer[index * 32 + 16] = command.position.X () + command.size.X () / 2.f;
                memory_as_buffer[index * 32 + 17] = command.position.Y () + command.size.Y () / 2.f;
                memory_as_buffer[index * 32 + 18] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_buffer[index * 32 + 19] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_buffer[index * 32 + 20] = command.tint.r ();
                memory_as_buffer[index * 32 + 21] = command.tint.g ();
                memory_as_buffer[index * 32 + 22] = command.tint.b ();
                memory_as_buffer[index * 32 + 23] = command.tint.a ();

                memory_as_buffer[index * 32 + 24] = command.position.X () - command.size.X () / 2.f;
                memory_as_buffer[index * 32 + 25] = command.position.Y () + command.size.Y () / 2.f;
                memory_as_buffer[index * 32 + 26] = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_buffer[index * 32 + 27] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_buffer[index * 32 + 28] = command.tint.r ();
                memory_as_buffer[index * 32 + 29] = command.tint.g ();
                memory_as_buffer[index * 32 + 30] = command.tint.b ();
                memory_as_buffer[index * 32 + 31] = command.tint.a ();
            }
            batch.lastMemorySize += memory_chunk->currentSize;
        }
    }

    void BatchingSystem::DrawSpriteBatch (RenderBatch& batch)
    {
        for (size_t index = 0; index < batch.memoryChunksVBO.size (); index++)
        {
            vertexBufferManager.FillSpriteBuffer(batch.memoryChunksVBO[index], batch.memoryChunksEBO[index]);
            vertexBufferManager.DrawSpriteBuffer(batch.memoryChunksEBO[index]->currentSize / 6);
        }
    }

    /*
    void BatchingSystem::ProcessBatches ()
    {
        for (auto& command : spriteRenderCommands)
        {
            if (batches.contains( command.batchKey))
            {
                auto& batch = batches.at (command.batchKey);
                ++batch.draw_count;
            }
        }

        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            batch.memoryChunk = memoryManager.RequestMemoryChunk(batch.draw_count * ELEMENT_SIZE_MAP.at (batch.type));
        }

        size_t index = 0;
        for (auto& command : spriteRenderCommands)
        {
            if (batches.contains( command.batchKey))
            {
                auto& batch = batches.at (command.batchKey);
                auto memory = static_cast<float*>(batch.memoryChunk->data);
                
                memory[index + 0] = command.position.X () - command.size.X () / 2.f;
                memory[index + 1] = command.position.Y () - command.size.Y () / 2.f;
                memory[index + 2] = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory[index + 3] = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory[index + 4] = command.tint.r ();
                memory[index + 5] = command.tint.g ();
                memory[index + 6] = command.tint.b ();
                memory[index + 7] = command.tint.a ();

                memory[index + 8] = command.position.X () + command.size.X () / 2.f;
                memory[index + 9] = command.position.Y () - command.size.Y () / 2.f;
                memory[index + 10] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory[index + 11] = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory[index + 12] = command.tint.r ();
                memory[index + 13] = command.tint.g ();
                memory[index + 14] = command.tint.b ();
                memory[index + 15] = command.tint.a ();

                memory[index + 16] = command.position.X () + command.size.X () / 2.f;
                memory[index + 17] = command.position.Y () + command.size.Y () / 2.f;
                memory[index + 18] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory[index + 19] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory[index + 20] = command.tint.r ();
                memory[index + 21] = command.tint.g ();
                memory[index + 22] = command.tint.b ();
                memory[index + 23] = command.tint.a ();

                memory[index + 24] = command.position.X () - command.size.X () / 2.f;
                memory[index + 25] = command.position.Y () + command.size.Y () / 2.f;
                memory[index + 26] = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory[index + 27] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory[index + 28] = command.tint.r ();
                memory[index + 29] = command.tint.g ();
                memory[index + 30] = command.tint.b ();
                memory[index + 31] = command.tint.a ();

                index += 32;
            }
        }
    }
*/
}