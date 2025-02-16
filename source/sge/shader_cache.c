#include <sge/shader_cache.h>
#include <sge/memory_allocator.h>
#include <sge/assert.h>
#include <sge/debug.h>
#include <common/string.h> /* for com_safe_strncmp() */

static const char MAGIC_BYTES[] = "V3D Shader Cache File";
/* exclude the null character */
static const int MAGIC_BYTE_COUNT = SIZEOF_ARRAY(MAGIC_BYTES) - 1;

typedef struct data_t
{
	com_readonly_data_t data;
	u32 offset;
	u32 length;
	bool is_new;
} data_t;

SGE_API shader_cache_t* shader_cache_create(memory_allocator_t* allocator)
{
	shader_cache_t* cache = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_SHADER_CACHE, shader_cache_t);
	memzero(cache, shader_cache_t);
	cache->allocator = allocator;
	cache->memcache = hash_table_create(u64, data_t, 10, 10, u64_equal_to, u64_hash, NULL);
	return cache;
}

static void free_data_visitor(void* key_ptr, void* value_ptr, void* user_data)
{
	CAN_BE_UNUSED_VARIABLE AUTO cache = CAST_TO(shader_cache_t*, user_data);
	AUTO value = CAST_TO(data_t*, value_ptr);
	if(value->data.bytes)
	{
		memory_allocator_dealloc(cache->allocator, CAST_TO(void*, value->data.bytes));
		value->data.bytes = NULL;
	}
}

SGE_API void shader_cache_destroy(shader_cache_t* cache)
{
	shader_cache_flush(cache, true);
	hash_table_foreach(&cache->memcache, free_data_visitor, cache);
	hash_table_free(&cache->memcache);
	memory_allocator_dealloc(cache->allocator, cache);
}

SGE_API void shader_cache_set_path(shader_cache_t* cache, const char* path)
{
	if(cache->file)
	{
		DEBUG_LOG_FETAL_ERROR("You can't call shader_cache_set_path multiple times, you may only call it once before any other operations");
		return;
	}
	_debug_assert__(path != NULL);
	if(strlen(path) > SHADER_CACHE_PATH_MAX_LENGTH)
		DEBUG_LOG_ERROR("Failed to set shader path as the length exceeds SHADER_CACHE_PATH_MAX_LENGTH(%lu)", SHADER_CACHE_PATH_MAX_LENGTH);
	else
	{
		strncpy(cache->shader_cache_path, path, SHADER_CACHE_PATH_MAX_LENGTH);
		cache->shader_cache_path[SHADER_CACHE_PATH_MAX_LENGTH] = 0;
	}
}

typedef struct cache_entry_t
{
	u64 key;
	u32 offset;
	u32 length;
} cache_entry_t;

typedef struct store_data_visitor_user_data_t
{
	shader_cache_t* cache;
	bool is_success;
} store_data_visitor_user_data_t;


static bool store_data_visitor(void* key_ptr, void* value_ptr, void* user_data)
{
	store_data_visitor_user_data_t* ud = CAST_TO(store_data_visitor_user_data_t*, user_data);
	data_t* data = CAST_TO(data_t*, value_ptr);

	if(fwrite(data->data.bytes, sizeof(u8), data->data.size, ud->cache->file) == data->data.size)
	{
		return true;
	} else DEBUG_LOG_ERROR("Failed to Write");
	ud->is_success = false;
	/* stop iterating */
	return false;
}

typedef struct populate_entry_user_data_t
{
	cache_entry_t* entries;
	u32 index;
	u32 offset;
} populate_entry_user_data_t;

static void populate_entry(void* key_ptr, void* value_ptr, void* user_data)
{
	AUTO ud = CAST_TO(populate_entry_user_data_t*, user_data);
	data_t* data = CAST_TO(data_t*, value_ptr);
	ud->entries[ud->index] = (cache_entry_t)
	{
		.key = DREF_TO(u64, key_ptr),
		.length = data->length
	};
	ud->entries[ud->index].offset += ud->offset;
	ud->index += 1;
	ud->offset += data->length;
}

