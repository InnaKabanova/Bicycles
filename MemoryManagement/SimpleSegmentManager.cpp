#include "SimpleSegmentManager.hpp"

#include <iostream>
#include <cassert>

namespace
{
    const char* V_LOG_TAG = "__SSM__ "; // tag for verbose debugging
}

// All size calculations are made in units equal to MemControlBlock's size.
// This ensures that all allocated memory has the same alignment as MemControlBlock.
const size_t SimpleSegmentManager::MIN_USABLE_FRAGMENT_SZ = sizeof(MemControlBlock);
const size_t SimpleSegmentManager::MIN_USABLE_FRAGMENT_CB_SZ = MIN_USABLE_FRAGMENT_SZ + sizeof(MemControlBlock);
const size_t SimpleSegmentManager::MIN_SEGMENT_SZ =
        sizeof(MemControlBlock) + MIN_USABLE_FRAGMENT_CB_SZ; // free list header + min usable fragment

SimpleSegmentManager::SimpleSegmentManager(char* segment,
                                           size_t size, bool verboseDebugging) :
    mSegment(segment),
    mSegmentSize(size),
    mVerboseDebug(verboseDebugging),
    mMutex(),
    mFreeListHeader(nullptr),
    // Stat data:
    mFreeUnits(0),
    mOccupUnits(0)
{
    assert(mSegment != nullptr && mSegmentSize != 0);
    // Check we have at least the minimum required amount of usable memory:
    if (mSegmentSize < MIN_SEGMENT_SZ)
    {
        throw std::runtime_error("Segment is too small to be used");
    }

    initStatData();
    mFreeListHeader = (MemControlBlock*)mSegment;
    mFreeListHeader->size = mFreeUnits;
    mFreeListHeader->next = mFreeListHeader; // circular linked list
}

void SimpleSegmentManager::printFreeList() const
{
    std::lock_guard<std::mutex> lock(mMutex);

    std::cout << "-------- FREE LIST LAYOUT ---------" << std::endl;
    std::cout << "Header addr: " << reinterpret_cast<long>((void*)mFreeListHeader)
              << ", next free CB: " << reinterpret_cast<long>((void*)mFreeListHeader->next) << std::endl;

    MemControlBlock* currBlock = mFreeListHeader->next;
    while (currBlock != mFreeListHeader) {
        std::cout << "CB addr: " << reinterpret_cast<long>((void*)currBlock)
                  << ", next free CB: " << reinterpret_cast<long>((void*)currBlock->next)
                  << ", next free units: " << currBlock->size << std::endl;
        currBlock = currBlock->next;
    }
    std::cout << "-----------------------------------" << std::endl;
}

void SimpleSegmentManager::initStatData()
{
    mFreeUnits = (mSegmentSize / MIN_USABLE_FRAGMENT_SZ) - 1; // -1 for initial CB as mFreeListHeader
    mOccupUnits = 1;

    if (mVerboseDebug)
    {
        std::cout << V_LOG_TAG << "Segment size: " << mSegmentSize
                               << ", base addr: " << reinterpret_cast<long>((void*)mSegment) << "\n"
                  << V_LOG_TAG << "Unit size: " << MIN_USABLE_FRAGMENT_SZ << " bytes. "
                                  "Min unit size with CB: " << MIN_USABLE_FRAGMENT_CB_SZ << " bytes\n"
                  << V_LOG_TAG << "Free units: " << mFreeUnits << "\n\n";
    }
}

// Called in case of successful allocation
void SimpleSegmentManager::incrStatData(size_t neededBytes, size_t neededUnitsWithCb)
{
    if (mVerboseDebug)
    {
        std::cout << "\n";
        std::cout << V_LOG_TAG << "------- ALLOC BEGIN -------" << std::endl;
        std::cout << V_LOG_TAG << "User requested " << neededBytes << " bytes.\n"
                  << V_LOG_TAG << "Needed units (with 1 unit for CB): " << neededUnitsWithCb
                               << " --> " << neededUnitsWithCb * MIN_USABLE_FRAGMENT_SZ << " bytes.\n"
                  << V_LOG_TAG << "Current free units: " << mFreeUnits
                               << " --> " << mFreeUnits * MIN_USABLE_FRAGMENT_SZ << " bytes.\n"
                  << V_LOG_TAG << "Current occupied units (with CBs): " << mOccupUnits
                               << " --> " << mOccupUnits * MIN_USABLE_FRAGMENT_SZ << " bytes.\n";
    }

    mOccupUnits += neededUnitsWithCb;
    mFreeUnits -= neededUnitsWithCb;

    if (mVerboseDebug)
    {
        std::cout << V_LOG_TAG << "------- ALLOC END --------" << std::endl;
        std::cout << V_LOG_TAG << "Updated free units: " << mFreeUnits << " --> " << mFreeUnits * MIN_USABLE_FRAGMENT_SZ << " bytes.\n"
                  << V_LOG_TAG << "Updated occupied units (with CBs): " << mOccupUnits << std::endl;
        printFreeList();
    }
}

