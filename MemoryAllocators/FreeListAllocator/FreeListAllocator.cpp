#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pStart{}
	, TotalSize{ 1024 }
	, UsedSize{}
{
	pStart = malloc(TotalSize);
}
FreeListAllocator::FreeListAllocator(const size_t size)
	: pStart{}
	, TotalSize{ size }
	, UsedSize{}
{
	pStart = malloc(size);
}