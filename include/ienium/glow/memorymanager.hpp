#pragma once

#include <cstddef>
#include <exception>
#include <mutex>
#include <vector>

#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

namespace ienium
{
    constexpr size_t INVALID_SIZE = -1;
    constexpr size_t DEFAULT_SIZE = 131072; // 512 * 256 byte

    struct MemoryChunkPos
    {
        size_t blockIndex;
        size_t chunkIndex;
    };

    struct MemoryChunk
    {
        size_t startIndex;
        size_t alignedStartIndex;
        size_t size;
        size_t usedSize;

        void* blockData;

        template <typename T>
        inline T* GetFirstValue()
        {
            return reinterpret_cast<T*>(static_cast<char*>(blockData) + alignedStartIndex);
        }

        template <typename T>
        inline void WriteMemory(const T& value, size_t index, T* memory)
        {
            usedSize += sizeof(T);
            if (usedSize > size)
            {
                LOGGER->Log(utils::IENIUM_ERROR,
                            "Trying to write memory block out of bounds for chunk!\n" + std::to_string(usedSize) + "/" + std::to_string(size));
                usedSize -= sizeof(T);
                throw(std::exception());
                return;
            }
            memory[index] = value;
        }
    };

    class MemoryBlock
    {
        friend class MemoryManager;

      public:
      private:
        size_t                   size;
        void*                    data;
        std::vector<MemoryChunk> chunks;

        size_t usedSize = 0;
    };

    struct MemoryBlockStats
    {
        size_t size;
        size_t usedSize;
    };

    struct MemoryManagerStats
    {
        size_t                        allocatedMemory;
        MemoryBlockStats              mainBlockStats;
        std::vector<MemoryBlockStats> backupBlockStats;
    };

    class MemoryManager
    {
        struct ExpandCommand
        {
            bool   active = false;
            void*  data{};
            size_t allocatedMemory = INVALID_SIZE;
        };

      public:
        void        Initialize(size_t initial_size);
        void        Shutdown();
        static void CheckMemoryLeak();

        void ExpandBlock();
        void FreeUnused();

        MemoryManagerStats GetStats();

        MemoryChunkPos RequestMemoryChunk(size_t required_size, size_t align);
        MemoryChunk*   GetMemoryChunk(const MemoryChunkPos& chunk_pos);
        void           ResetMemoryChunks();

        std::mutex chunkAcessMutex;

      private:
        static size_t            allocatedMemory;
        MemoryBlock              memoryBlock;
        std::mutex               expandCommandsMutex;
        std::mutex               allocationMutex;
        ExpandCommand            expandCommand;
        std::vector<MemoryBlock> backupBlocks;

        void AddBlock(size_t required_size);

        static void AllocateAsync(size_t required_size, MemoryManager* memory_manager);
    };
} // namespace ienium