static bool load_data(shader_cache_t* cache, u64 key, com_readonly_data_t* data, u32 offset, u32 length)
{
	if(cache->mode != 'r')
	{
		if(cache->file)
			fclose(cache->file);
		cache->file = fopen(cache->shader_cache_path, "rb");
		if(!cache->file)
		{
			DEBUG_LOG_ERROR("Failed to load cache data for key %llu, Couldn't open the shader cache file %s", key, cache->shader_cache_path);
			cache->mode = 0;
			cache->file = NULL;
			return false;
		}
	}

	_com_assert(cache->file != NULL);

	if(!fseek(cache->file, offset, SEEK_SET))
	{
		void* bytes = memory_allocator_alloc(cache->allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, length);
		if(fread(bytes, sizeof(char), length, cache->file) == length)
		{
			data->bytes = bytes;
			data->size = length;
			cache->mode = 'r';
			return true;
		}
		else
		{
		 	DEBUG_LOG_ERROR("Failed to read");
		 	memory_allocator_dealloc(cache->allocator, bytes);
		}
	}
	else DEBUG_LOG_ERROR("Failed to seek");
	cache->mode = 0;
	fclose(cache->file);
	/* next-time retry open */
	cache->file = NULL;
	return false;
}

typedef struct load_entry_data_user_data_t
{
	shader_cache_t* cache;
	bool is_success;
} load_entry_data_user_data_t;

static bool load_entry_data(void* key_ptr, void* value_ptr, void* user_data)
{
	AUTO ud = CAST_TO(load_entry_data_user_data_t*, user_data);
	data_t* data = CAST_TO(data_t*, user_data);
	if(!data->data.bytes)
	{
		_com_assert(!data->is_new);
		if(!load_data(ud->cache, DREF_TO(u64, key_ptr), &data->data, data->offset, data->length))
		{
			ud->is_success = false;
			return false;
		}
	}
	return true;
}

static void load_table(shader_cache_t* cache)
{
	if(cache->is_table_loaded)
		return;

	if((cache->mode != 'r') && cache->file)
		fclose(cache->file);

	FILE* file = fopen(cache->shader_cache_path, "rb");
	if(file)
	{
		char buffer[MAGIC_BYTE_COUNT];
		if(fread(buffer, sizeof(char), MAGIC_BYTE_COUNT, file) == MAGIC_BYTE_COUNT)
		{
			if(!com_safe_strncmp(buffer, MAGIC_BYTES, MAGIC_BYTE_COUNT))
			{
				u32 num_entries;
				if(fread(&num_entries, sizeof(u32), 1, file) == 1)
				{
					if(!num_entries)
						debug_log_warning("Number of entries is zero in the shader cache table");
					else
					{
						cache_entry_t entries[num_entries];
						if(fread(entries, sizeof(cache_entry_t), num_entries, file) == num_entries)
						{
							for(u32 i = 0; i < num_entries; ++i)
							{
								data_t data = 
								{ 
									.offset = entries[i].offset, 
									.length = entries[i].length,
									.is_new = false
								};
								hash_table_add(&cache->memcache, &entries[i].key, &data);
							}
							cache->file = file;
							cache->mode = 'r';
							cache->is_table_loaded = true;
							return;
						}
						else DEBUG_LOG_ERROR("Couldn't load shader cache table, Failed to read entries");
					}
				}
				else DEBUG_LOG_ERROR("Couldn't load shader cache table, Failed to read number of entries");
			}
			else DEBUG_LOG_ERROR("Couldn't load shader cache table, Corrupted shader cache file %s", cache->shader_cache_path);
		}
		else DEBUG_LOG_ERROR("Couldn't load shader cache table, Failed to read magic number");	
		fclose(file);
	}
	else DEBUG_LOG_ERROR("Couldn't load shader cache table, Failed to open shader cache file %s", cache->shader_cache_path);
	cache->mode = 0;
	/* next-time retry open */
	cache->file = NULL;
}

