#pragma once

namespace Utils
{
	/* https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2 */
	inline bool IsPowerOfTwo(const size_t val) { return (val & (val - 1)) == 0; }

	/* https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/ */
	inline size_t AlignForward(const void* const p, const size_t alignment)
	{
		// Example: Suppose p = 12 == 0xC and alignment = 8 == 0x8
		// (12 + (8 - 1)) & ~(8 - 1)
		// 19 & ~7
		// 0x0013 & ~0x0007
		// 0x0013 & 0xFFF8
		// 0x0010
		// 16

		return (reinterpret_cast<size_t>(p) + (alignment - 1)) & ~(alignment - 1) - reinterpret_cast<size_t>(p);
	}

	template<typename ... Values>
	size_t AlignForward(const void* const p, const size_t alignment)
	{
		size_t adjustment = AlignForward(p, alignment);

		size_t neededSpace = (sizeof(Values...) + ...);

		if (adjustment < neededSpace)
		{
			neededSpace -= adjustment;

			adjustment += alignment * (neededSpace / alignment);

			if (neededSpace % alignment > 0)
			{
				adjustment += alignment;
			}
		}

		return adjustment;
	}

	size_t AlignForward(size_t p, const size_t alignment)
	{
		const size_t modulo = p & (alignment - 1);

		if (modulo != 0)
		{
			p += alignment - modulo;
		}

		return p;
	}
}