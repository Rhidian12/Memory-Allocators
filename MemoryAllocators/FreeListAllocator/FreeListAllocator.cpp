#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pStart{}
	, TotalSize{ 1024 }
	, UsedSize{}
	, pFreeBlocks{}
{
	pStart = malloc(TotalSize);
	pFreeBlocks = new Block[50]{};
}
FreeListAllocator::FreeListAllocator(const size_t size)
	: pStart{}
	, TotalSize{ size }
	, UsedSize{}
	, pFreeBlocks{}
{
	pStart = malloc(size);
	pFreeBlocks = new Block[size]{};
}

FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
	: pStart{ std::move(other.pStart) }
	, TotalSize{ std::move(other.TotalSize) }
	, UsedSize{ std::move(other.UsedSize) }
{
	other.pStart = nullptr;
}

FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& other) noexcept
{
	pStart = std::move(other.pStart);
	TotalSize = std::move(other.TotalSize);
	UsedSize = std::move(other.UsedSize);

	other.pStart = nullptr;

	return *this;
}
