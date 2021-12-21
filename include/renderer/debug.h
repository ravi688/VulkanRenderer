#pragma once

#include <calltrace/calltrace.h>

#define log_u32(var) log_msg("%s: %u\n", #var, var)
#define log_u64(var) log_msg("%s: %llu\n", #var, var)
#define log_ptr(var) log_msg("%s: %p\n", #var, var)

