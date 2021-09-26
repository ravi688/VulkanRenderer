
#pragma once

#include <stdint.h>

const char* string(uint32_t length, const char* format, ...);
const char* const* string_array(uint32_t count, ...);