SGE_API void shader_cache_load_all(shader_cache_t* cache)
{
	load_table(cache);
	load_entry_data_user_data_t ud = 
	{
		.cache = cache,
		.is_success = true
	};
	hash_table_foreach_until(&cache->memcache, load_entry_data, &ud);
	if(!ud.is_success)
		DEBUG_LOG_ERROR("Failed to load all the entries'data");
}

SGE_API void shader_cache_flush(shader_cache_t* cache, bool is_close_file)
{
	shader_cache_load_all(cache);
	/*
		Format:

		1. MAGIC_BYTE_COUNT bytes of magic number
		2. 32-bit number encoding the number of entries in the cache
		3. number of entries X sizeof each entry
		4. Data
	*/
	if(cache->mode != 'e')
	{
		if(cache->file)
			fclose(cache->file);
		cache->file = fopen(cache->shader_cache_path, "w+b");
		if(!cache->file)
		{
			DEBUG_LOG_ERROR("Failed to flush shader cache, Couldn't open the shader cache file %s", cache->shader_cache_path);
			cache->mode = 0;
			cache->file = NULL;
			return;
		}
	}

	rewind(cache->file);
	if(fwrite(MAGIC_BYTES, sizeof(char), MAGIC_BYTE_COUNT, cache->file) == MAGIC_BYTE_COUNT)
	{
		u32 num_entries = hash_table_get_count(&cache->memcache);
		if(fwrite(&num_entries, sizeof(u32), 1, cache->file) == 1)
		{
			cache_entry_t entries[num_entries];
			populate_entry_user_data_t ud = 
			{
				.entries = entries,
				.index = 0,
				.offset = MAGIC_BYTE_COUNT + sizeof(u32) + sizeof(cache_entry_t) * num_entries
			};
			hash_table_foreach(&cache->memcache, populate_entry, &ud);
			if(fwrite(entries, sizeof(cache_entry_t), num_entries, cache->file) == num_entries)
			{
				store_data_visitor_user_data_t ud = 
				{
					.cache = cache,
					.is_success = true
				};
				hash_table_foreach_until(&cache->memcache, store_data_visitor, &ud);
				if(ud.is_success)
				{
					if(is_close_file)
						goto CLOSE_FILE;
				}
				else DEBUG_LOG_ERROR("Write error, Couldn't write entries'data");
			}
			else DEBUG_LOG_ERROR("Write error, Couldn't write entries");
		}
		else DEBUG_LOG_ERROR("Write error, Couldn't write number of entries");
	}
	else DEBUG_LOG_ERROR("Write error, Couldn't write magic number");

CLOSE_FILE:
	if(cache->file)
		fclose(cache->file);
	cache->mode = 0;
	cache->file = NULL;
}

static com_readonly_data_t copy_data(shader_cache_t* cache, com_immutable_data_t* in_data)
{
	return (com_readonly_data_t)
	{
		.bytes = memory_allocator_duplicate(cache->allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, in_data->bytes, in_data->size),
		.size = in_data->size
	};
}

SGE_API bool shader_cache_try_add(shader_cache_t* cache, u64 key, com_immutable_data_t in_data)
{
	load_table(cache);
	data_t* data = hash_table_get_value(&cache->memcache, &key);
	if(!data)
	{
		data_t d = 
		{
			.data = copy_data(cache, &in_data),
			.is_new = true
		};
		d.length = d.data.size;
		hash_table_add(&cache->memcache, &key, &d);
		return true;
	}
	return false;
}

SGE_API bool shader_cache_try_get(shader_cache_t* cache, u64 key, com_readonly_data_t OUT out_data)
{
	load_table(cache);
	data_t* data = hash_table_get_value(&cache->memcache, &key);
	if(data)
	{
		if(!data->data.bytes)
		{
			_com_assert(!data->is_new);
			if(!load_data(cache, key, &data->data, data->offset, data->length))
				return false;
		}
		out_data->bytes = data->data.bytes;
		out_data->size = data->data.size;
		return true;
	}
	return false;
}
