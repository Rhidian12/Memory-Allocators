#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "StackAllocator/StackAllocator.h"

TEST_CASE("Stack Allocator Test")
{
	StackAllocator<int, 10> alloc{};

	REQUIRE(alloc.capacity() != 0);
	REQUIRE(alloc.capacity() == 10);

	REQUIRE_NOTHROW(alloc.allocate(10));
	alloc.deallocate(10);

	REQUIRE_THROWS_AS(alloc.allocate(11), std::bad_alloc);

	int* pArr = alloc.allocate(10);
	REQUIRE(pArr != nullptr);

	for (int i{}; i < 10; ++i)
	{
		REQUIRE(pArr + i != nullptr);
	}

	alloc.deallocate(10);
}