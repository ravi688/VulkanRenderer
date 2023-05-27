/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: dictionary.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


//	TODO: Dictionary data structure should be in a submodule called HPDS (High Performance Data Structures)
//	TODO: Make template version

#pragma once

#include <shader_compiler/defines.h>
#include <bufferlib/buffer.h>
#include <calltrace/calltrace.h>

#define DICTIONARY_INVALID_INDEX BUF_INVALID_INDEX

typedef struct dictionary_t
{ 
	BUFFER buffer;										// holds all the key-value pairs in a contigous block of memory
	bool (*key_comparer)(void* compare_key, void* key);	// compare_key = the key to be compared, key = the key already present in the dictionary
	u32 key_size;										// holds the size of the key
	u32 value_size;										// holds the size of the value
} dictionary_t;

#ifdef __cplusplus
	extern "C" {
#endif // __cplusplus

// constructors and destructors
#define dictionary_create(Tkey, Tvalue, capacity, key_comparer) __dictionary_create(sizeof(Tkey), sizeof(Tvalue), capacity, key_comparer)
#define __dictionary_create(...) define_alias_function_macro(__dictionary_create, __VA_ARGS__)
SC_API function_signature(dictionary_t, __dictionary_create, u32 key_size, u32 value_size, buf_ucount_t capacity, bool (*key_comparer)(void* compare_key, void* key));
#define dictionary_free(...) define_alias_function_macro(dictionary_free, __VA_ARGS__)
SC_API function_signature(void, dictionary_free, dictionary_t* dictionary);

// getters
#define dictionary_get_at(...) define_alias_function_macro(dictionary_get_at, __VA_ARGS__)
SC_API function_signature(void, dictionary_get_at, dictionary_t* dictionary, buf_ucount_t index, void* out_key, void* out_value);
#define dictionary_get_value(...) define_alias_function_macro(dictionary_get_value, __VA_ARGS__)
SC_API function_signature(void, dictionary_get_value, dictionary_t* dictionary, void* key, void* out_value);
#define dictionary_try_get_value(...) define_alias_function_macro(dictionary_try_get_value, __VA_ARGS__)
SC_API function_signature(bool, dictionary_try_get_value, dictionary_t* dictionary, void* key, void* out_value);
#define dictionary_get_key_at(...) define_alias_function_macro(dictionary_get_key_at, __VA_ARGS__)
SC_API function_signature(void, dictionary_get_key_at, dictionary_t* dictionary, buf_ucount_t index, void* out_key);
#define dictionary_get_key_ptr_at(...) define_alias_function_macro(dictionary_get_key_ptr_at, __VA_ARGS__)
SC_API function_signature(void*, dictionary_get_key_ptr_at, dictionary_t* dictionary, buf_ucount_t index);
#define dictionary_get_value_at(...) define_alias_function_macro(dictionary_get_value_at, __VA_ARGS__)
SC_API function_signature(void, dictionary_get_value_at, dictionary_t* dictionary, buf_ucount_t index, void* out_value);
#define dictionary_get_value_ptr_at(...) define_alias_function_macro(dictionary_get_value_ptr_at, __VA_ARGS__)
SC_API function_signature(void*, dictionary_get_value_ptr_at, dictionary_t* dictionary, buf_ucount_t index);
#define dictionary_get_value_ptr(...) define_alias_function_macro(dictionary_get_value_ptr, __VA_ARGS__)
SC_API function_signature(void*, dictionary_get_value_ptr, dictionary_t* dictionary, void* key);
#define dictionary_get_count(...) define_alias_function_macro(dictionary_get_count, __VA_ARGS__)
SC_API function_signature(buf_ucount_t, dictionary_get_count, dictionary_t* dictionary);
#define dictionary_get_capacity(...) define_alias_function_macro(dictionary_get_capacity, __VA_ARGS__)
SC_API function_signature(buf_ucount_t, dictionary_get_capacity, dictionary_t* dictionary);


// setters
#define dictionary_set_at(...) define_alias_function_macro(dictionary_set_at, __VA_ARGS__)
SC_API function_signature(void, dictionary_set_at, dictionary_t* dictionary, buf_ucount_t index, void* in_key, void* in_value);
#define dictionary_set_value(...) define_alias_function_macro(dictionary_set_value, __VA_ARGS__)
SC_API function_signature(void, dictionary_set_value, dictionary_t* dictionary, void* key, void* in_value);
#define dictionary_set_value_at(...) define_alias_function_macro(dictionary_set_value_at, __VA_ARGS__)
SC_API function_signature(void, dictionary_set_value_at, dictionary_t* dictionary, buf_ucount_t index, void* in_value);

// logic functions
#define dictionary_add(...) dictionary_push(__VA_ARGS__)
#define dictionary_push(...) define_alias_function_macro(dictionary_push, __VA_ARGS__)
SC_API function_signature(void, dictionary_push, dictionary_t* dictionary, void* in_key, void* in_value);
#define dictionary_pop(...) define_alias_function_macro(dictionary_pop, __VA_ARGS__)
SC_API function_signature(void, dictionary_pop, dictionary_t* dictionary, void* out_key, void* out_value);
#define dictionary_remove(...) define_alias_function_macro(dictionary_remove, __VA_ARGS__)
SC_API function_signature(void, dictionary_remove, dictionary_t* dictionary, void* key);
#define dictionary_contains(...) define_alias_function_macro(dictionary_contains, __VA_ARGS__)
SC_API function_signature(bool, dictionary_contains, dictionary_t* dictionary, void* key);
#define dictionary_clear(...) define_alias_function_macro(dictionary_clear, __VA_ARGS__)
SC_API function_signature(void, dictionary_clear, dictionary_t* dictionary);
#define dictionary_find_index_of(...) define_alias_function_macro(dictionary_find_index_of, __VA_ARGS__)
SC_API function_signature(buf_ucount_t, dictionary_find_index_of, dictionary_t* dictionary, void* key);

#define dictionary_key_comparer_char dictionary_key_comparer_s8
#define dictionary_key_comparer_int dictionary_key_comparer_s32
#define dictionary_key_comparer_uint dictionary_key_comparer_u32
SC_API bool dictionary_key_comparer_u16(void* v1, void* v2);
SC_API bool dictionary_key_comparer_u32(void* v1, void* v2);
SC_API bool dictionary_key_comparer_buf_ucount_t(void* v1, void* v2);
SC_API bool dictionary_key_comparer_u8(void* v1, void* v2);
SC_API bool dictionary_key_comparer_s16(void* v1, void* v2);
SC_API bool dictionary_key_comparer_s32(void* v1, void* v2);
SC_API bool dictionary_key_comparer_s64(void* v1, void* v2);
SC_API bool dictionary_key_comparer_s8(void* v1, void* v2);
SC_API bool dictionary_key_comparer_float(void* v1, void* v2);
SC_API bool dictionary_key_comparer_double(void* v1, void* v2);
SC_API bool dictionary_key_comparer_string(void* v1, void* v2);


#ifdef __cplusplus
}
#endif // __cplusplus
