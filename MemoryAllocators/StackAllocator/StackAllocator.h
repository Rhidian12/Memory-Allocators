#pragma once
#include <stdlib.h> /* malloc */

template<typename T, size_t N>
class StackAllocator final
{
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	StackAllocator()
	{
		pBegin = malloc(N * sizeof(T));
		pEnd = pBegin + N;
		StackPointer = pBegin;
	}
	template<typename U>
	StackAllocator(const StackAllocator<U, N>& other)
		: pBegin{ other.pBegin }
		, pEnd{ other.pEnd }
		, StackPointer{ other.StackPointer }
	{}

	pointer allocate(const size_t amount)
	{

	}

private:
	pointer pBegin;
	pointer pEnd;
	pointer StackPointer;
};