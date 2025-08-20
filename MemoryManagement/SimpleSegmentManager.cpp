#include "SimpleSegmentManager.hpp"

SimpleSegmentManager::SimpleSegmentManager(void* segment,
                               size_t size,
                               bool isOwner,
                               Destroyer destroyer) :
    mSegment(segment),
    mSegmentSize(size),
    mIsOwner(isOwner),
    mSegmentDestroyer(destroyer)
{
    // TODO _IK_: implement
}

SimpleSegmentManager::~SimpleSegmentManager()
{
    // TODO _IK_: implement
}

void* SimpleSegmentManager::alloc(size_t nbytes)
{
    // TODO _IK_: implement
    return operator new(nbytes); // for test only
}

void SimpleSegmentManager::free(void* addr)
{
    // TODO _IK_: implement
    ::operator delete(addr); // for test only
}
