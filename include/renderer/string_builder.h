#pragma once


#include <renderer/defines.h>
#include <bufferlib/buffer.h>

/* string builder is used to build string efficiently with convenience */
typedef struct string_builder_t
{
	/* stores the main string buffer */
	BUFFER string_buffer;
	/* stores the list of tab character ('\t') */
	BUFFER indentation_buffer;
} string_builder_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API string_builder_t* string_builder_create(u32 capacity);
RENDERER_API void string_builder_destroy(string_builder_t* builder);

/* appends the formatted string to the internal character buffer */
RENDERER_API void string_builder_append(string_builder_t* builder, const char* const format, ...);

/* appends null termination character, sometimes we don't need to append that so lets keep it explicit  not default */
RENDERER_API void string_builder_append_null(string_builder_t* builder);

/* increments the indentation for the subsequent calls to string_builder_append() */
RENDERER_API void string_builder_increment_indentation(string_builder_t* builder);
/* decrements the indentiation for the subsequent calls to the string_builder_append() */
RENDERER_API void string_builder_decrement_indentation(string_builder_t* builder);

/* returns the internal string buffer */
RENDERER_API char* string_builder_get_str(string_builder_t* builder);

END_CPP_COMPATIBLE
