#pragma once

#include <cstddef>

/**
 * This dummy segment manager does nothing special & is just used for unit testing of
 * @MyAllocatorOnStack and @MyAllocatorNonOwning.
 */
class DummySegmentManager
{
public:
    DummySegmentManager(void*, size_t, bool);
    ~DummySegmentManager() = default;

    void* alloc(size_t neededBytes);
    void free(void* addr);
};
