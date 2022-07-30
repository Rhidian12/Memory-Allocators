#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: FreeListAllocator(64, 20) /* arbitrary numbers */
{}

FreeListAllocator::FreeListAllocator(const size_t sizeOfBlock, const size_t nrOfBlocks)
	: pBlocks{}
	, pStart{}
	, NrOfBlocks{ nrOfBlocks }
{
	if (nrOfBlocks == 0)
	{
		throw std::invalid_argument{ "Allocator needs more than 0 blocks" };
	}

	pBlocks = static_cast<Block*>(malloc(Utils::AlignForward<Block, Header>(sizeOfBlock, alignof(std::max_align_t))));
	pStart = pBlocks;

	pBlocks->pNext = nullptr;
	pBlocks->Size = sizeOfBlock;

	Block* pBlock{ pBlocks->pNext };
	Block* pPreviousBlock{ pBlocks };

	for (size_t i{ 1 }; i < nrOfBlocks; ++i) /* start at 1 cus we already create a block */
	{
		pBlock = static_cast<Block*>(malloc(Utils::AlignForward(sizeOfBlock, alignof(std::max_align_t))));

		pPreviousBlock->pNext = pBlock;
		pPreviousBlock = pBlock;

		pBlock->Size = sizeOfBlock;
		pBlock->pNext = nullptr;

		pBlock = pBlock->pNext;
	}
}

FreeListAllocator::~FreeListAllocator()
{
	Block* pTemp{};
	Block* pStartBlocks{ static_cast<Block*>(pStart) };

	while (pStartBlocks)
	{
		pTemp = pStartBlocks;
		pStartBlocks = pStartBlocks->pNext;

		free(pTemp);
	}
}

FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
	: pBlocks{ std::move(other.pBlocks) }
	, pStart{ std::move(other.pStart) }
	, NrOfBlocks{std::move(other.NrOfBlocks)}
{
	other.pBlocks = nullptr;
	other.pStart = nullptr;
}

FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& other) noexcept
{
	pBlocks = std::move(other.pBlocks);
	pStart = std::move(other.pStart);
	NrOfBlocks = std::move(other.NrOfBlocks);

	other.pBlocks = nullptr;
	other.pStart = nullptr;

	return *this;
}
