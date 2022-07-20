#include "LinearAllocator.h"

#include <stdlib.h> /* malloc */

LinearAllocator::LinearAllocator()
	: pStart{}
	, pCurrent{}
{
	pStart = malloc(256);
	pCurrent = pStart;
}

LinearAllocator::~LinearAllocator()
{
	free(pStart);
}
