
#pragma once

#include <common/assert.h>

#define release_assert__(condition, ...) ASSERT(DESCRIPTION((condition) == true), __VA_ARGS__)
#define release_assert_wrn__(condition, ...) ASSERT_WRN(DESCRIPTION((condition) == true), __VA_ARGS__)
#define release_assert_not_null__(ptr) _ASSERT((ptr) != NULL)
#define release_assert_called_once__() static int __FUNCTION__##call_counter = 0; ++__FUNCTION__##call_counter; release_assert(__FUNCTION__##call_counter == 1, "%s has been called more than once\n", __FUNCTION__)
#define _release_assert__(condition) _ASSERT(condition)
#define _release_assert_wrn__(condition) _ASSERT_WRN(condition)

#if defined(GLOBAL_DEBUG)
#	define debug_assert__(...) release_assert__(__VA_ARGS__)
#	define debug_assert_wrn__(...) release_assert_wrn__(__VA_ARGS__)
#	define debug_assert_not_null__(ptr) release_assert_not_null__(ptr)
#	define debug_assert_called_once__() release_assert_called_once__()
#	define _debug_assert__(condition) _release_assert__(condition)
#	define _debug_assert_wrn__(condition) _release_assert_wrn__(condition)
#else
#	define debug_assert__(...)
#	define debug_assert_wrn__(...)
#	define debug_assert_not_null__(ptr)
#	define debug_assert_called_once__()
#	define _debug_assert__(condition)
#	define _debug_assert_wrn__(condition)
#endif
