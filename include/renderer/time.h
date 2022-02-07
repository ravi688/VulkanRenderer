
#pragma once

#include <stdint.h>
#include <renderer/defines.h>

typedef u32 time_handle_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API time_handle_t time_get_handle();
RENDERER_API float time_get_seconds(time_handle_t handle);
RENDERER_API u32 time_get_milliseconds(time_handle_t handle);
RENDERER_API float time_get_delta_time(time_handle_t* handle);

END_CPP_COMPATIBLE
