#include <iostream>
#include <vld.h>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "StackAllocator/StackAllocator.h"
#include "LinearAllocator/LinearAllocator.h"

TEST_CASE("Stack Allocator Test")
{
	SECTION("Non-STL use")
	{
		StackAllocator<10 * sizeof(int)> alloc{};

		REQUIRE(alloc.capacity() != 0);
		REQUIRE(alloc.capacity() / sizeof(int) == 10);
		REQUIRE(alloc.size() == 0);

		int* pArr{};
		REQUIRE_NOTHROW(pArr = alloc.allocate<int>(10));
		REQUIRE_NOTHROW(alloc.deallocate(pArr, 10));
		pArr = nullptr;

		REQUIRE_THROWS_AS(alloc.allocate<int>(11), std::bad_alloc);

		pArr = alloc.allocate<int>(10);
		REQUIRE(pArr != nullptr);
		REQUIRE(alloc.size() / sizeof(int) == 10);

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr + i != nullptr);
			pArr[i] = i;
		}

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr[i] == i);
		}

		REQUIRE_THROWS_AS(alloc.deallocate<int>(nullptr, 10), std::invalid_argument);
		REQUIRE_NOTHROW(alloc.deallocate(pArr, 10));

		int* pInt{ new int{} };
		REQUIRE_THROWS_AS(alloc.deallocate(pInt, 10), std::invalid_argument);
		delete pInt;
	}

	SECTION("STL use")
	{
		/* I want to test with 10 integers, so 10 * sizeof(int), but std::vector also automatically allocates std::_Container_proxy */
		using StackAlloc = StackAllocator<10 * sizeof(int) + sizeof(std::_Container_proxy)>;

		StackAlloc alloc{};
		std::vector<int, STLStackAllocator<int, StackAlloc>> vec(alloc);

		/* we have to reserve first, otherwise we cannot guarantee LIFO deallocation which will result in UB */
		REQUIRE_NOTHROW(vec.reserve(10));

		for (int i{}; i < 10; ++i)
		{
			REQUIRE_NOTHROW(vec.push_back(i));
		}

		/* any new reallocation will trigger a std::bad_alloc */
		REQUIRE_THROWS_AS(vec.push_back(10), std::bad_alloc);
	}
}

TEST_CASE("Linear Allocator Test")
{
	SECTION("Non-STL use")
	{
		LinearAllocator alloc{};

		REQUIRE(alloc.capacity() != 0);
		REQUIRE(alloc.size() == 0);

		int* pArr{};
		REQUIRE_NOTHROW(pArr = alloc.allocate<int>(10));
		REQUIRE_NOTHROW(alloc.deallocate(pArr));
		pArr = nullptr;

		REQUIRE_THROWS_AS(alloc.allocate<int>(11), std::bad_alloc);

		pArr = alloc.allocate<int>(10);
		REQUIRE(pArr != nullptr);

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr + i != nullptr);
			pArr[i] = i;
		}

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr[i] == i);
		}

		REQUIRE_THROWS_AS(alloc.deallocate<int>(nullptr), std::invalid_argument);
		REQUIRE_NOTHROW(alloc.deallocate(pArr));

		int* pInt{ new int{} };
		REQUIRE_THROWS_AS(alloc.deallocate(pInt), std::invalid_argument);
		delete pInt;
	}
}