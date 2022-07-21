#pragma once

namespace Utils
{
	/* https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2 */
	inline bool IsPowerOfTwo(const size_t val) { return (val & (val - 1)) == 0; }

	/* https://embeddedartistry.com/blog/2017/02/22/generating-aligned-memory/ */
	template<typename T>
	size_t AlignForward(const T* p, const size_t wantedAlignment) { return ((p + (wantedAlignment - 1)) & ~(wantedAlignment - 1)); }
}