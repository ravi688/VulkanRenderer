
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

RENDERER_API const char* string(uint32_t length, const char* format, ...);
RENDERER_API const char* const* string_array(uint32_t count, ...);


RENDERER_API const char* string_bool(bool value); 
RENDERER_API const char* string_uint32(uint32_t value); 
RENDERER_API const char* string_int32(int32_t value); 
RENDERER_API const char* string_uint64(uint64_t value); 
RENDERER_API const char* string_int64(int64_t value);
RENDERER_API const char* string_float(float value); 
RENDERER_API const char* string_char(char value);

END_CPP_COMPATIBLE
