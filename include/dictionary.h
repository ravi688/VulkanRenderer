
//	TODO: Dictionary data structure should be in a submodule called HPDS (High Performance Data Structures)
//	TODO: Make template version

#pragma once

#include <buffer.h>
#include <renderer/defines.h>
#include <calltrace/calltrace.h>

#define DICTIONARY_INVALID_INDEX BUF_INVALID_INDEX

typedef struct dictionary_t
{ 
	BUFFER buffer;										// holds all the key-value pairs in a contigous block of memory
	bool (*key_comparer)(void* compare_key, void* key);	// compare_key = the key to be compared, key = the key already present in the dictionary
	u32 key_size;										// holds the size of the key
	u32 value_size;										// holds the size of the value
} dictionary_t;

// constructors and destructors
#define dictionary_create(Tkey, Tvalue, capacity, key_comparer) __dictionary_create(sizeof(Tkey), sizeof(Tvalue), capacity, key_comparer)
#define __dictionary_create(...) define_alias_function_macro(__dictionary_create, __VA_ARGS__)
function_signature(dictionary_t, __dictionary_create, u32 key_size, u32 value_size, u64 capacity, bool (*key_comparer)(void* compare_key, void* key));
#define dictionary_free(...) define_alias_function_macro(dictionary_free, __VA_ARGS__)
function_signature(void, dictionary_free, dictionary_t* dictionary);

// getters
#define dictionary_get_at(...) define_alias_function_macro(dictionary_get_at, __VA_ARGS__)
function_signature(void, dictionary_get_at, dictionary_t* dictionary, u64 index, void* out_key, void* out_value);
#define dictionary_get_value(...) define_alias_function_macro(dictionary_get_value, __VA_ARGS__)
function_signature(void, dictionary_get_value, dictionary_t* dictionary, void* key, void* out_value);
#define dictionary_get_value_at(...) define_alias_function_macro(dictionary_get_value_at, __VA_ARGS__)
function_signature(void, dictionary_get_value_at, dictionary_t* dictionary, u64 index, void* out_value);
#define dictionary_get_value_ptr_at(...) define_alias_function_macro(dictionary_get_value_at, __VA_ARGS__)
function_signature(void*, dictionary_get_value_ptr_at, dictionary_t* dictionary, u64 index);
#define dictionary_get_value_ptr(...) define_alias_function_macro(dictionary_get_value, __VA_ARGS__)
function_signature(void*, dictionary_get_value_ptr, dictionary_t* dictionary, void* key);
#define dictionary_get_count(...) define_alias_function_macro(dictionary_get_count, __VA_ARGS__)
function_signature(u64, dictionary_get_count, dictionary_t* dictionary);
#define dictionary_get_capacity(...) define_alias_function_macro(dictionary_get_capacity, __VA_ARGS__)
function_signature(u64, dictionary_get_capacity, dictionary_t* dictionary);


// setters
#define dictionary_set_at(...) define_alias_function_macro(dictionary_set_at, __VA_ARGS__)
function_signature(void, dictionary_set_at, dictionary_t* dictionary, u64 index, void* in_key, void* in_value);
#define dictionary_set_value(...) define_alias_function_macro(dictionary_set_value, __VA_ARGS__)
function_signature(void, dictionary_set_value, dictionary_t* dictionary, void* key, void* in_value);
#define dictionary_set_value_at(...) define_alias_function_macro(dictionary_set_value_at, __VA_ARGS__)
function_signature(void, dictionary_set_value_at, dictionary_t* dictionary, u64 index, void* in_value);

// logic functions
#define dictionary_add(...) dictionary_push(__VA_ARGS__)
#define dictionary_push(...) define_alias_function_macro(dictionary_push, __VA_ARGS__)
function_signature(void, dictionary_push, dictionary_t* dictionary, void* in_key, void* in_value);
#define dictionary_pop(...) define_alias_function_macro(dictionary_pop, __VA_ARGS__)
function_signature(void, dictionary_pop, dictionary_t* dictionary, void* out_key, void* out_value);
#define dictionary_remove(...) define_alias_function_macro(dictionary_remove, __VA_ARGS__)
function_signature(void, dictionary_remove, dictionary_t* dictionary, void* key);
#define dictionary_contains(...) define_alias_function_macro(dictionary_contains, __VA_ARGS__)
function_signature(bool, dictionary_contains, dictionary_t* dictionary, void* key);
#define dictionary_clear(...) define_alias_function_macro(dictionary_clear, __VA_ARGS__)
function_signature(void, dictionary_clear, dictionary_t* dictionary);
#define dictionary_find_index_of(...) define_alias_function_macro(dictionary_find_index_of, __VA_ARGS__)
function_signature(u64, dictionary_find_index_of, dictionary_t* dictionary, void* key);
