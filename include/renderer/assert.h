
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <calltrace/calltrace.h>

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
	
#if defined(GLOBAL_DEBUG)
#	define LOG_MSG(...) log_msg(__VA_ARGS__)
#	define LOG_ERR(...) log_err(__VA_ARGS__)
#	define LOG_WRN(...) log_wrn(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) log_fetal_err(__VA_ARGS__)
#else
#	define LOG_WRN(...) debug_log("[Warning]: ", __VA_ARGS__)
#	define LOG_MSG(...) debug_log("[Log]: ", __VA_ARGS__)
#	define LOG_ERR(...) debug_log("[Error]: ", __VA_ARGS__)
#	define LOG_FETAL_ERR(...) debug_log_exit("[Fetal Error]: ", __VA_ARGS__)
#endif

static void debug_log(const char* description, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	printf(description);
	vprintf(format, args);
	va_end(args);
}

static void debug_log_exit(const char* description, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_log(description, format, args);
	va_end(args);
	exit(0);
}
