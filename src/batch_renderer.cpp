#include <cstddef>
#include <cstdint>
#include <functional>
#include <print>
#include <string>

#include "ienium/glow/batch_renderer.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/glow/core/publictypes.hpp"
#include "ienium/glow/rendercommand.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

namespace ienium::glow
{
    void BatchingSystem::BeginFrame ()
    {
        ResetAllBatches ();

    }

    void BatchingSystem::EndFrame ()
    {
        ProcessBatches ();
        //SortBatches ();
        DrawBatches ();
    }

    void BatchingSystem::ResetAllBatches ()
    {
        for (auto& batch_pair : batches)
        {
            auto& batch = batch_pair.second;
            batch.memoryChunksVBO.clear ();
            batch.memoryChunksEBO.clear ();
            batch.draw_count = 0;
            batch.next_index = 0;
        }
    }

    RenderBatch& BatchingSystem::RequestBatch (int layer, ResourceId texture_id, RenderType type)
    {
        size_t hash = CalculateHash (layer, texture_id, type);

        // Create new batch if needed
        if (!batches.contains (hash))
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
            auto batch = RenderBatch (layer, texture_id, type, align);
            batches.emplace(hash, batch);
        }

        auto& batch = batches.at (hash);

        // Request memory for batch from prediction
        if (batch.memoryChunksVBO.empty () && batch.lastMemorySize != INVALID_MEMORY_SIZE)
            batch.memoryChunksVBO.push_back (memoryManager->RequestMemoryChunk(batch.lastMemorySize * 1.25f, batch.vboAlign));

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

