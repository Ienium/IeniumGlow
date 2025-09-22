#pragma once

#include <cstddef>
#include <mutex>
#include <vector>

namespace ienium::glow
{
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

        MemoryChunk* RequestMemoryChunk (size_t required_size);
        MemoryChunk* RequestInitialMemoryChunk ();

        void ReleaseChunk (MemoryChunk* memory_chunk);

        void ResetAllPools ();

        void Shutdown ();


        private:
        struct MemoryPool
        {
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
        void AllocateMemoryPools ();
        void DeletePools ();

        PoolType FindBestPoolType (size_t required_size);
        MemoryChunk* AllocateFromPool (PoolType poolType);
    };
}