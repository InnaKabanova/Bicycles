#pragma once

#include <functional>
#include <mutex>

/**
 * TODO _IK_:
 * Implement rbtree_best_fit algorithm (best-fit logarithmic-time complexity allocation)
 * https://www.boost.org/doc/libs/1_88_0/doc/html/interprocess/memory_algorithms.html#interprocess.memory_algorithms.rbtree_best_fit
 */

using Destroyer = std::function<void(char*,size_t)>;

struct MemControlBlock
{
    size_t size; // Size in mem units, not bytes (see SimpleSegmentManager's implemetation)
    MemControlBlock* next;
};

/**
 * This class obtains a segment of memory for subsequent allocation/deallocation of that memory's
 * fragments on user request. This class implements simple variation of the sequential fit algorithm,
 * hence the name:
 * https://www.boost.org/doc/libs/1_88_0/doc/html/interprocess/memory_algorithms.html#interprocess.memory_algorithms.simple_seq_fit
 *
 * There's also an option to transfer ownership over the managed memory to this class' object, by
 * setting its ctor's @isOwner param to true and submitting the way how to destruct that memory via
 * the @destroyer param.
 */
class SimpleSegmentManager
{
public:
    SimpleSegmentManager(char* segment,
                         size_t size,
                         bool isOwner = false,
                         Destroyer destroyer = {});
    ~SimpleSegmentManager();

    SimpleSegmentManager(const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager& operator= (const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager(SimpleSegmentManager&& rhs) = delete;
    SimpleSegmentManager& operator= (SimpleSegmentManager&& rhs) = delete;

    void* alloc(size_t neededBytes);
    void free(void* addr);

private:
    char* mSegment;
    size_t mSegmentSize; // bytes
    bool mIsOwner;
    Destroyer mSegmentDestroyer;

    std::mutex mMutex;
    MemControlBlock* mFreeList; // linked list of free mem fragments
};

/**
 * This dummy segment manager does nothing special & is just used for unit testing of @MyAllocator.
 */
class DummySegmentManager
{
public:
    DummySegmentManager(void* segment,
                        size_t size,
                        bool isOwner = false,
                        Destroyer destroyer = {});
    ~DummySegmentManager() = default;

    void* alloc(size_t neededBytes);
    void free(void* addr);
};

