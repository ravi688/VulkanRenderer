#include <renderer/debug.h>

RENDERER_API void debug_break__(debug_break_reason_t reason)
{
	switch(reason)
	{
		case DEBUG_BREAK_REASON_NONE:
		case DEBUG_BREAK_REASON_INVALID_MEMORY_ACCESS:
		case DEBUG_BREAK_REASON_ASSERTION_FAILED:
		case DEBUG_BREAK_REASON_UNKNOWN:
		default:
			__builtin_trap();
	}
}
