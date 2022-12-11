
#pragma once

#include <shader_compiler/defines.h>

#include <string.h>
#include <ctype.h>

SC_API const char* skip_whitespaces(const char* str, const char* const end);
SC_API u32 __get_word_length(const char* str, const char* const end, const char* delimiters);
SC_API u32 get_word_length(const char* string, const char delimiter);
SC_API int safe_strncmp(const char* str1, const char* const str2, u32 len);
SC_API bool is_empty(const char* start, const char* const end);
SC_API void remove_comments(char* start, const char* const end);
