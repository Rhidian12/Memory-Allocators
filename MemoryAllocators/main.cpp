#include <iostream>
#include <vld.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "StackAllocator/StackAllocator.h"

TEST_CASE("Stack Allocator Test")
{
	StackAllocator<int, 10> alloc{};

	REQUIRE(alloc.capacity() != 0);
	REQUIRE(alloc.capacity() == 10);

	int* pArr{};
	REQUIRE_NOTHROW(pArr = alloc.allocate(10));
	REQUIRE_NOTHROW(alloc.deallocate(pArr, 10));
	pArr = nullptr;

	REQUIRE_THROWS_AS(alloc.allocate(11), std::bad_alloc);

	pArr = alloc.allocate(10);
	REQUIRE(pArr != nullptr);

	for (int i{}; i < 10; ++i)
	{
		REQUIRE(pArr + i != nullptr);
	}

	REQUIRE_THROWS_AS(alloc.deallocate(nullptr, 10), std::invalid_argument);
	REQUIRE_NOTHROW(alloc.deallocate(pArr, 10));

	int* pInt{ new int{} };
	REQUIRE_THROWS_AS(alloc.deallocate(pInt, 10), std::invalid_argument);
	delete pInt;
}