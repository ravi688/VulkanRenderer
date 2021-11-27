
//TODO: Dictionary data structure should be in a submodule called HPDS (High Performance Data Structures)

#pragma once

#include <buffer.h>
#include <renderer/defines.h>
#include <calltrace/calltrace.h>

typedef struct dictionary_t
{ 
	BUFFER buffer;
	bool (*key_comparer)(void* compare_key, void* key);
	u32 key_size;
	u32 value_size;
} dictionary_t;

#define dictionary_new(Tkey, Tvalue, key_comparer) __dictionary_new(sizeof(Tkey), sizeof(Tvalue), key_comparer)
function_signature(dictionary_t*, __dictionary_new, u32 key_size, u32 value_size, bool (*key_comparer)(void* compare_key, void* key));

function_signature(void, dictionary_destroy, dictionary_t* dictionary);

function_signature(void*, dictionary_get_value_at, dictionary_t* dictionary, u64 index);
function_signature(void*, dictionary_get_value, dictionary_t* dictionary, void* key);
function_signature(void, dictionary_set_value, dictionary_t* dictionary, void* key, void* value);
function_signature(void, dictionary_set_value_at, dictionary_t* dictionary, u64 index, void* value);

function_signature(void, dictionary_add, dictionary_t* dictionary, void* key, void* value);
function_signature(void, dictionary_remove, dictionary_t* dictionary, void* remove_key);
function_signature(bool, dictionary_contains, dictionary_t* dictionary, void* key);

#define __dictionary_new(...) define_alias_function_macro(__dictionary_new, __VA_ARGS__)
#define dictionary_destroy(...) define_alias_function_macro(dictionary_destroy, __VA_ARGS__)

#define dictionary_get_value_at(...) define_alias_function_macro(dictionary_get_value_at, __VA_ARGS__)
#define dictionary_get_value(...) define_alias_function_macro(dictionary_get_value, __VA_ARGS__)
#define dictionary_set_value(...) define_alias_function_macro(dictionary_set_value, __VA_ARGS__)
#define dictionary_set_value_at(...) define_alias_function_macro(dictionary_set_value_at, __VA_ARGS__)

#define dictionary_add(...) define_alias_function_macro(dictionary_add, __VA_ARGS__)
#define dictionary_remove(...) define_alias_function_macro(dictionary_remove, __VA_ARGS__)
#define dictionary_contains(...) define_alias_function_macro(dictionary_contains, __VA_ARGS__)

