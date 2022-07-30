#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */
#include <stdexcept> /* std::invalid_argument */
#include <assert.h> /* assert() */
#include <iostream>

#include "../Utils/Utils.h"

/* reference: https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/ */

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

struct NrOfElements final
{
	explicit NrOfElements(const size_t n)
		: NumberOfElements{ n }
	{}

	size_t NumberOfElements;
};

inline NrOfElements operator"" _elem(const size_t n)
{
	return NrOfElements(n);
}

class FreeListAllocator final
{
public:
	explicit FreeListAllocator();
	explicit FreeListAllocator(const size_t sizeOfBlock, const size_t nrOfBlocks);

	~FreeListAllocator();

	FreeListAllocator(const FreeListAllocator&) noexcept = delete;
	FreeListAllocator(FreeListAllocator&& other) noexcept;
	FreeListAllocator& operator=(const FreeListAllocator&) noexcept = delete;
	FreeListAllocator& operator=(FreeListAllocator&& other) noexcept;

	template<typename T>
	T* allocate(const NrOfElements nrOfElements, bool bAllowReallocation = false, const size_t alignment = sizeof(size_t))
	{
		if (nrOfElements.NumberOfElements == 0)
		{
			throw std::invalid_argument{ "FreeListAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		Block* pPreviousBlock = nullptr;
		Block* pFreeBlock = pBlocks;

		Block* pPreviousBestFit = nullptr;
		Block* pBestFit = nullptr;

		size_t bestFitAdjustment = 0;
		size_t bestFitTotalSize = 0;

		while (pFreeBlock)
		{
			/* Alignment adjustment needed to store the Header */
			bestFitAdjustment = Utils::AlignForward<Header, Block>(pFreeBlock, alignment);

			/* Calculate total size */
			bestFitTotalSize = nrOfElements.NumberOfElements * sizeof(T) + bestFitAdjustment;

			/* Is the current block a better fit than the current best fit? */
			if (pFreeBlock->Size >= bestFitTotalSize && (!pBestFit || pFreeBlock->Size < pBestFit->Size))
			{
				pPreviousBestFit = pPreviousBlock;
				pBestFit = pFreeBlock;

				/* Is the new current block a perfect fit? */
				if (pFreeBlock->Size == bestFitTotalSize)
				{
					break;
				}
			}

			pPreviousBlock = pFreeBlock;
			pFreeBlock = pFreeBlock->pNext;
		}

		if (!pBestFit)
		{
			if (bAllowReallocation)
			{
				pBestFit = CreateNewBlocks(bestFitTotalSize, NrOfBlocks * 2, alignment);
			}
			else
			{
				throw std::bad_alloc{};
			}
		}

		/* Can the best fit be split into two parts? */
		if (pBestFit->Size - bestFitTotalSize > sizeof(Header))
		{
			/* Split the block into memory for the allocation and the remainder */

			/* New block starts at best fit + size */
			Block* const pNewBlock{ reinterpret_cast<Block*>(reinterpret_cast<size_t>(pBestFit) + bestFitTotalSize) };

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
				pBlocks = pNewBlock;
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
				pBlocks = pBestFit->pNext;
			}
		}

		/* Get the aligned address */
		const size_t alignedAddress{ reinterpret_cast<size_t>(pBestFit) + bestFitAdjustment };

		/* Get the header from this aligned address */
		Header* const pHeader{ reinterpret_cast<Header*>(alignedAddress - sizeof(Header)) };

		pHeader->Size = bestFitTotalSize;
		pHeader->Adjustment = bestFitAdjustment;

		return reinterpret_cast<T*>(alignedAddress);
	}

	template<typename T>
	void deallocate(void* p)
	{
		if (!p)
		{
			throw std::invalid_argument{ "FreeListAllocator::Deallocate() > pointer is null" };
		}



		/* Get the header from the memory we allocated */
		Header* const pHeader{ reinterpret_cast<Header*>(reinterpret_cast<size_t>(p) - sizeof(Header)) };

		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			/* Get our aligned address we returned in allocate() */
			T* const pTemp{ reinterpret_cast<T*>(reinterpret_cast<size_t>(p) + sizeof(Header) + pHeader->Adjustment) };
			pTemp->~T();
		}

		/* Get the actual start of the allocation by moving backwards the amount specified by the header */
		const size_t blockStart{ reinterpret_cast<size_t>(p) - pHeader->Adjustment };
		const size_t blockSize{ pHeader->Size };
		const size_t blockEnd{ blockStart + blockSize };

		Block* pPreviousFreeBlock{};
		Block* pFreeBlock{ pBlocks };

		/* Find the first block that starts after this heap of allocated memory */
		while (pFreeBlock)
		{
			if (reinterpret_cast<size_t>(pFreeBlock) >= blockEnd)
			{
				break;
			}

			pPreviousFreeBlock = pFreeBlock;
			pFreeBlock = pFreeBlock->pNext;
		}

		if (!pPreviousFreeBlock)
		{
			/* There is no free block after this heap of allocated memory, so add it to the start of the list */
			pPreviousFreeBlock = reinterpret_cast<Block*>(blockStart);
			pPreviousFreeBlock->Size = blockSize;
			pPreviousFreeBlock->pNext = pBlocks;

			pBlocks = pPreviousFreeBlock;
		}
		else if (reinterpret_cast<size_t>(pPreviousFreeBlock) + pPreviousFreeBlock->Size == blockStart)
		{
			/* The block before (pPreviousFreeBlock) the block we're freeing (pFreeBlock) ends right on the boundary of the heap of allocated memory */
			/* So we can merge the previous block and our heap of allocated memory together */

			pPreviousFreeBlock->Size += blockSize;
		}
		else
		{
			/* The block before (pPreviousBlock) the block we're freeing (pFreeBlock) does NOT end rightt on the boundary of the heap of allocated memory */
			/* Therefore we cannot merge the two */
			/* So, we have to create a new Block */

			Block* const pTemp{ reinterpret_cast<Block*>(blockStart) };

			pTemp->Size = blockSize;
			pTemp->pNext = pPreviousFreeBlock->pNext;

			pPreviousFreeBlock->pNext = pTemp;
			pPreviousFreeBlock = pTemp;
		}

		if (reinterpret_cast<size_t>(pPreviousFreeBlock) + pPreviousFreeBlock->Size ==
			reinterpret_cast<size_t>(pPreviousFreeBlock->pNext))
		{
			/* The new or merged block ends right on the next block of list, so we can merge these again */
			pPreviousFreeBlock->Size += pPreviousFreeBlock->pNext->Size;
			pPreviousFreeBlock->pNext = pPreviousFreeBlock->pNext->pNext;
		}
	}

