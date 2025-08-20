#pragma once

#include <functional>

/**
 * TODO _IK_:
 * Implement rbtree_best_fit algorithm (best-fit logarithmic-time complexity allocation)
 * https://www.boost.org/doc/libs/1_88_0/doc/html/interprocess/memory_algorithms.html#interprocess.memory_algorithms.rbtree_best_fit
 */

using Destroyer = std::function<void(void*,size_t)>;

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
    SimpleSegmentManager(void* segment,
                   size_t size,
                   bool isOwner = false,
                   Destroyer destroyer = {});
    ~SimpleSegmentManager();

    SimpleSegmentManager(const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager& operator= (const SimpleSegmentManager& rhs) = delete;
    SimpleSegmentManager(SimpleSegmentManager&& rhs) = delete;
    SimpleSegmentManager& operator= (SimpleSegmentManager&& rhs) = delete;

    void* alloc(size_t nbytes);
    void free(void* addr);

  private:
    void* mSegment;
    size_t mSegmentSize; // bytes
    bool mIsOwner;
    Destroyer mSegmentDestroyer;
};