void SimpleSegmentManager::decrStatData(void* addr, size_t freedUnits)
{
    mOccupUnits -= freedUnits;
    mFreeUnits += freedUnits;

    if (mVerboseDebug)
    {
        std::cout << V_LOG_TAG << "------- DEALLOC ----------" << std::endl;
        std::cout << V_LOG_TAG << "Dealloc CB addr: " << (MemControlBlock*)addr - 1 << ", user addr: " << addr << "\n";
        std::cout << V_LOG_TAG << "Updated free units: " << mFreeUnits
                               << " --> free bytes: " << mFreeUnits * MIN_USABLE_FRAGMENT_SZ
                               << ", occupied units (with CBs): " << mOccupUnits << std::endl;
        printFreeList();
        std::cout << std::endl;
    }
}

void* SimpleSegmentManager::alloc(size_t neededBytes)
{
    std::lock_guard<std::mutex> lock(mMutex);

    size_t neededUnitsWithCb = ((neededBytes + MIN_USABLE_FRAGMENT_SZ - 1) / MIN_USABLE_FRAGMENT_SZ) + 1;
    size_t neededUnits = neededUnitsWithCb - 1;
    MemControlBlock* retCb = nullptr;
    void* retAddr = nullptr;

    // Base case: we are starting from scratch, mem segment is completely free
    if (mFreeListHeader->next == mFreeListHeader)
    {
        if (mFreeListHeader->size >= neededUnitsWithCb)
        {
            // Mem block to return to user + its CB right before it:
            retCb = mFreeListHeader + 1;
            retAddr = (void*)(retCb + 1);
            retCb->size = neededUnits;
            // Update free list:
            MemControlBlock* newFreeCb = (MemControlBlock*)((char*)retAddr + neededBytes);
            newFreeCb->size = mFreeUnits - neededUnitsWithCb - 1; // -1 CB of newFreeCb
            newFreeCb->next = mFreeListHeader;
            mFreeListHeader->next = newFreeCb;
            incrStatData(neededBytes, neededUnitsWithCb);
        }
    }
    else
    {
        MemControlBlock* prevCb = mFreeListHeader;
        MemControlBlock* currCb = mFreeListHeader->next;
        while (currCb != mFreeListHeader)
        {
            if (currCb->size >= neededUnits)
            {
                size_t remainingUnits = currCb->size - neededUnits;

                retCb = currCb;
                retAddr = (void*)(retCb + 1);
                retCb->size = neededUnits;

                // Handle partial fit if any
                MemControlBlock* newFreeCb = nullptr;
                if (remainingUnits >= MIN_USABLE_FRAGMENT_CB_SZ)
                {
                    newFreeCb = (MemControlBlock*)((char*)retAddr + neededBytes);
                    newFreeCb->size = remainingUnits - 1; // -1 CB of newFreeCb
                    newFreeCb->next = currCb->next;
                }

                prevCb->next = newFreeCb; // remove from freelist
                incrStatData(neededBytes, neededUnitsWithCb + (remainingUnits >= MIN_USABLE_FRAGMENT_CB_SZ ? 1 : 0)); // +1 CB of newFreeCb
                break; // proceed to returning an address
            }

            prevCb = currCb;
            currCb = currCb->next;
        }
    }

    if (mVerboseDebug)
    {
        if (retAddr != nullptr)
        {
            std::cout << V_LOG_TAG << "Ret CB addr: " << reinterpret_cast<long>((void*)retCb)
                                   << ", ret addr: " << reinterpret_cast<long>(retAddr) << std::endl;
        }
        else
        {
            std::cout << V_LOG_TAG << "Allocation failure for " << neededUnitsWithCb << " units --> "
                                                                << neededBytes << " bytes" << std::endl;
        }
        std::cout << "-----------------------------------\n\n" << std::endl;
    }

    return retAddr;
}

void SimpleSegmentManager::free(void* addr)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (nullptr == addr)
    {
        throw std::invalid_argument("Cannot free a null pointer");
    }

    MemControlBlock* userCb = (MemControlBlock*)addr - 1;

    // Ensure the block belongs to us:
    char* startAddress = mSegment;
    char* endAddress = startAddress + mSegmentSize;
    if ((char*)userCb < startAddress || (char*)userCb >= endAddress) {
        throw std::runtime_error("Invalid ptr: we did not allocate this memory");
    }

    // Check if the block is already freed:
    MemControlBlock* currCb = mFreeListHeader->next;
    while (currCb != mFreeListHeader)
    {
        if (currCb == userCb)
        {
            throw std::runtime_error("Memory block is already freed");
        }
        currCb = currCb->next;
    }

    // Insert the block back into the freelist:
    MemControlBlock* prevCb = mFreeListHeader;
    currCb = mFreeListHeader->next;

    while (currCb != mFreeListHeader)
    {
        // If there is free space AFTER our to-be-freed block, merge with that free space:
        if (currCb == (userCb + sizeof(MemControlBlock) + userCb->size))
        {
            userCb->size += currCb->size + 1;
            userCb->next = currCb->next;
            prevCb->next = userCb;
            decrStatData(addr, userCb->size);
            break;
        }
        // If there is free space BEFORE our to-be-freed block, merge with that free space:
        if ((currCb + sizeof(MemControlBlock) + currCb->size) == userCb)
        {
            currCb->size += userCb->size + 1;
            decrStatData(addr, currCb->size);
            break;
        }

        prevCb = currCb;
        currCb = currCb->next;

        // If we've looped back to the start, insert the block
        if (currCb == mFreeListHeader) {
            userCb->next = currCb->next;
            currCb->next = userCb;
            decrStatData(addr, userCb->size);
            break;
        }
    }
}
