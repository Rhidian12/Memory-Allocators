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

LinearAllocator::~LinearAllocator()
{
	free(pStart);
}
