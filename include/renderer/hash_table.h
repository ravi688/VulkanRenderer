#pragma once

#include <renderer/defines.h>
#include <renderer/comparer.h> // comparer_t
#include <renderer/hash_function.h> // hash_function_t
#include <renderer/multi_buffer.h> 	// multi_buffer_t

typedef buffer_t /* sub_buffer_handle_t */ bucket_handle_list_t;

typedef struct hash_table_t
{
	/* size of the key in bytes */
	u32 key_size;
	/* size of the value in bytes */
	u32 value_size;
	/* capacity of this hash table */
	u32 capacity;
	/* hash function to calculate hash of a key */
	hash_function_t get_hash;
	/* key comparer to compare a pair of keys for equality */
	comparer_t is_equal;
	/* (it's special data structure to store the buckets into one continguous memory block) */
	multi_buffer_t buffer;
	/* list of sub */
	bucket_handle_list_t bucket_handles;
} hash_table_t;

BEGIN_CPP_COMPATIBLE

/* constructor and destructors */
#define hash_table_create(Tkey, Tvalue, capacity, key_comparer, key_hash_function) __hash_table_create(sizeof(Tkey), sizeof(Tvalue), capacity, key_comparer, key_hash_function)
RENDERER_API hash_table_t __hash_table_create(u32 key_size, u32 value_size, u32 capacity, comparer_t key_comparer, hash_function_t key_hash_function);
RENDERER_API void hash_table_free(hash_table_t* table);

/* clears the hash table and ready to be used again */
RENDERER_API void hash_table_clear(hash_table_t* table);
/* adds a key value pair */
RENDERER_API void hash_table_add(hash_table_t* table, void* key, void* value);
/* removes a key value pair, returns true if success otherwise false (not found) */
RENDERER_API bool hash_table_remove(hash_table_t* table, void* key);
/* returns true if the hash table contains an entry with key 'key' otherwise false */
RENDERER_API bool hash_table_contains(hash_table_t* table, void* key);
/* returns the number of key value pairs in the hash table */
RENDERER_API u32 hash_table_get_count(hash_table_t* table);
/* returns pointer to the value by it's key */
RENDERER_API void* hash_table_get_value(hash_table_t* table, void* key);
/* visits each key value pairs in the hash table */
RENDERER_API void hash_table_foreach(hash_table_t* table, void (*visitor)(void* key, void* value, void* user_data), void* user_data);
/* visits each key value pairs in the hash table until visitor returns false */
RENDERER_API void hash_table_foreach_until(hash_table_t* table, bool (*visitor)(void* key, void* value, void* user_data), void* user_data);

END_CPP_COMPATIBLE
