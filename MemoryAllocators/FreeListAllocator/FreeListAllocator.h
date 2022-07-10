#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */

class FreeListAllocator
{
public:
	explicit FreeListAllocator();
	explicit FreeListAllocator(const size_t size);

	FreeListAllocator(const FreeListAllocator&) noexcept = delete;
	FreeListAllocator(FreeListAllocator&& other) noexcept;
	FreeListAllocator& operator=(const FreeListAllocator& ) noexcept = delete;
	FreeListAllocator& operator=(FreeListAllocator&& other) noexcept;

private:
	void* pStart;
	size_t TotalSize;
	size_t UsedSize;
};