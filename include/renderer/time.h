
#pragma once

#include <stdint.h>
#include <renderer/defines.h>

typedef uint64_t time_handle_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

RENDERER_API time_handle_t time_get_handle();
RENDERER_API float time_get_seconds(time_handle_t handle);
RENDERER_API uint32_t time_get_milliseconds(time_handle_t handle);
RENDERER_API float time_get_delta_time(time_handle_t* handle);

#ifdef __cplusplus
}
#endif // __cplusplus
