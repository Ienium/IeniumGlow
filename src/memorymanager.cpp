#include "ienium/glow/memorymanager.hpp"
#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"
#include <cstddef>
#include <new>
#include <string>
#include <vector>

using namespace ienium::utils;

namespace ienium::glow
{
    void RenderMemoryManager::DefineChunkSizes (size_t tiny_size, size_t small_size, size_t medium_size, size_t large_size)
    {
        chunkSizes[0] = tiny_size;
        chunkSizes[1] = small_size;
        chunkSizes[2] = medium_size;
        chunkSizes[3] = large_size;
    }

    void RenderMemoryManager::DefinePoolSizes (size_t tiny_size, size_t small_size, size_t medium_size, size_t large_size)
    {
        poolSizes[0] = tiny_size;
        poolSizes[1] = small_size;
        poolSizes[2] = medium_size;
        poolSizes[3] = large_size;
    }

    void RenderMemoryManager::InitializePools ()
    {
        CreatePools ();
        AllocateMemoryPools ();
    }

    MemoryChunk* RenderMemoryManager::RequestMemoryChunk (size_t required_size)
    {
        auto pool_type = FindBestPoolType (required_size);
        if (pool_type == POOL_COUNT)
        {
            return nullptr;
            LOGGER->Log (utils::IENIUM_ERROR, "Required memory size exceed maximum chunk size.");
        }
            

        return AllocateFromPool (pool_type);
    }

    MemoryChunk* RenderMemoryManager::RequestInitialMemoryChunk ()
    {
        auto pool_type = PoolType::MEDIUM;

        return AllocateFromPool (pool_type);
    }

    void RenderMemoryManager::ReleaseChunk (MemoryChunk* memory_chunk)
    {
         std::lock_guard<std::mutex> lock(pools[memory_chunk->poolType].poolMutex);
        if (!memory_chunk || !memory_chunk->isUsed)
        {
            LOGGER->Log (utils::IENIUM_WARNING, "Tried to release a nonexistant or already released memory chunk.");
            return;
        }
            

        memory_chunk->currentSize = 0;
        memory_chunk->isUsed = false;
        pools[memory_chunk->poolType].freeChunks.push_back (memory_chunk->poolIndex);
    }

    void RenderMemoryManager::ResetAllPools ()
    {
        for (int i = 0; i < POOL_COUNT; ++i)
        {
            std::lock_guard<std::mutex> lock(pools[i].poolMutex);
            auto pool = &pools[i];

            pool->freeChunks.clear ();
            
            for (auto& chunk : pool->chunks)
            {
                chunk.isUsed = false;
                chunk.currentSize = 0;
                pool->freeChunks.push_back (chunk.poolIndex);
            }
        }
    }

    void RenderMemoryManager::LogStats ()
    {
        int used_pools [4];
        int counter = 0;

        std::vector<MemoryChunk*> used_chunks;
        for (auto& pool : pools)
        {
            used_pools [counter] = 0;
            for (auto& chunk : pool.chunks)
            {
                if (chunk.isUsed)
                {
                    used_pools [counter] ++;
                    used_chunks.push_back (&chunk);
                }
                
            }

            counter ++;
        }

        std::string msg = "Memory chunks in use:\t" + std::to_string (used_pools[0]) + " | " + std::to_string (used_pools[1]) + " | " + std::to_string (used_pools[2]) + " | " + std::to_string (used_pools[3]) + " | ";
        for (auto chunk : used_chunks)
        {
            msg.append ("\nMax Size:\t" + std::to_string(chunk->maxSize) + "\tCurrent Size:\t" + std::to_string(chunk->currentSize));
        }
        
        LOGGER->Log(utils::IENIUM_INFO, msg);
    }

    void RenderMemoryManager::Shutdown ()
    {
        DeletePools ();
    }

