
#pragma once

#include <common/defines.hpp>

#ifdef SUTK_STATIC_LIBRARY
#	define SUTK_API
#elif SUTK_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllexport)
#else
#	define SUTK_API
#endif


#include <limits>

namespace SUTK
{
	/* For now, we will only support ASCII set of characters,
	 * but in future we may increase its bit-width and display more
	 * complex characters. */
	typedef u8 CharType;

	#define CHAR_TYPE_MAX std::numeric_limits<CharType>::max()

	/* For now, we can represent number of lines up to U32_MAX,
	 * but in future we may increase its bit-width to let users to 
	 * work with gigantic text files. */
	typedef u32 LineCountType;

	#define LINE_COUNT_TYPE_MAX std::numeric_limits<LineCountType>::max()
}
