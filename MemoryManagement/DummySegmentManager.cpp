#include "DummySegmentManager.hpp"

DummySegmentManager::DummySegmentManager(void*, size_t, bool)
{
}

void* DummySegmentManager::alloc(size_t neededBytes)
{
    return operator new(neededBytes);
}

void DummySegmentManager::free(void* addr)
{
    ::operator delete(addr);
}