    void RenderMemoryManager::CreatePools ()
    {
        for (int i = 0; i < POOL_COUNT; ++i)
        {
            std::lock_guard<std::mutex> lock(pools[i].poolMutex);
            auto& pool = pools[i];
            pool.chunkSize = chunkSizes[i];
            pool.chunks = std::vector<MemoryChunk> (poolSizes[i]);
            
            for (size_t j = 0; j < poolSizes[i]; ++j)
            {
                pool.chunks[j].poolIndex = j;
                pool.freeChunks.push_back (j);
                pool.chunks[j].isUsed = false;
                pool.chunks[j].maxSize = chunkSizes[PoolType(i)];
                pool.chunks[j].poolType = PoolType(i);
            }
        }
    }

    void RenderMemoryManager::AllocateMemoryPools ()
    {
        size_t total_chunk_count = 0;
        size_t total_allocated_memory = 0;
        for (int i = 0; i < POOL_COUNT; ++i)
        {
            std::lock_guard<std::mutex> lock(pools[i].poolMutex);
            auto pool = &pools[i];

            for (auto& chunk : pool->chunks)
            {
                chunk.data = operator new (pool->chunkSize);
                ++total_chunk_count;
                total_allocated_memory += pool->chunkSize;
            }
        }

        LOGGER->Log (utils::IENIUM_INFO, "Allocated memory pools:\n"
            "Tiny pool:\t" + std::to_string(poolSizes[TINY]) + " chunks\t" + std::to_string(chunkSizes[TINY]) + " Bytes =>\t\t" + std::to_string(poolSizes[TINY]*chunkSizes[TINY]) + " Bytes\n"
            "Small pool:\t" + std::to_string(poolSizes[SMALL]) + " chunks\t" + std::to_string(chunkSizes[SMALL]) + " Bytes =>\t\t" + std::to_string(poolSizes[SMALL]*chunkSizes[SMALL]) + " Bytes\n"
            "Medium pool:\t" + std::to_string(poolSizes[MEDIUM]) + " chunks\t" + std::to_string(chunkSizes[MEDIUM]) + " Bytes =>\t\t" + std::to_string(poolSizes[MEDIUM]*chunkSizes[MEDIUM]) + " Bytes\n"
            "Large pool:\t" + std::to_string(poolSizes[LARGE]) + " chunks\t" + std::to_string(chunkSizes[LARGE]) + " Bytes =>\t" + std::to_string(poolSizes[LARGE]*chunkSizes[LARGE]) + " Bytes\n"
            "Total : \t" + std::to_string(total_chunk_count) + " chunks\t\t\t\t" + std::to_string(total_allocated_memory) + " Bytes"
        );
    }

    void RenderMemoryManager::DeletePools ()
    {
        for (int i = 0; i < POOL_COUNT; ++i)
        {
            std::lock_guard<std::mutex> lock(pools[i].poolMutex);
            auto pool = &pools[i];

            for (auto& chunk : pool->chunks)
            {
                operator delete (chunk.data);
            }
        }
    }

    RenderMemoryManager::PoolType RenderMemoryManager::FindBestPoolType (size_t required_size)
    {
        int i = 0;
        for (i = 0; i < POOL_COUNT; ++i)
        {
            if (required_size <= chunkSizes[i])
                break;
        }

        return PoolType(i);
    }

    MemoryChunk* RenderMemoryManager::AllocateFromPool (RenderMemoryManager::PoolType pool_type)
    {
        std::lock_guard<std::mutex> lock(pools[pool_type].poolMutex);
        auto& pool = pools[pool_type];
        if (pool.freeChunks.empty ())
        {
            // TODO: Increase pool size for this pool type for next frame. And use larger available pool for this frame
            // If no larger pool available: Create new chunk on the fly
            // Create on the fly right here, pool increasing at end of frame (count how many pools are missing over the frame)
            LOGGER->Log (utils::IENIUM_ERROR, "No memory chunks available in requested pool.");
            return nullptr;
        }

        auto selected_chunk = &pool.chunks[pool.freeChunks.back ()];
        pool.freeChunks.pop_back ();
        selected_chunk->isUsed = true;
        //selected_chunk->maxSize = required_size;

        return selected_chunk;
    }
}