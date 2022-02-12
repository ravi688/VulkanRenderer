
#pragma once

#include <renderer/time.h>

namespace Showcase
{
	class SHOWCASE_API Time
	{

		Time() = delete; 		// static class only

	public:

		typedef time_handle_t Handle;

		static inline Handle createHandle() { return time_get_handle(); }
		static inline float getSeconds(Handle handle) { return time_get_seconds(handle); }
		static inline float getMilliseconds(Handle handle) { return time_get_milliseconds(handle); }
		static inline float getDeltaTime(Handle& handle) { return time_get_delta_time(&handle); }
	};
}