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

	void Deallocate(void* p)
	{

	}

private:
	void* pStart;
	void* pCurrent;
	void* pEnd;
};