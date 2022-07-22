#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: FreeListAllocator(256)
{}

FreeListAllocator::FreeListAllocator(const size_t size)
	: pFreeBlocks{}
	, Size{}
	, Capacity{ size }
{
	pFreeBlocks = static_cast<Block*>(malloc(size));

	pFreeBlocks->pNext = nullptr;
	pFreeBlocks->Size = size;
}

FreeListAllocator::~FreeListAllocator()
{
	free(pFreeBlocks);
}
