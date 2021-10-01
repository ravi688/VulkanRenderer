
#pragma once

#include <stdint.h>
#include <stdbool.h>

const char* string(uint32_t length, const char* format, ...);
const char* const* string_array(uint32_t count, ...);


const char* string_bool(bool value); 
const char* string_uint32(uint32_t value); 
const char* string_int32(int32_t value); 
const char* string_uint64(uint64_t value); 
const char* string_int64(int64_t value);
const char* string_float(float value); 
const char* string_char(char value);