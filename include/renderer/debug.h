#pragma once

#include <common/debug.h>
#include <calltrace/calltrace.h>

#define log_u32(var) log_msg("%s: %u\n", #var, var)
#define log_u64(var) log_msg("%s: %llu\n", #var, var)
#define log_ptr(var) log_msg("%s: %p\n", #var, var)

#if defined(GLOBAL_DEBUG)
#	define LOG_MSG(...) log_msg(__VA_ARGS__)
#	define LOG_ERR(...) log_err(__VA_ARGS__)
#	define LOG_WRN(...) log_wrn(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) log_fetal_err(__VA_ARGS__)
#else
#	define LOG_WRN(...) DEBUG_LOG_WARNING( __VA_ARGS__)
#	define LOG_MSG(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define LOG_ERR(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#endif


#include <renderer/defines.h>

#if defined(debug_break)
#undef debug_break
#endif

typedef enum debug_break_reason_t
{
	DEBUG_BREAK_REASON_NONE,
	DEBUG_BREAK_REASON_INVALID_MEMORY_ACCESS,
	DEBUG_BREAK_REASON_ASSERTION_FAILED,
	DEBUG_BREAK_REASON_UNKNOWN
} debug_break_reason_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API NO_RETURN_FUNCTION void debug_break__(debug_break_reason_t reason);

END_CPP_COMPATIBLE
