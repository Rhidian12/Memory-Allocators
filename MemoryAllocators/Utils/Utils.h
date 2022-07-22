#pragma once

namespace Utils
{
	/* https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2 */
	inline bool IsPowerOfTwo(const size_t val) { return (val & (val - 1)) == 0; }

	/* https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/ */
	inline size_t AlignForward(const void* const p, const size_t wantedAlignment)
	{
		return ((reinterpret_cast<size_t>(p) - 1u + wantedAlignment) & -wantedAlignment) - reinterpret_cast<size_t>(p);
	}

	template<typename T>
	size_t AlignForward(const void* const p, const size_t wantedAlignment)
	{
		return ((reinterpret_cast<size_t>(p) - 1u + wantedAlignment) & -wantedAlignment) - reinterpret_cast<size_t>(p) + alignof(T);
	}
}