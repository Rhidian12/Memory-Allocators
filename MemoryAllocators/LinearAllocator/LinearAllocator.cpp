#include "LinearAllocator.h"

#include <stdlib.h> /* malloc */

LinearAllocator::LinearAllocator()
	: pStart{}
	, pCurrent{}
	, pEnd{}
{
	const size_t size{ 256 };

	pStart = malloc(size);
	pCurrent = pStart;
	pEnd = reinterpret_cast<char*>(pStart) + size;
}

LinearAllocator::LinearAllocator(const size_t nrOfBytes)
	: pStart{}
	, pCurrent{}
	, pEnd{}
{
	pStart = malloc(nrOfBytes);
	pCurrent = pStart;
	pEnd = reinterpret_cast<char*>(pStart) + nrOfBytes;
}

LinearAllocator::~LinearAllocator()
{
	free(pStart);
}
