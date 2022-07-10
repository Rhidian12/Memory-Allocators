#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pPools{}
	, AmountOfPools{ sizeof(pPools) / sizeof(Block) }
{
	for (size_t i{}; i < AmountOfPools; ++i)
	{
		pPools[i] = new Block[16];
	}
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
