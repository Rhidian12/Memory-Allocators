#pragma once
#include <stdexcept> /* std::invalid_argument */
#include <iostream>

template<size_t N>
class StackAllocator final
{
public:
	StackAllocator()
		: Buffer{}
		, StackPointer{}
	{}

	template<typename T>
	T* allocate(const size_t nrOfElements)
	{
		if (nrOfElements * sizeof(T) <= N - StackPointer)
		{
			T* data = reinterpret_cast<T*>(&Buffer[StackPointer]);
			StackPointer += nrOfElements * sizeof(T);
			return data;
		}
		else
		{
			throw std::bad_alloc{};
		}
	}

	template<typename T>
	void deallocate(T* p, size_t n)
	{
		if (IsPointerInBuffer(p))
		{
			StackPointer -= n * sizeof(T);
		}
		else
		{
			throw std::invalid_argument{ "pointer is not part of allocated memory" };
		}
	}

	constexpr size_t capacity() const
	{
		return N;
	}

	size_t size() const
	{
		return StackPointer;
	}

	constexpr size_t max_size() const
	{
		return N;
	}

	char* buffer() const
	{
		return Buffer;
	}

private:
	template<typename T>
	bool IsPointerInBuffer(const T* p) const
	{
		return std::greater_equal<const T*>{}(p, reinterpret_cast<const T*>(&Buffer)) && std::less<const T*>{}(p, reinterpret_cast<const T*>(&Buffer + N));
	}

	char Buffer[N];
	size_t StackPointer;
};

template<typename T, typename Allocator>
class STLStackAllocator final
{
public:
	using value_type = T;

	STLStackAllocator() = delete;
	STLStackAllocator(Allocator& alloc)
		: _Allocator{ alloc }
	{}
	template<typename U>
	STLStackAllocator(const STLStackAllocator<U, Allocator>& other) noexcept
		: _Allocator(other._Allocator)
	{}

	T* allocate(const size_t nrOfElements)
	{
		return _Allocator.allocate<T>(nrOfElements);
	}

	void deallocate(T* p, size_t n)
	{
		_Allocator.deallocate<T>(p, n);
	}

	T* buffer() const
	{
		return reinterpret_cast<T*>(_Allocator.buffer());
	}

	bool operator==(const STLStackAllocator<T, Allocator>& rhs) const noexcept
	{
		return buffer() == rhs.buffer();
	}

	bool operator!=(const STLStackAllocator<T, Allocator>& rhs) const noexcept
	{
		return buffer() != rhs.buffer();
	}

	Allocator& _Allocator;
};