    void BatchingSystem::LogStats ()
    {
        std::string msg = "Batches:\t" + std::to_string (batches.size());
        for (auto& batch_pair : batches)
        {
            msg.append ("\nHash: " + std::to_string(batch_pair.first) + "\tChunk count: " + std::to_string (batch_pair.second.memoryChunksVBO.size()));
            for (auto& chunk_pos : batch_pair.second.memoryChunksVBO)
            {
                auto chunk = memoryManager->GetMemoryChunk(chunk_pos);
                msg.append ("\n\tUsage: " + std::to_string (chunk->usedSize) + " / " + std::to_string (chunk->size));
            }
        }

        LOGGER->Log(utils::IENIUM_DEBUG, msg);
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

        //memoryManager.LogStats();
        //LogStats();
        int counter = -1;
        for (auto chunk_pos : batch.memoryChunksVBO)
        {
            auto memory_chunk = memoryManager->GetMemoryChunk(chunk_pos);
            //LOGGER->Log(utils::IENIUM_DEBUG, std::to_string(counter) + "/49");
            //memoryManager.LogStats();
            //LogStats();
            counter++;

            auto memory_as_render_command = memory_chunk->GetFirstValue<SpriteRenderCommand>();

            // Get Target overwrite for allocated memory
            auto memory_as_vbo = reinterpret_cast<float*>(static_cast<char*>(memory_chunk->blockData) + memory_chunk->startIndex);//static_cast<float*>(memory_chunk->blockData);

            size_t draw_count_in_chunk = memory_chunk->usedSize / ELEMENT_SIZE_MAP.at (SPRITE);
            size_t required_ebo_size = 6 * draw_count_in_chunk * sizeof(unsigned int);
            
            // Request memory for EBO
            MemoryChunkPos ebo_chunk_pos = memoryManager->RequestMemoryChunk (required_ebo_size, alignof(uint32_t));
            auto ebo_chunk = memoryManager->GetMemoryChunk(ebo_chunk_pos);
            //memoryManager.LogStats();
            //LogStats();
            
            auto ebo_chunk_as_ebo = reinterpret_cast<uint32_t*>(static_cast<char*>(ebo_chunk->blockData) + ebo_chunk->startIndex);

            // Fill EBO
            // 6 Elements per sprite => 24 bytes per sprite
            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4 + 1,   index * 6 + 1, ebo_chunk_as_ebo);
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4 + 2,   index * 6 + 2, ebo_chunk_as_ebo);
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4,       index * 6 + 3, ebo_chunk_as_ebo);
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4 + 2,   index * 6 + 4, ebo_chunk_as_ebo);
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4 + 3,   index * 6 + 5, ebo_chunk_as_ebo);
                //ebo_chunk->WriteMemory<uint32_t>((unsigned int)(index) * 4,       index * 6 + 0, ebo_chunk_as_ebo);

                ebo_chunk_as_ebo[index * 6 + 1] = (unsigned int)(index) * 4 + 1;
                ebo_chunk_as_ebo[index * 6 + 2] = (unsigned int)(index) * 4 + 2;
                ebo_chunk_as_ebo[index * 6 + 3] = (unsigned int)(index) * 4;    
                ebo_chunk_as_ebo[index * 6 + 4] = (unsigned int)(index) * 4 + 2;
                ebo_chunk_as_ebo[index * 6 + 5] = (unsigned int)(index) * 4 + 3;
                ebo_chunk_as_ebo[index * 6 + 0] = (unsigned int)(index) * 4;
            }
            ebo_chunk->usedSize = 6 * sizeof(uint32_t) * draw_count_in_chunk; 

            
            batch.memoryChunksEBO.push_back (ebo_chunk_pos);

            // Overwrite VBO with vertex information
            // [PosX, PosY, UVx, UVy, r,g,b,a] => 8 floats per vertex
            // 4 vertices per Sprite => 128 bytes per sprite

            memory_chunk = memoryManager->GetMemoryChunk(chunk_pos);
            memory_chunk->usedSize = 0;
            for (size_t index = 0; index < draw_count_in_chunk; index++)
            {
                // Get Render command from allocated memory
                SpriteRenderCommand command = memory_as_render_command[index];

                // Fill individual vertex data
               // memory_chunk->WriteMemory<float>(command.position.X () - command.size.X () / 2.f,                                       index * 32 + 0,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.position.Y () - command.size.Y () / 2.f,                                       index * 32 + 1,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f,    index * 32 + 2,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f,    index * 32 + 3,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.tint.r (),                                                                     index * 32 + 4,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.tint.g (),                                                                     index * 32 + 5,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.tint.b (),                                                                     index * 32 + 6,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.tint.a (),                                                                     index * 32 + 7,  memory_as_vbo);
 
               // memory_chunk->WriteMemory<float>(command.position.X () + command.size.X () / 2.f,                                       index * 32 + 8,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>(command.position.Y () - command.size.Y () / 2.f,                                       index * 32 + 9,  memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f,   index * 32 + 10, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f,   index * 32 + 11, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.r (),                                                                    index * 32 + 12, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.g (),                                                                    index * 32 + 13, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.b (),                                                                    index * 32 + 14, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.a (),                                                                    index * 32 + 15, memory_as_vbo);

               // memory_chunk->WriteMemory<float>( command.position.X () + command.size.X () / 2.f,                                      index * 32 + 16, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.position.Y () + command.size.Y () / 2.f,                                      index * 32 + 17, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f,   index * 32 + 18, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f,   index * 32 + 19, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.r (),                                                                    index * 32 + 20, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.g (),                                                                    index * 32 + 21, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.b (),                                                                    index * 32 + 22, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.a (),                                                                    index * 32 + 23, memory_as_vbo);

               // memory_chunk->WriteMemory<float>( command.position.X () - command.size.X () / 2.f,                                      index * 32 + 24, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.position.Y () + command.size.Y () / 2.f,                                      index * 32 + 25, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f,   index * 32 + 26, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f,   index * 32 + 27, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.r (),                                                                    index * 32 + 28, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.g (),                                                                    index * 32 + 29, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.b (),                                                                    index * 32 + 30, memory_as_vbo);
               // memory_chunk->WriteMemory<float>( command.tint.a (),                                                                    index * 32 + 31, memory_as_vbo);



                memory_as_vbo[index * 32 + 0]  = command.position.X () - command.size.X () / 2.f;                                   
                memory_as_vbo[index * 32 + 1]  = command.position.Y () - command.size.Y () / 2.f;                                   
                memory_as_vbo[index * 32 + 2]  = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_vbo[index * 32 + 3]  = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_vbo[index * 32 + 4]  = command.tint.r ();                                                                
                memory_as_vbo[index * 32 + 5]  = command.tint.g ();                                                                
                memory_as_vbo[index * 32 + 6]  = command.tint.b ();                                                                
                memory_as_vbo[index * 32 + 7]  = command.tint.a ();                                                                
                memory_as_vbo[index * 32 + 8]  = command.position.X () + command.size.X () / 2.f;                                  
                memory_as_vbo[index * 32 + 9]  = command.position.Y () - command.size.Y () / 2.f;                                  
                memory_as_vbo[index * 32 + 10] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_vbo[index * 32 + 11] = command.textureOffset.Y () - (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_vbo[index * 32 + 12] = command.tint.r ();                                                                 
                memory_as_vbo[index * 32 + 13] = command.tint.g ();                                                                 
                memory_as_vbo[index * 32 + 14] = command.tint.b ();                                                                 
                memory_as_vbo[index * 32 + 15] = command.tint.a ();                                                                 
                memory_as_vbo[index * 32 + 16] = command.position.X () + command.size.X () / 2.f;                                 
                memory_as_vbo[index * 32 + 17] = command.position.Y () + command.size.Y () / 2.f;                                 
                memory_as_vbo[index * 32 + 18] = command.textureOffset.X () + (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_vbo[index * 32 + 19] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_vbo[index * 32 + 20] = command.tint.r ();                                                               
                memory_as_vbo[index * 32 + 21] = command.tint.g ();                                                               
                memory_as_vbo[index * 32 + 22] = command.tint.b ();                                                               
                memory_as_vbo[index * 32 + 23] = command.tint.a ();                                                               
                memory_as_vbo[index * 32 + 24] = command.position.X () - command.size.X () / 2.f;                                 
                memory_as_vbo[index * 32 + 25] = command.position.Y () + command.size.Y () / 2.f;                                 
                memory_as_vbo[index * 32 + 26] = command.textureOffset.X () - (command.size.X () * command.textureScale.X ()) / 2.f;
                memory_as_vbo[index * 32 + 27] = command.textureOffset.Y () + (command.size.Y () * command.textureScale.Y ()) / 2.f;
                memory_as_vbo[index * 32 + 28] = command.tint.r ();                                                              
                memory_as_vbo[index * 32 + 29] = command.tint.g ();                                                              
                memory_as_vbo[index * 32 + 30] = command.tint.b ();                                                              
                memory_as_vbo[index * 32 + 31] = command.tint.a ();                                                              
                
            }
            memory_chunk->usedSize += 32 * sizeof(float) * draw_count_in_chunk;
            batch.lastMemorySize += memory_chunk->usedSize;
        }
    }

    void BatchingSystem::DrawSpriteBatch (RenderBatch& batch)
    {
        for (size_t index = 0; index < batch.memoryChunksVBO.size (); index++)
        {
            vertexBufferManager->FillSpriteBuffer(memoryManager->GetMemoryChunk(batch.memoryChunksVBO[index]), memoryManager->GetMemoryChunk(batch.memoryChunksEBO[index]));
            vertexBufferManager->DrawSpriteBuffer(memoryManager->GetMemoryChunk(batch.memoryChunksEBO[index])->usedSize / 6);
        }
    }
}