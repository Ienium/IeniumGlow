#pragma once

#include <cstddef>
#include <mutex>
#include <vector>

namespace ienium::glow
{
    struct MemoryChunkInfo
    {
        int poolType;
        size_t poolIndex;

        MemoryChunkInfo (int pool_type, size_t pool_index)
            : poolType (pool_type), poolIndex (pool_index) {}
    };

    struct MemoryChunk
    {
        void* data;
        size_t currentSize = 0;
        size_t maxSize = 0;
        size_t poolIndex = -1;
        int poolType;
        bool isUsed = false;
    };

    class RenderMemoryManager
    {
        public:        
        void DefineChunkSizes (size_t tiny_size, size_t small_size, size_t medium_size, size_t large_size);
        void DefinePoolSizes (size_t tiny_size, size_t small_size, size_t medium_size, size_t large_size);
        void InitializePools ();

        MemoryChunkInfo RequestMemoryChunk (size_t required_size);
        MemoryChunkInfo RequestInitialMemoryChunk ();

        MemoryChunk* GetChunk (const MemoryChunkInfo& memory_chunk_info);

        void ReleaseChunk (const MemoryChunkInfo& memory_chunk);

        void ResetAllPools ();

        void LogStats ();

        void Shutdown ();


        private:
        struct MemoryPool
        {
            int poolType;
            std::vector<MemoryChunk> chunks;
            size_t chunkSize;
            std::vector<size_t> freeChunks;
            std::mutex poolMutex;
        };

        enum PoolType { TINY, SMALL, MEDIUM, LARGE, POOL_COUNT };

        MemoryPool pools[POOL_COUNT];

        size_t chunkSizes[POOL_COUNT] = {
            1024,    // 1KB
            16384,   // 16KB  
            262144,  // 256KB
            2097152  // 2MB
        };

        size_t poolSizes[POOL_COUNT] = {
            50,
            100,
            50,
            20
        };

        void CreatePools ();
        void AddChunks (MemoryPool* pool, size_t chunk_count);
        void AllocateMemoryPools ();
        void DeletePools ();

        PoolType FindBestPoolType (size_t required_size) const;
        MemoryChunk* AllocateFromPool (PoolType poolType);
    };
}