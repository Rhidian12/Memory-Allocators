#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */
#include <stdexcept> /* std::invalid_argument */
#include <assert.h> /* assert() */

#include "../Utils/Utils.h"

struct Block final
{
	size_t Size;
	Block* pNext;
};

struct Header final
{
	size_t Size;
	size_t Adjustment;
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
	void allocate(const size_t nrOfElements, const size_t alignment)
	{
		if (nrOfElements == 0)
		{
			throw std::invalid_argument{ "FreeListAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		Block* pPreviousBlock = nullptr;
		Block* pFreeBlock = pFreeBlocks;

		Block* pPreviousBestFit = nullptr;
		Block* pBestFit = nullptr;

		size_t bestFitAdjustment = 0;
		size_t bestFitTotalSize = 0;

		while (pFreeBlock)
		{
			/* Alignment adjustment needed to store the Header */
			const size_t adjustment{ Utils::AlignForward<Header>(pFreeBlock, alignment) };

			/* Calculate total size */
			const size_t totalSize{ nrOfElements * sizeof(T) + adjustment };

			/* Is the current block a better fit than the current best fit? */
			if (pFreeBlock->Size > totalSize && (!pBestFit || pFreeBlock->Size < pBestFit->Size))
			{
				pPreviousBestFit = pPreviousBlock;
				pBestFit = pFreeBlock;

				bestFitAdjustment = adjustment;
				bestFitTotalSize = totalSize;

				/* Is the new current block a perfect fit? */
				if (pFreeBlock->Size == totalSize)
				{
					break;
				}
			}

			pPreviousBlock = pFreeBlock;
			pFreeBlock = pFreeBlock->pNext;
		}

		if (!pBestFit)
		{
			throw std::bad_alloc{};
		}

		/* Can the best fit be split into two parts? */
		if (pBestFit->Size - bestFitTotalSize > sizeof(Header))
		{
			/* Split the block into memory for the allocation and the remainder */

			/* New block starts at best fit + size */
			Block* const pNewBlock{ reinterpret_cast<Block*>(reinterpret_cast<char*>(pBestFit) + bestFitTotalSize) };

			/* The new block has the remaining size */
			pNewBlock->Size = pBestFit->Size - bestFitTotalSize;
			/* The new block now points to the original block's next */
			pNewBlock->pNext = pBestFit->pNext;

			/* Instert the new block into the list */
			/* By not pointing towards pNewBlock instead of pBestFit, we're removing it from the list */
			if (pPreviousBestFit)
			{
				pPreviousBestFit->pNext = pNewBlock;
			}
			else
			{
				pFreeBlocks = pNewBlock;
			}
		}
		else /* we can't split it into two */
		{
			/* Update the size to the entire best fit block size */
			bestFitTotalSize = pBestFit->Size;

			/* Remove pBestFit from the linked list */
			if (pPreviousBestFit)
			{
				pPreviousBestFit->pNext = pBestFit->pNext;
			}
			else
			{
				pFreeBlocks = pBestFit->pNext;
			}
		}

		/* Get the aligned address */
		const size_t alignedAddress{ reinterpret_cast<char*>(pBestFit) + bestFitAdjustment };

		/* Get the header from this aligned address */
		Header* const pHeader{ reinterpret_cast<Header*>(alignedAddress - sizeof(Header)) };

		pHeader->Size = bestFitTotalSize;
		pHeader->Adjustment = bestFitAdjustment;

		return reinterpret_cast<T*>(alignedAddress);
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
	Block* pFreeBlocks;
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