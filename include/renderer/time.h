
#pragma once

#include <stdint.h>

typedef uint64_t time_handle_t;

time_handle_t time_get_handle();
float time_get_seconds(time_handle_t handle);
uint32_t time_get_milliseconds(time_handle_t handle);
float time_get_delta_time(time_handle_t* handle);
