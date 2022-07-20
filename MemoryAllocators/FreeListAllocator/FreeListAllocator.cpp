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