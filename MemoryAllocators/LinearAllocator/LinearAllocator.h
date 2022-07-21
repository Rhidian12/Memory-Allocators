#pragma once
#include <stdexcept> /* std::invalid_argument */

#include "../Utils/Utils.h"

class LinearAllocator final
{
public:
	explicit LinearAllocator();
	~LinearAllocator();

	template<typename T>
	T* allocate(const size_t nrOfElements)
	{
		if (nrOfElements == 0)
		{
			throw std::invalid_argument{ "LinearAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		void* pData{ pCurrent };

		pCurrent = reinterpret_cast<T*>(pCurrent) + nrOfElements;

		return reinterpret_cast<T*>(pData);
	}

	template<typename T>
	void deallocate(T* p)
	{
		if (!p)
		{
			throw std::invalid_argument{ "LinearAllocator::Deallocate() > Cannot deallocate a nullptr" };
		}

		if (p > pEnd)
		{
			throw std::invalid_argument{ "LinearAllocator::Deallocate() > Cannot deallocate past allocated memory" };
		}

		if (p != pCurrent)
		{
			throw std::invalid_argument{ "LinearAllocator::Deallocate() > Cannot deallocate not last allocated memory" };
		}

		if (p < pStart)
		{
			throw std::invalid_argument{ "LinearAllocator::Deallocate() > Cannot deallocate before allocated memory" };
		}

		reinterpret_cast<char*>(pCurrent) -= sizeof(T);
	}

	size_t capacity() const { return reinterpret_cast<char*>(pEnd) - reinterpret_cast<char*>(pStart); }
	size_t size() const { return reinterpret_cast<char*>(pCurrent) - reinterpret_cast<char*>(pStart); }

private:
	void* pStart;
	void* pCurrent;
	void* pEnd;
};