/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: hash_table.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

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

#pragma once

#include <sge/defines.h>
#include <sge/comparer.h> // comparer_t
#include <sge/hash_function.h> // hash_function_t
#include <sge/multi_buffer.h> 	// multi_buffer_t

typedef struct memory_allocator_t memory_allocator_t;
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

typedef hash_table_t* hash_table_ptr_t;

BEGIN_CPP_COMPATIBLE

/* constructor and destructors */
#define hash_table_create(allocator, Tkey, Tvalue, capacity, key_comparer, key_hash_function) __hash_table_create(allocator, sizeof(Tkey), sizeof(Tvalue), capacity, key_comparer, key_hash_function)
SGE_API hash_table_t __hash_table_create(memory_allocator_t* allocator, u32 key_size, u32 value_size, u32 capacity, comparer_t key_comparer, hash_function_t key_hash_function);
SGE_API void hash_table_free(hash_table_t* table);

/* clears the hash table and ready to be used again */
SGE_API void hash_table_clear(hash_table_t* table);
/* adds a key value pair and returns pointer to the value */
SGE_API void* hash_table_add_get(hash_table_t* table, void* key, void* value);
/* adds a key value pair */
SGE_API void hash_table_add(hash_table_t* table, void* key, void* value);
/* removes a key value pair, returns true if success otherwise false (not found) */
SGE_API bool hash_table_remove(hash_table_t* table, void* key);
/* returns true if the hash table contains an entry with key 'key' otherwise false */
SGE_API bool hash_table_contains(hash_table_t* table, void* key);
/* returns the number of key value pairs in the hash table */
SGE_API u32 hash_table_get_count(hash_table_t* table);
/* returns pointer to the value by it's key, NULL if the key doesn't exists */
SGE_API void* hash_table_get_value(hash_table_t* table, void* key);
/* visits each key value pairs in the hash table */
SGE_API void hash_table_foreach(hash_table_t* table, void (*visitor)(void* key, void* value, void* user_data), void* user_data);
/* visits each key value pairs in the hash table until visitor returns false */
SGE_API void hash_table_foreach_until(hash_table_t* table, bool (*visitor)(void* key, void* value, void* user_data), void* user_data);

END_CPP_COMPATIBLE
