#pragma once
#include <stdexcept> /* std::invalid_argument */
#include <functional> /* std::bad_function_call */

#include "../Utils/Utils.h"

class LinearAllocator final
{
public:
	explicit LinearAllocator();
	explicit LinearAllocator(const size_t nrOfBytes);
	~LinearAllocator();

	template<typename T>
	T* allocate(const size_t nrOfElements)
	{
		if (nrOfElements == 0)
		{
			throw std::invalid_argument{ "LinearAllocator::Allocate() > Cannot allocate 0 elements" };
		}

		if (reinterpret_cast<T*>(pCurrent) + nrOfElements >= pEnd)
		{
			throw std::bad_alloc{};
		}

		void* pData{ pCurrent };

		pCurrent = reinterpret_cast<T*>(pCurrent) + nrOfElements;

		return reinterpret_cast<T*>(pData);
	}

	template<typename T>
	void deallocate([[unused]] T* p)
	{
		throw std::bad_function_call{};
	}

	size_t capacity() const { return reinterpret_cast<char*>(pEnd) - reinterpret_cast<char*>(pStart) - 1; }
	size_t size() const { return reinterpret_cast<char*>(pCurrent) - reinterpret_cast<char*>(pStart); }

private:
	void* pStart;
	void* pCurrent;
	void* pEnd; /* points past allocated memory */
};