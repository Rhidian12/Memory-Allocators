#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: FreeListAllocator(256)
{}

FreeListAllocator::FreeListAllocator(const size_t size)
	: pStart{}
	, pFreeBlocks{}
	, Size{}
	, Capacity{ size }
{
	pStart = malloc(size);
	pFreeBlocks = static_cast<Block*>(pStart);

	pFreeBlocks->pNext = nullptr;
	pFreeBlocks->Size = size;
}

FreeListAllocator::~FreeListAllocator()
{
	free(pStart);
}
