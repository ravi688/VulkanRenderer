#pragma once

#include <sge/defines.h>
#include <common/hash_table.h>
#include <stdio.h> /* for file handling */

#define SHADER_CACHE_PATH_MAX_LENGTH 255

/* this can also be generalized and renamed as 'persistent cache' */
typedef struct shader_cache_t
{
	memory_allocator_t* allocator;
	char shader_cache_path[SHADER_CACHE_PATH_MAX_LENGTH + 1];
	hash_table_t memcache;
	FILE* file;
	char mode;
	bool is_table_loaded;
} shader_cache_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API shader_cache_t* shader_cache_create(memory_allocator_t* allocator);
SGE_API void shader_cache_destroy(shader_cache_t* cache);

/* sets the path internally,
 * NOTE: only allowed to call be called once 
 * however, can be called multiple times if non of shader_cache_try_add or shader_cache_try_get has been called. */
SGE_API void shader_cache_set_path(shader_cache_t* cache, const char* path);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const char* shader_cache_get_path(shader_cache_t* cache) { return cache->shader_cache_path; }

/* pre-loads all the shader data from the shader cache file */
SGE_API void shader_cache_load_all(shader_cache_t* cache);

/* flushes any new data to the disk 
 * is_close_file: if true then it also closes the file by calling fclose(). otherwise the file remains open. 
 * NOTE: it does nothing if all of the data (new entires) has been flushed by previous call to shader_cache_flush(), 
 * or no new data since been added. */
SGE_API void shader_cache_flush(shader_cache_t* cache, bool is_close_file);

/* adds a new entry if it doesn't already exists 
 * key: the key which should be unique
 * data: the data associated with the key, shader_cache_try_add creates a deep copy of the 'data' internally. 
 * returns: true if addition is successfull, false if the key already exists or something has gone wrong. */
SGE_API bool shader_cache_try_add(shader_cache_t* cache, u64 key, com_immutable_data_t data);
/* retrieves data associated with a key
 * key: the key to look for
 * data: output data containing pointer to memory buffer and its size in bytes. (NOTE: you can't delete/free the pointer as it is owned by this shader_cache class)
 * returns: true if retrieval is successfull, false if the key doesn't exists or something has gone wrong. */
SGE_API bool shader_cache_try_get(shader_cache_t* cache, u64 key, com_readonly_data_t OUT data);


END_CPP_COMPATIBLE
