#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */
#include <stdexcept> /* std::invalid_argument */
#include <assert.h> /* assert() */

struct Block final
{
	size_t Size;
	Block* pNext;
	char Data[16];
};

class FreeListAllocator final
{
public:
	explicit FreeListAllocator();

	// FreeListAllocator(const FreeListAllocator&) noexcept = delete;
	// FreeListAllocator(FreeListAllocator&& other) noexcept;
	// FreeListAllocator& operator=(const FreeListAllocator&) noexcept = delete;
	// FreeListAllocator& operator=(FreeListAllocator&& other) noexcept;

	template<typename T>
	void Allocate(const size_t nrOfElements)
	{
		if (nrOfElements == 0)
		{
			throw std::invalid_argument{ "FreeListAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		const size_t totalAllocationSize{ sizeof(Block) + nrOfElements * sizeof(T) };
	}

	template<typename T>
	void Deallocate(T* p)
	{
		if (!p)
		{
			throw std::invalid_argument{ "FreeListAllocator::Deallocate() > pointer is null" };
		}

		// Header* pHeader{ reinterpret_cast<Header*>(p - sizeof(Header)) };


	}

private:
	/* https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2 */
	bool IsPowerOfTwo(const size_t val) const { return (val & (val - 1)) == 0; }
	/* https://embeddedartistry.com/blog/2017/02/22/generating-aligned-memory/ */
	template<typename T>
	size_t AlignForward(const T* p, const size_t wantedAlignment) const { return ((p + (wantedAlignment - 1)) & ~(wantedAlignment - 1)); }
	
	uint8_t AmountOfPools;
	Block* pPools[5] =
	{
		nullptr, /* 8 bytes */
		nullptr, /* 16 bytes */
		nullptr, /* 32 bytes */
		nullptr, /* 64 bytes */
		nullptr /* > 64 bytes */
	};
	uint32_t PoolSizes[5] =
	{
		0, /* 8 bytes */
		0, /* 16 bytes */
		0, /* 32 bytes */
		0, /* 64 bytes */
		0 /* > 64 bytes */
	};
};

//template<typename T>
//T* Allocate(const size_t nrOfElements, const size_t alignment = alignof(T))
//{
//	if (alignment == 0 || nrOfElements == 0)
//	{
//		throw std::invalid_argument{ "FreeListAllocator::Allocate() > Alignment or Nr of Elements to be allocated was 0" };
//	}
//
//	if (!IsPowerOfTwo(alignment))
//	{
//		throw std::invalid_argument{ "FreeListAllocator::Allocate() > Alignment must be a power of 2" };
//	}
//
//	Block* pPreviousBlock{};
//	Block* pFreeBlock{ pFreeBlocks };
//
//	Block* pPreviousBestFit{};
//	Block* pBestFit{};
//
//	size_t bestFitAdjustment{};
//	size_t bestFitTotalSize{};
//
//	/* Find the best fitting block of memory */
//	while (pFreeBlock != nullptr)
//	{
//		const size_t adjustment{ AlignForward(pFreeBlock, alignment + sizeof(Header)) };
//		const size_t totalSizeToAllocate{ nrOfElements * sizeof(T) + adjustment };
//
//		/* Is the current block a better fit than the current best fit? */
//		if (pFreeBlock->Size > totalSizeToAllocate && (pBestFit == nullptr || pFreeBlock->Size < pBestFit->Size))
//		{
//			pPreviousBestFit = pBestFit;
//			pBestFit = pFreeBlock;
//			bestFitAdjustment = adjustment;
//			bestFitTotalSize = totalSizeToAllocate;
//
//			/* If the block fits perfectly we can exit early */
//			if (pFreeBlock->Size == totalSizeToAllocate)
//			{
//				break;
//			}
//		}
//
//		pPreviousBlock = pFreeBlock;
//		pFreeBlock = pFreeBlock->pNext;
//	}
//
//	if (!pBestFit)
//	{
//		/* no suitable block was found */
//		/* [CRINGE]: Should just reallocate tbh */
//		throw std::bad_alloc{};
//	}
//
//	if (pBestFit->Size - bestFitTotalSize <= sizeof(Header))
//	{
//		/* The best fit block cannot be split into two, since the remaining size should be sizeof(Header) + at least 1 byte */
//
//		/* Adjust the size of this allocation to the entirety of the block */
//		bestFitTotalSize = pBestFit->Size;
//
//		/* Remove the block from the list */
//		if (pPreviousBestFit)
//		{
//			pPreviousBestFit->pNext = pBestFit->pNext;
//		}
//		else
//		{
//			pFreeBlock = pBestFit->pNext;
//		}
//	}
//	else
//	{
//		/* Split the best fit block into two parts, one needed for the allocation and the remainder */
//
//		/* New block starts at pBestFit + totalSize */
//		Block* pNewBlock{ reinterpret_cast<Block*>(pBestFit + bestFitTotalSize) };
//
//		/* It has the remaining size */
//		pNewBlock->Size = pBestFit->Size - bestFitTotalSize;
//
//		pNewBlock->pNext = pBestFit->pNext;
//
//		if (pPreviousBestFit)
//		{
//			pPreviousBestFit->pNext = pNewBlock;
//		}
//		else
//		{
//			pFreeBlock = pNewBlock;
//		}
//	}
//
//	/* Get the address */
//	size_t allocatedAddress{ reinterpret_cast<size_t>(pBestFit) + bestFitAdjustment };
//
//	/* Get the Header */
//	Header* pHeader{ reinterpret_cast<Header*>(allocatedAddress - sizeof(Header)) };
//
//	pHeader->Size = bestFitTotalSize;
//	pHeader->Adjustment = bestFitAdjustment;
//
//	UsedSize += bestFitTotalSize;
//
//	return reinterpret_cast<T*>(allocatedAddress);
//}