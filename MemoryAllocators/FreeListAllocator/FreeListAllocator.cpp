#include "FreeListAllocator.h"

FreeListAllocator::FreeListAllocator()
	: pPools{}
	, AmountOfPools{ sizeof(pPools) / sizeof(Block) }
	, PoolSizes{}
{
	const size_t size{ 16 };
	for (size_t i{}; i < AmountOfPools; ++i)
	{
		pPools[i] = new Block[size];
		PoolSizes[i] = size;
	}
}