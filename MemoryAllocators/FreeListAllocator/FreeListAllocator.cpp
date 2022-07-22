#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pFreeBlocks{}
	, Size{}
	, Capacity{ 256 }
{
	pFreeBlocks = static_cast<Block*>(malloc(Capacity));
}

FreeListAllocator::FreeListAllocator(const size_t size)
	: pFreeBlocks{}
	, Size{}
	, Capacity{ size }
{
	pFreeBlocks = static_cast<Block*>(malloc(size));
}

FreeListAllocator::~FreeListAllocator()
{
	free(pFreeBlocks);
}
