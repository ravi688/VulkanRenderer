
#pragma once

#include <tuple/header_config.h>
#include <tuple/tuple.h>

#include <stdint.h>

tuple_t(uint64_t, pchar_t) load_text_from_file(const char* file_name); 
tuple_t(uint64_t, pchar_t) load_binary_from_file(const char* file_name); 
tuple_t(uint64_t, pchar_t) load_text_from_file_exclude_comments(const char* file_name);