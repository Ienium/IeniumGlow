#include "ienium/glow/memorymanager.hpp"

#include <cstddef>
#include <mutex>
#include <print>
#include <thread>

#include "ienium/glow/core/internaldefinitions.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"

namespace ienium
{
    using namespace utils;
    size_t MemoryManager::allocatedMemory = 0;

    void MemoryManager::CheckMemoryLeak()
    {
        if (allocatedMemory > 0)
        {
            std::println("Not all MemoryManagers shutdown! {} bytes are still allocated!", allocatedMemory);
        }
    }

    MemoryManagerStats MemoryManager::GetStats()
    {
        MemoryManagerStats stats;
        stats.allocatedMemory         = allocatedMemory;
        stats.mainBlockStats.size     = memoryBlock.size;
        stats.mainBlockStats.usedSize = memoryBlock.usedSize;

        for (auto& backup_block : backupBlocks)
        {
            MemoryBlockStats block_stats{};
            block_stats.size     = backup_block.size;
            block_stats.usedSize = backup_block.usedSize;
            stats.backupBlockStats.push_back(block_stats);
        }

        return stats;
    }

    void MemoryManager::Initialize(size_t initial_size)
    {
        memoryBlock.data = operator new(initial_size);
        memoryBlock.size = initial_size;
        allocatedMemory += initial_size;
    }

    void MemoryManager::Shutdown()
    {
        operator delete(memoryBlock.data);
        allocatedMemory -= memoryBlock.size;

        for (auto& backup_block : backupBlocks)
        {
            operator delete(backup_block.data);
            allocatedMemory -= backup_block.size;
        }

        backupBlocks.clear();
    }

    MemoryChunkPos MemoryManager::RequestMemoryChunk(size_t required_size, size_t align)
    {
        required_size += align;
        MemoryBlock* block_to_use = nullptr;
        size_t       block_index  = -1;
        if (required_size == INVALID_SIZE)
            required_size = DEFAULT_SIZE;

        if (memoryBlock.usedSize + required_size > memoryBlock.size)
        {
            block_index = 0;
            for (auto& backup_block : backupBlocks)
            {
                if (backup_block.usedSize + required_size <= backup_block.size)
                {
                    block_to_use = &backup_block;
                    break;
                }
                block_index++;
            }

            if (block_to_use == nullptr)
            {
                LOGGER->Log(IENIUM_WARNING, "Out of Memory, allocating new block!");
                AddBlock(10 * required_size);
                block_to_use = &backupBlocks.back();
            }
        }
        else
        {
            block_to_use = &memoryBlock;
        }

        MemoryChunk& chunk      = block_to_use->chunks.emplace_back();
        chunk.startIndex        = block_to_use->usedSize;
        chunk.alignedStartIndex = (chunk.startIndex + align - 1) & ~(align - 1);
        chunk.size              = required_size;
        chunk.blockData         = block_to_use->data;
        chunk.usedSize          = chunk.alignedStartIndex - chunk.startIndex;

        block_to_use->usedSize += required_size;

        return {.blockIndex = block_index, .chunkIndex = block_to_use->chunks.size() - 1};
    }

    MemoryChunk* MemoryManager::GetMemoryChunk(const MemoryChunkPos& chunk_pos)
    {
        if (chunk_pos.blockIndex == (size_t)-1)
            return &memoryBlock.chunks[chunk_pos.chunkIndex];

        return &backupBlocks[chunk_pos.blockIndex].chunks[chunk_pos.chunkIndex];
    }

    void MemoryManager::ResetMemoryChunks()
    {
        memoryBlock.chunks.clear();
        memoryBlock.usedSize = 0;

        for (auto& backup_block : backupBlocks)
        {
            backup_block.chunks.clear();
            backup_block.usedSize = 0;
        }
    }

    void MemoryManager::AddBlock(size_t required_size)
    {
        MemoryBlock backup_block;
        backup_block.data = operator new(required_size);
        backup_block.size = required_size;
        allocatedMemory += required_size;
        backupBlocks.push_back(backup_block);
    }

    void MemoryManager::ExpandBlock()
    {
        expandCommandsMutex.lock();
        if (expandCommand.active || backupBlocks.empty())
        {
            expandCommandsMutex.unlock();
            return;
        }

        size_t required_memory = memoryBlock.size;

        if (!backupBlocks.empty())
            expandCommand.active = true;

        for (auto& backup_block : backupBlocks)
        {
            required_memory += backup_block.size;
        }
        expandCommand.allocatedMemory = INVALID_SIZE;

        expandCommandsMutex.unlock();

        std::thread expand_thread(AllocateAsync, required_memory, this);

        expand_thread.detach();
    }

    void MemoryManager::FreeUnused()
    {
        expandCommandsMutex.lock();
        if (expandCommand.active && expandCommand.allocatedMemory != INVALID_SIZE)
        {
            size_t required_memory = memoryBlock.size;

            allocationMutex.lock();
            size_t initial_size = backupBlocks.size();
            for (size_t i = 0; i < initial_size; i++)
            {
                auto& block = backupBlocks[0];
                required_memory += block.size;
                if (required_memory <= expandCommand.allocatedMemory)
                {
                    operator delete(block.data);
                    allocatedMemory -= block.size;
                    backupBlocks.erase(backupBlocks.begin());
                }
                else
                    break;
            }

            allocationMutex.unlock();

            operator delete(memoryBlock.data);
            allocatedMemory -= memoryBlock.size;

            memoryBlock.data = expandCommand.data;
            memoryBlock.size = expandCommand.allocatedMemory;

            expandCommand.allocatedMemory = INVALID_SIZE;
            expandCommand.active          = false;
        }
        expandCommandsMutex.unlock();
    }

    void MemoryManager::AllocateAsync(size_t required_size, MemoryManager* memory_manager)
    {
        memory_manager->expandCommandsMutex.lock();
        memory_manager->expandCommand.data            = operator new(required_size);
        memory_manager->expandCommand.allocatedMemory = required_size;
        memory_manager->expandCommandsMutex.unlock();

        memory_manager->allocationMutex.lock();
        allocatedMemory += required_size;
        memory_manager->allocationMutex.unlock();
        LOGGER->Log(utils::IENIUM_INFO, "New Block allocated: " + std::to_string(required_size) + "bytes");
    }
} // namespace ienium