	void* buffer() { return pBlocks; }

private:
	Block* CreateNewBlocks(const size_t sizeOfBlock, const size_t nrOfBlocks, const size_t alignment)
	{
		/* Find end of list */
		Block* pPreviousBlock{ pBlocks };

		if (pPreviousBlock)
		{
			while (pPreviousBlock->pNext)
			{
				pPreviousBlock = pPreviousBlock->pNext;
			}
		}

		Block* pFirstNewBlock{};
		for (size_t i{}; i < nrOfBlocks; ++i)
		{
			Block* const pNewBlock{ static_cast<Block*>(malloc(Utils::AlignForward<Header, Block>(sizeOfBlock, alignment))) };

			pNewBlock->pNext = nullptr;
			pNewBlock->Size = sizeOfBlock;

			if (pPreviousBlock)
			{
				pPreviousBlock->pNext = pNewBlock;
			}

			pPreviousBlock = pNewBlock;

			if (!pFirstNewBlock)
			{
				pFirstNewBlock = pNewBlock;
			}
		}

		NrOfBlocks += nrOfBlocks;

		return pFirstNewBlock;
	}

	template<typename T>
	void DeleteData(Block* pStart)
	{
		while (pStart)
		{
			Header* const pHeader{ reinterpret_cast<Header*>(reinterpret_cast<size_t>(pStart) - sizeof(Header)) };

			T* const pTemp{ reinterpret_cast<T*>(reinterpret_cast<size_t>(pStart) + sizeof(Header) + pHeader->Adjustment) };

			pTemp->~T();

			pStart = pStart->pNext;
		}
	}

	Block* pBlocks;
	void* pStart;
	size_t NrOfBlocks;
};

template<typename T, typename Allocator>
class STLFreeListAllocator final
{
public:
	using value_type = T;

	STLFreeListAllocator() = delete;
	STLFreeListAllocator(Allocator& alloc)
		: _Allocator{ alloc }
	{}
	template<typename U>
	STLFreeListAllocator(const STLFreeListAllocator<U, Allocator>& other) noexcept
		: _Allocator(other._Allocator)
	{}

	T* allocate(const size_t nrOfElements)
	{
		return _Allocator.allocate<T>(NrOfElements(nrOfElements), true, alignof(T));
	}

	void deallocate(T* p, [[maybe_unused]] size_t)
	{
		_Allocator.deallocate<T>(p);
	}

	T* buffer() const
	{
		return reinterpret_cast<T*>(_Allocator.buffer());
	}

	bool operator==(const STLFreeListAllocator<T, Allocator>& rhs) const noexcept
	{
		return buffer() == rhs.buffer();
	}

	bool operator!=(const STLFreeListAllocator<T, Allocator>& rhs) const noexcept
	{
		return buffer() != rhs.buffer();
	}

	Allocator& _Allocator;
};