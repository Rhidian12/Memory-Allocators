#pragma once
#include <stdexcept> /* std::invalid_argument */

#include "../Utils/Utils.h"

class LinearAllocator final
{
public:
	explicit LinearAllocator();
	~LinearAllocator();

	template<typename T>
	T* Allocate(const size_t nrOfElements)
	{
		if (nrOfElements == 0)
		{
			throw std::invalid_argument{ "LinearAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		void* pData{ pCurrent };

		reinterpret_cast<T*>(pCurrent) += nrOfElements;

		return pData;
	}

	template<typename T>
	void Deallocate(T* p)
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

private:
	void* pStart;
	void* pCurrent;
	void* pEnd;
};