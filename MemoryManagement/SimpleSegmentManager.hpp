#pragma once

#include <mutex>

// TODO _IK_:
// Implement rbtree_best_fit algorithm (best-fit logarithmic-time complexity allocation)
// https://www.boost.org/doc/libs/1_88_0/doc/html/interprocess/memory_algorithms.html#interprocess.memory_algorithms.rbtree_best_fit

/**
 * This class obtains a memory segment for subsequent allocation/deallocation of that memory's
 * fragments on user request. This class implements simple variation of the sequential fit algorithm,
 * hence the name:
 * https://www.boost.org/doc/libs/1_88_0/doc/html/interprocess/memory_algorithms.html#interprocess.memory_algorithms.simple_seq_fit
 * SimpleSegmentManager is not responsible for destruction of the memory it manages.
 */
class SimpleSegmentManager
{
private:
    struct MemControlBlock
    {
        size_t size; // Size in mem units, not bytes. Mem unit size is implementation-defined
        MemControlBlock* next;
    };

public:
    SimpleSegmentManager(char* segment,
                         size_t size,
                         bool verboseDebugging = false);
    ~SimpleSegmentManager() = default;

    SimpleSegmentManager(const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager& operator= (const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager(SimpleSegmentManager&& rhs) = delete;
    SimpleSegmentManager& operator= (SimpleSegmentManager&& rhs) = delete;

    void* alloc(size_t neededBytes);
    void free(void* addr);

private:
    void initStatData();
    void incrStatData(size_t neededBytes, size_t neededUnitsWithCb);
    void decrStatData(void *addr, size_t freedUnits);

    void printFreeList() const;

    char* mSegment;
    size_t mSegmentSize; // bytes

    bool mVerboseDebug;

    mutable std::mutex mMutex;
    MemControlBlock* mFreeListHeader; // linked list of free mem fragments

    // Statistics data:
    size_t mFreeUnits;
    size_t mOccupUnits;

    // Minimum required size of a fragment that can be allocated to user:
    static const size_t MIN_USABLE_FRAGMENT_SZ;
    // MIN_USABLE_FRAGMENT_SZ along with its control block:
    static const size_t MIN_USABLE_FRAGMENT_CB_SZ;
    static const size_t MIN_SEGMENT_SZ;
};
