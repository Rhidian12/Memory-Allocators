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

FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
	: pStart{ std::move(other.pStart) }
	, pFreeBlocks{ std::move(other.pFreeBlocks) }
	, Size{ std::move(other.Size) }
	, Capacity{ std::move(other.Capacity) }
{
	other.pStart = nullptr;
	other.pFreeBlocks = nullptr;
}

FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& other) noexcept
{
	pStart = std::move(other.pStart);
	pFreeBlocks = std::move(other.pFreeBlocks);
	Size = std::move(other.Size);
	Capacity = std::move(other.Capacity);

	other.pStart = nullptr;
	other.pFreeBlocks = nullptr;

	return *this;
}
