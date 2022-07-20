#pragma once

class LinearAllocator final
{
public:
	explicit LinearAllocator();
	~LinearAllocator();

private:
	void* pStart;
	void* pCurrent;
};