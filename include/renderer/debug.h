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
