#include <iostream>
#include <vld.h>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "StackAllocator/StackAllocator.h"
#include "LinearAllocator/LinearAllocator.h"
#include "FreeListAllocator/FreeListAllocator.h"

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
		LinearAllocator alloc{ 40 };

		REQUIRE(alloc.capacity() != 0);
		REQUIRE(alloc.capacity() == 40);
		REQUIRE(alloc.size() == 0);

		int* pArr{};
		REQUIRE_NOTHROW(pArr = alloc.allocate<int>(10));
		REQUIRE(alloc.size() == 40);
		REQUIRE_THROWS_AS(alloc.deallocate(pArr), std::bad_function_call);

		REQUIRE_THROWS_AS(alloc.allocate<int>(11), std::bad_alloc);

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr + i != nullptr);
			pArr[i] = i;
		}

		for (int i{}; i < 10; ++i)
		{
			REQUIRE(pArr[i] == i);
		}

		REQUIRE_THROWS_AS(alloc.deallocate<int>(nullptr), std::bad_function_call);

		int* pInt{ new int{} };
		REQUIRE_THROWS_AS(alloc.deallocate(pInt), std::bad_function_call);
		delete pInt;
	}
}

TEST_CASE("FreeList Allocator Test")
{
	SECTION("Non-STL use")
	{
		{
			FreeListAllocator alloc{ 57 }; /* 40 for 10 integers + 16 for the allocation header + 1 for space */

			REQUIRE(alloc.capacity() != 0);
			REQUIRE(alloc.capacity() == 57);
			REQUIRE(alloc.size() == 0);

			int* pArr{};
			REQUIRE_NOTHROW(pArr = alloc.allocate<int>(10_elem));
			REQUIRE(alloc.size() == 1);
			REQUIRE_NOTHROW(alloc.deallocate(pArr));

			REQUIRE_THROWS_AS(alloc.allocate<int>(11_elem), std::bad_alloc);

			pArr = alloc.allocate<int>(10_elem);
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

			REQUIRE_NOTHROW(alloc.deallocate(pArr));
			pArr = nullptr;
			REQUIRE_THROWS_AS(alloc.deallocate(nullptr), std::invalid_argument);
		}

		/* Deallocating memory not allocated by this allocator is UB */

		{
			FreeListAllocator alloc{ 63 }; /* 12 for 3 integers + 48 for the allocation headers + 3 for space */

			REQUIRE(alloc.capacity() == 63);
			REQUIRE(alloc.size() == 0);

			int* pOne{}, * pTwo{}, * pThree{};

			REQUIRE_NOTHROW(pOne = alloc.allocate<int>(1_elem));
			REQUIRE(alloc.size() == 1);

			REQUIRE_NOTHROW(pTwo = alloc.allocate<int>(1_elem));
			REQUIRE(alloc.size() == 2);

			REQUIRE_NOTHROW(pThree = alloc.allocate<int>(1_elem));
			REQUIRE(alloc.size() == 3);

			REQUIRE_NOTHROW(alloc.deallocate(pTwo));
			pTwo = nullptr;

			/* There should now be a free block of memory between pOne and pThree */

			REQUIRE_NOTHROW(pTwo = alloc.allocate<int>(1_elem));
			REQUIRE(pTwo > pOne);
			REQUIRE(pTwo < pThree);
		}
	}

	SECTION("STL use")
	{
		SECTION("STL use")
		{
			FreeListAllocator alloc{ 33 }; /* Don't give it enough memory */
			std::vector<int, STLFreeListAllocator<int, FreeListAllocator>> vec(alloc);

			for (int i{}; i < 10; ++i)
			{
				REQUIRE_NOTHROW(vec.push_back(i)); /* Force a reallocation */
			}
		}
	}
}