#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */

class FreeListAllocator
{
public:
	explicit FreeListAllocator();
	explicit FreeListAllocator(const size_t size);

private:
	void* pStart;
	size_t TotalSize;
	size_t UsedSize;
};