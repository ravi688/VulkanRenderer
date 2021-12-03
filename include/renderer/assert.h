
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <calltrace/calltrace.h>

#ifdef ASSERT
#	undef ASSERT
#endif

#if defined(GLOBAL_DEBUG)
#	define ASSERT(boolean, ...)\
	do\
	{\
		if(!(boolean))\
		{\
			printf("Assertion Failed: ");\
			printf(__VA_ARGS__);\
			puts(calltrace_string());\
			exit(0);\
		}\
	} while(0)
#else
#	define ASSERT(boolean, ...)
#endif

#if defined(GLOBAL_DEBUG)
#	define LOG_MSG(...) log_msg(__VA_ARGS__)
#	define LOG_ERR(...) log_err(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) log_fetal_err(__VA_ARGS__)
#else
#	define LOG_MSG(...)\
	{\
		printf("[Log]: ");\
		printf(__VA_ARGS__);\
	}
#	define LOG_ERR(...)\
	{\
		printf("[Error]: ");\
		printf(__VA_ARGS__);\
	}
#	define LOG_FETAL_ERR(...)\
	{\
		printf("[Fetal Error]: ");\
		printf(__VA_ARGS__);\
		exit(0);\
	}
#endif