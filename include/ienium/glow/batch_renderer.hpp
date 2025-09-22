#pragma once

#include "ienium/glow/core/publictypes.hpp"
#include "ienium/glow/vertexbuffermanager.hpp"
#include "memorymanager.hpp"
#include "rendercommand.hpp"

#include <print>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>


namespace ienium::glow
{
    static const size_t INVALID_MEMORY_SIZE = size_t(-1);

    struct MemoryChunk;

    enum RenderType {
        SPRITE,
        TRIANGLE,
        RECTANGLE,
        CIRCLE,
        LINE
    };

    struct BatchKey
    {

    };

    struct RenderBatch
    {
        int layer;
        ResourceId textureId;
        RenderType type;

        std::vector<MemoryChunk*> memoryChunksVBO;
        std::vector<MemoryChunk*> memoryChunksEBO;

        size_t draw_count = 0;
        size_t lastMemorySize = -1;

        RenderBatch (int layer, ResourceId texture_id, RenderType type) : 
        layer (layer), textureId (texture_id), type (type) {}
    };

    class BatchingSystem
    {
        public:
        void BeginFrame ();
        void EndFrame ();

        BatchingSystem (VertexBufferManager& vertex_buffer_manager, RenderMemoryManager& memory_manager) : vertexBufferManager(vertex_buffer_manager), memoryManager(memory_manager) {};

        void ResetAllBatches ();
        RenderBatch& RequestBatch (int layer, ResourceId texture_id, RenderType type);

        template <typename T, typename... Args>
        requires (std::is_base_of_v<RenderCommand, T>)
        void AddRenderCommand (RenderBatch& batch, Args&&... args)
        {
            if (batch.memoryChunksVBO.empty () || batch.memoryChunksVBO.back ()->currentSize + ELEMENT_SIZE_MAP.at (SPRITE) > batch.memoryChunksVBO.back ()->maxSize)
            {
                batch.memoryChunksVBO.push_back (memoryManager.RequestInitialMemoryChunk ());
            }

            auto memory = static_cast<T*> (batch.memoryChunksVBO.back ()->data);
            memory [batch.draw_count] = T{std::forward<Args>(args)...};
            batch.draw_count ++;

            batch.memoryChunksVBO.back ()->currentSize += ELEMENT_SIZE_MAP.at (SPRITE);
        }

        void AddSpriteRenderCommand (RenderBatch& batch, const Vector2& size, const Vector2& position, ResourceId texture_id, const utils::Color& tint, const Vector2& texture_scale, const Vector2& texture_offset, float angle);

        private:
        std::unordered_map<size_t, RenderBatch> batches = std::unordered_map<size_t, RenderBatch>();
        std::vector<SpriteRenderCommand> spriteRenderCommands;
        VertexBufferManager& vertexBufferManager;
        RenderMemoryManager& memoryManager;

        std::unordered_map<RenderType, size_t> ELEMENT_SIZE_MAP
        {
            {SPRITE, 128},      // (2x4 position, 4x4 color, 2x4 uvs) x 4 vertices
            {TRIANGLE, 72},     // (2x4 position, 4x4 color) x 3 vertices
            {RECTANGLE, 96}     // (2x4 position, 4x4 color) x 4 vertices
        };

        std::unordered_map<RenderType, size_t> COMMAND_SIZE_MAP
        {
            {SPRITE, 80}        // (2x8 size, 2x8 position, 3x4 tint, 2x8 texture_scale, 2x8 texture_offset, 1x4 angle) = 80 byte
        };


        int currentLayer;

        void ProcessBatches ();
        void DrawBatches ();

        void ProcessSpriteBatch (RenderBatch& batch);
        void DrawSpriteBatch (RenderBatch& batch);
        //void SortBatches ();

        size_t CalculateHash (int layer, ResourceId texture_id, RenderType type);
    };

    bool operator<(const RenderBatch& a, const RenderBatch& b);
}