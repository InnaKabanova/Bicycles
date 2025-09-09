#include "SimpleSegmentManager.hpp"

#include <iostream>
#include <cassert>

namespace
{
    // Size calculations are made in units equal to the size of one MemControlBlock.
    // This ensures that all memory allocated will have the same alignment as MemControlBlock.
    const size_t MIN_USABLE_FRAGMENT = sizeof(MemControlBlock);
    const size_t MIN_USABLE_FRAGMENT_CB = MIN_USABLE_FRAGMENT + sizeof(MemControlBlock);
}

SimpleSegmentManager::SimpleSegmentManager(char* segment,
                                           size_t size) :
    mSegment(segment),
    mSegmentSize(size),
    mMutex(),
    mFreeList(nullptr)
{
    std::cout << "SimpleSegmentManager MIN_USABLE_FRAGMENT: " << MIN_USABLE_FRAGMENT << " bytes" << std::endl;
    std::cout << "SimpleSegmentManager MIN_USABLE_FRAGMENT_CB: " << MIN_USABLE_FRAGMENT_CB << " bytes" << std::endl;

    assert(mSegment != nullptr && mSegmentSize != 0);
    // Check we have at least the minimum amount of usable memory:
    if (mSegmentSize < MIN_USABLE_FRAGMENT_CB)
    {
        throw std::runtime_error("Segment is too small to be used");
    }

    size_t availableUnits = mSegmentSize / sizeof(MemControlBlock);
    mFreeList = (MemControlBlock*)mSegment;
    mFreeList->size = availableUnits - 1; // minus one unit for the initial control block itself
    mFreeList->next = mFreeList; // circular linked list
}

void* SimpleSegmentManager::alloc(size_t neededBytes)
{
    std::lock_guard<std::mutex> lock(mMutex);

    size_t neededUnits = ((neededBytes + sizeof(MemControlBlock) - 1) / sizeof(MemControlBlock)) + 1;
    MemControlBlock* prevBlock = mFreeList;
    MemControlBlock* currBlock = mFreeList->next;

    do
    {
        if (currBlock->size >= neededUnits)
        {
            if (currBlock->size == neededUnits) // Exact fit
            {
                prevBlock->next = currBlock->next; // remove from freelist
            }
            else // Partial fit
            {
                size_t remainingSize = currBlock->size - neededUnits;
                // Update the current block we are allocating:
                currBlock->size = neededUnits;

                if (remainingSize >= MIN_USABLE_FRAGMENT_CB)
                {
                    MemControlBlock* newBlock = currBlock + neededUnits;
                    newBlock->size = remainingSize;
                    newBlock->next = currBlock->next;
                    currBlock->next = newBlock;
                }
                prevBlock->next = currBlock->next; //remove from freelist
            }

            return (void*)(currBlock + 1);
        }

        prevBlock = currBlock;
        currBlock = currBlock->next;
    } while (currBlock != mFreeList);

    return nullptr;
}

void SimpleSegmentManager::free(void* addr)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (nullptr == addr)
    {
        throw std::invalid_argument("Cannot free a null pointer");
    }

    MemControlBlock* blockToFree = (MemControlBlock*)((char*)addr - 1);

    // Ensure the block belongs to us:
    char* startAddress = mSegment;
    char* endAddress = startAddress + mSegmentSize;
    if ((char*)blockToFree < startAddress || (char*)blockToFree >= endAddress)
    {
        throw std::runtime_error("Invalid ptr: we did not allocate this memory");
    }

    // Insert the block back into the freelist:
    MemControlBlock* prevBlock = mFreeList;
    MemControlBlock* currBlock = mFreeList->next;
    while (currBlock != mFreeList)
    {
        if (prevBlock->next == blockToFree)
        {
            throw std::runtime_error("Memory block is already freed");
        }

        if ((char*)currBlock == (char*)blockToFree + (blockToFree->size * sizeof(MemControlBlock)))
        {
            blockToFree->size += currBlock->size;
            blockToFree->next = currBlock->next;
            prevBlock->next = blockToFree;
            return; // Success
        }

        if ((char*)blockToFree == (char*)prevBlock + (prevBlock->size * sizeof(MemControlBlock))) {
            prevBlock->size += blockToFree->size;
            return; // Success
        }

        prevBlock = currBlock;
        currBlock = currBlock->next;
    }
}

DummySegmentManager::DummySegmentManager(void* segment,
                                         size_t size)
{
    (void)segment;
    (void)size;
}

void *DummySegmentManager::alloc(size_t neededBytes)
{
    return operator new(neededBytes);
}

void DummySegmentManager::free(void* addr)
{
    ::operator delete(addr);
}
