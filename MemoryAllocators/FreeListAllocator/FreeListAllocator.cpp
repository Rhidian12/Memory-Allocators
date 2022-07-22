#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pFreeBlocks{}
{
	const size_t size{ 256 };
	pFreeBlocks = static_cast<Block*>(malloc(size));
}