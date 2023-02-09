
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <calltrace/calltrace.h>

#include <renderer/debug.h>

#ifdef ASSERT
#	undef ASSERT
#endif

#if defined(GLOBAL_DEBUG)
#	define ASSERT_WRN(boolean, ...)\
	do\
	{\
		if(!(boolean))\
		{\
			printf("[Warning] Assertion Failed: ");\
			printf(__VA_ARGS__);\
			puts(calltrace_string());\
		}\
	} while(0)
#	define ASSERT(boolean, ...)\
	do\
	{\
		if(!(boolean))\
		{\
			printf("[Fetal Error] Assertion Failed: ");\
			printf(__VA_ARGS__);\
			puts(calltrace_string());\
			exit(0);\
		}\
	} while(0)
#else
#	define ASSERT_WRN(boolean, ...)
#	define ASSERT(boolean, ...)
#endif

#if defined(GLOBAL_DEBUG)
#	define assert(condition) ASSERT((condition) != false, "\"%s\" is found to be false\n", #condition)
#	define assert_wrn(condition) ASSERT_WRN((condition) != false, "\"%s\" is found to be false\n", #condition)
#	define ASSERT_NOT_NULL(ptr) assert(ptr != NULL)
#else
#	define assert(condition)
#	define assert_wrn(condition)
#	define ASSERT_NOT_NULL(ptr)
#endif

#define ASSERT_CALLED_ONCE() static int __FUNCTION__##call_counter = 0; ++__FUNCTION__##call_counter; ASSERT(__FUNCTION__##call_counter == 1, "%s has been called more than once\n", __FUNCTION__)
