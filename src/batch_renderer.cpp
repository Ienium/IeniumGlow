#include "ienium/glow/batch_renderer.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <print>
#include <string>
#include <thread>

#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/glow/core/publictypes.hpp"
#include "ienium/glow/rendercommand.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

namespace ienium::glow
{
    void BatchingSystem::BeginFrame()
    {
        ResetAllBatches();
    }

    void BatchingSystem::EndFrame()
    {
        ProcessBatches();
        // SortBatches ();
        DrawBatches();
    }

    void BatchingSystem::ResetAllBatches()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            batch.memoryChunksVBO.clear();
            batch.memoryChunksEBO.clear();
            batch.draw_count = 0;
            batch.next_index = 0;
        }
    }

    RenderBatch& BatchingSystem::RequestBatch(int layer, ResourceId texture_id, RenderType type)
    {
        size_t hash = CalculateHash(layer, texture_id, type);

        // Create new batch if needed
        if (!batches.contains(hash))
        {
            size_t align;
            switch (type)
            {
            // TODO: Add other type aligns
            case SPRITE:
                align = alignof(SpriteRenderCommand);
                break;

            default:
                LOGGER->Log(utils::IENIUM_ERROR, "Align for requested type not defined.");
                align = -1;
                break;
            }
            auto batch = RenderBatch(layer, texture_id, type, align);
            batches.emplace(hash, batch);
        }

        auto& batch = batches.at(hash);

        // Request memory for batch from prediction
        if (batch.memoryChunksVBO.empty() && batch.lastMemorySize != INVALID_MEMORY_SIZE)
            batch.memoryChunksVBO.push_back(memoryManager->RequestMemoryChunk(batch.lastMemorySize * 1.25F, batch.vboAlign));

        return batch;
    }

    size_t BatchingSystem::CalculateHash(int layer, ResourceId texture_id, RenderType type)
    {
        size_t heu_1 = std::hash<uint32_t>{}(layer);
        size_t heu_2 = std::hash<uint32_t>{}(texture_id);
        size_t heu_3 = std::hash<uint8_t>{}(type);

        return heu_1 ^ (heu_2 << 1) ^ (heu_3 << 2);
    }

    void BatchingSystem::AddSpriteRenderCommand(RenderBatch& batch, const Vector2& size, const Vector2& position, ResourceId texture_id,
                                                const utils::Color& tint, const Vector2& texture_scale, const Vector2& texture_offset, float angle)
    {
        AddRenderCommand<SpriteRenderCommand>(batch, size, position, tint, texture_scale, texture_offset, angle);
    }

    void BatchingSystem::LogStats()
    {
        std::string msg = "Batches:\t" + std::to_string(batches.size());
        for (auto& batch_pair : batches)
        {
            msg.append("\nHash: " + std::to_string(batch_pair.first) + "\tChunk count: " + std::to_string(batch_pair.second.memoryChunksVBO.size()));
            for (auto& chunk_pos : batch_pair.second.memoryChunksVBO)
            {
                auto* chunk = memoryManager->GetMemoryChunk(chunk_pos);
                msg.append("\n\tUsage: " + std::to_string(chunk->usedSize) + " / " + std::to_string(chunk->size));
            }
        }

        LOGGER->Log(utils::IENIUM_DEBUG, msg);
    }

    bool operator<(const RenderBatch& first, const RenderBatch& second)
    {
        if (first.layer != second.layer)
            return first.layer < second.layer;
        if (first.textureId != second.textureId)
            return first.textureId < second.textureId;
        return first.type < second.type;
    }

    void BatchingSystem::ProcessBatch(RenderBatch& batch)
    {
        switch (batch.type)
        {
        case SPRITE:
            ProcessSpriteBatch(batch);
            break;
        default:
            break;
        }
    }

    void BatchingSystem::ProcessBatches()
    {
        if (batches.empty())
            return;

        std::vector<RenderBatch*> batch_list;
        batch_list.reserve(batches.size());
        for (auto& batch_pair : batches)
        {
            batch_list.push_back(&batch_pair.second);
        }

        std::atomic<size_t> next_index{0};
        const size_t        TOTAL = batch_list.size();

        std::vector<std::thread> threads;

        threads.reserve(3);
        for (int i = 0; i < 3; i++)
        {
            threads.emplace_back(
                    [&, i]()
                    {
                        for (;;)
                        {
                            size_t idx = next_index.fetch_add(1);
                            if (idx >= TOTAL)
                                break;
                            ProcessBatch(*batch_list[idx]);
                        }
                    });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    void BatchingSystem::DrawBatches()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;

            switch (batch.type)
            {
            case SPRITE:
                DrawSpriteBatch(batch);
                break;
            default:
                break;
            }
        }
    }

    void BatchingSystem::ProcessSpriteBatch(RenderBatch& batch)
    {
        batch.lastMemorySize = 0;

        for (auto& chunk_pos : batch.memoryChunksVBO)
        {
            memoryManager->chunkAcessMutex.lock();
            auto* vbo_chunk             = memoryManager->GetMemoryChunk(chunk_pos);
            auto* vbo_as_render_command = vbo_chunk->GetFirstValue<SpriteRenderCommand>();

            // Get Target overwrite for allocated memory
            auto*  vbo_chunk_as_vbo    = reinterpret_cast<float*>(static_cast<char*>(vbo_chunk->blockData) + vbo_chunk->startIndex);
            size_t draw_count_in_chunk = vbo_chunk->usedSize / ELEMENT_SIZE_MAP.at(SPRITE);
            memoryManager->chunkAcessMutex.unlock();

            size_t required_ebo_size = 6 * draw_count_in_chunk * sizeof(unsigned int);

            memoryManager->chunkAcessMutex.lock();
            // Request memory for EBO
            MemoryChunkPos ebo_chunk_pos    = memoryManager->RequestMemoryChunk(required_ebo_size, alignof(uint32_t));
            auto*          ebo_chunk        = memoryManager->GetMemoryChunk(ebo_chunk_pos);
            auto*          ebo_chunk_as_ebo = reinterpret_cast<uint32_t*>(static_cast<char*>(ebo_chunk->blockData) + ebo_chunk->startIndex);

            // Fill EBO
            // 6 Elements per sprite => 24 bytes per sprite
            size_t ebo_used_size = (ebo_chunk->startIndex + alignof(uint32_t) - 1) & (~(alignof(uint32_t) - 1) - ebo_chunk->startIndex);
            size_t ebo_max_size  = ebo_chunk->size;
            memoryManager->chunkAcessMutex.unlock();

            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                if (ebo_used_size + (6 * sizeof(uint32_t)) > ebo_max_size)
                {
                    LOGGER->Log(utils::IENIUM_ERROR, "Cannot Draw Sprite. Not enough memory in ebo chunk!\n" + std::to_string(ebo_used_size) + "/" +
                                                             std::to_string(ebo_max_size));
                    break;
                }
                ebo_used_size += 6 * sizeof(uint32_t);
                ebo_chunk_as_ebo[(index * 6) + 1] = ((unsigned int)(index) * 4) + 1;
                ebo_chunk_as_ebo[(index * 6) + 2] = ((unsigned int)(index) * 4) + 2;
                ebo_chunk_as_ebo[(index * 6) + 3] = (unsigned int)(index) * 4;
                ebo_chunk_as_ebo[(index * 6) + 4] = ((unsigned int)(index) * 4) + 2;
                ebo_chunk_as_ebo[(index * 6) + 5] = ((unsigned int)(index) * 4) + 3;
                ebo_chunk_as_ebo[(index * 6) + 0] = (unsigned int)(index) * 4;
            }
            memoryManager->chunkAcessMutex.lock();
            ebo_chunk           = memoryManager->GetMemoryChunk(ebo_chunk_pos);
            ebo_chunk->usedSize = ebo_used_size;
            batch.memoryChunksEBO.push_back(ebo_chunk_pos);

            // Overwrite VBO with vertex information
            // [PosX, PosY, UVx, UVy, r,g,b,a] => 8 floats per vertex
            // 4 vertices per Sprite => 128 bytes per sprite
            vbo_chunk = memoryManager->GetMemoryChunk(chunk_pos);

            size_t vbo_used_size = (vbo_chunk->startIndex + alignof(float) - 1) & (~(alignof(float) - 1) - vbo_chunk->startIndex);
            size_t vbo_max_size  = vbo_chunk->size;
            memoryManager->chunkAcessMutex.unlock();

            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                if (vbo_used_size + (32 * sizeof(float)) > vbo_max_size)
                {
                    LOGGER->Log(utils::IENIUM_ERROR, "Cannot Draw Sprite. Not enough memory in vbo chunk!\n" + std::to_string(vbo_used_size) + "/" +
                                                             std::to_string(vbo_max_size) + "\n" + std::to_string(vbo_chunk->usedSize) + "/" +
                                                             std::to_string(vbo_chunk->size));
                    break;
                }
                vbo_used_size += 32 * sizeof(float);

                // Get Render command from allocated memory
                SpriteRenderCommand command = vbo_as_render_command[index];

                vbo_chunk_as_vbo[(index * 32) + 0]  = command.position.X() - (command.size.X() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 1]  = command.position.Y() - (command.size.Y() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 2]  = command.textureOffset.X() - ((command.size.X() * command.textureScale.X()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 3]  = command.textureOffset.Y() - ((command.size.Y() * command.textureScale.Y()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 4]  = command.tint.r();
                vbo_chunk_as_vbo[(index * 32) + 5]  = command.tint.g();
                vbo_chunk_as_vbo[(index * 32) + 6]  = command.tint.b();
                vbo_chunk_as_vbo[(index * 32) + 7]  = command.tint.a();
                vbo_chunk_as_vbo[(index * 32) + 8]  = command.position.X() + (command.size.X() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 9]  = command.position.Y() - (command.size.Y() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 10] = command.textureOffset.X() + ((command.size.X() * command.textureScale.X()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 11] = command.textureOffset.Y() - ((command.size.Y() * command.textureScale.Y()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 12] = command.tint.r();
                vbo_chunk_as_vbo[(index * 32) + 13] = command.tint.g();
                vbo_chunk_as_vbo[(index * 32) + 14] = command.tint.b();
                vbo_chunk_as_vbo[(index * 32) + 15] = command.tint.a();
                vbo_chunk_as_vbo[(index * 32) + 16] = command.position.X() + (command.size.X() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 17] = command.position.Y() + (command.size.Y() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 18] = command.textureOffset.X() + ((command.size.X() * command.textureScale.X()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 19] = command.textureOffset.Y() + ((command.size.Y() * command.textureScale.Y()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 20] = command.tint.r();
                vbo_chunk_as_vbo[(index * 32) + 21] = command.tint.g();
                vbo_chunk_as_vbo[(index * 32) + 22] = command.tint.b();
                vbo_chunk_as_vbo[(index * 32) + 23] = command.tint.a();
                vbo_chunk_as_vbo[(index * 32) + 24] = command.position.X() - (command.size.X() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 25] = command.position.Y() + (command.size.Y() / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 26] = command.textureOffset.X() - ((command.size.X() * command.textureScale.X()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 27] = command.textureOffset.Y() + ((command.size.Y() * command.textureScale.Y()) / 2.F);
                vbo_chunk_as_vbo[(index * 32) + 28] = command.tint.r();
                vbo_chunk_as_vbo[(index * 32) + 29] = command.tint.g();
                vbo_chunk_as_vbo[(index * 32) + 30] = command.tint.b();
                vbo_chunk_as_vbo[(index * 32) + 31] = command.tint.a();
            }
            memoryManager->chunkAcessMutex.lock();
            vbo_chunk->usedSize = vbo_used_size;
            memoryManager->chunkAcessMutex.unlock();

            batch.lastMemorySize += vbo_used_size;
        }
    }

    void BatchingSystem::DrawSpriteBatch(RenderBatch& batch)
    {
        for (size_t index = 0; index < batch.memoryChunksVBO.size(); index++)
        {
            vertexBufferManager->FillSpriteBuffer(memoryManager->GetMemoryChunk(batch.memoryChunksVBO[index]),
                                                  memoryManager->GetMemoryChunk(batch.memoryChunksEBO[index]));
            vertexBufferManager->DrawSpriteBuffer(memoryManager->GetMemoryChunk(batch.memoryChunksEBO[index])->usedSize / 6);
        }
    }
} // namespace ienium::glow