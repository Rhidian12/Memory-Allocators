#pragma once
#include <stdlib.h> /* malloc */
#include <memory> /* std::distance() */

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
		pStackPointer = pBegin;
	}
	template<typename U>
	StackAllocator(const StackAllocator<U, N>& other)
		: pBegin{ other.pBegin }
		, pEnd{ other.pEnd }
		, pStackPointer{ other.pStackPointer }
	{}

	pointer allocate(const size_t nrOfElements)
	{
		if (nrOfElements <= std::distance(pStackPointer, pEnd))
		{
			pointer data = pStackPointer;
			pStackPointer += nrOfElements;
			return data;
		}
		else
		{
			throw std::bad_alloc{};
		}
	}

	void deallocate(pointer p, size_t n)
	{
		if (IsPointerInBuffer(p))
		{
			pStackPointer -= n;
		}
		else
		{
			throw std::invalid_argument{};
		}
	}

	constexpr size_t capacity() const
	{
		return N;
	}

private:
	bool IsPointerInBuffer(const_pointer p) const
	{
		return std::greater<const_pointer>{}(p, pBegin) && std::less<const_pointer>{}(p, pEnd);
	}

	pointer pBegin;
	pointer pEnd;
	pointer pStackPointer;
};