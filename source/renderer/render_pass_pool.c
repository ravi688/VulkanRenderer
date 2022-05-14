
#include <renderer/render_pass_pool.h>
#include <renderer/memory_allocator.h>

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new()
{
	render_pass_pool_t* pool = heap_new(render_pass_pool_t);
	memset(pool, 0, sizeof(render_pass_pool_t));
	return pool;
}

RENDERER_API render_pass_pool_t* render_pass_pool_create(render_pass_hash_function_t hash_function)
{
	render_pass_pool_t* pool = render_pass_pool_new();
	pool->get_hash = hash_function;
	pool->relocation_table = buf_create(sizeof(render_pass_handle_t), 1, 0);
	pool->slots = buf_create(sizeof(render_pass_pool_slot_t), 1, 0);
	return pool;
}

RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		render_pass_pool_slot_t* slot = buf_get_ptr_at(&pool->slots, i);
		render_pass_destroy(slot->render_pass);
	}
}

RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		render_pass_pool_slot_t* slot = buf_get_ptr_at(&pool->slots, i);
		render_pass_release_resources(slot->render_pass);
	}
	buf_free(&pool->relocation_table);
	buf_free(&pool->slots);
	heap_free(pool);
}

/* logic functions */
static bool hash_compare(void* pHash, void* ref)
{
	return DEREF_TO(pHash, render_pass_hash_t) == CAST_TO(slot, render_pass_pool_slot_t*)->hash;
}

static render_pass_handle_t render_pass_pool_create_slot(render_pass_pool_t* pool, render_pass_hash_t hash, const char* name)
{
	if(buf_find_index_of(&pool->slots, &hash, hash_compare) == BUF_INVALID_INDEX)
	{
		// generate a new handle
		buf_ucount_t handle = buf_get_element_count(&pool->relocation_table);
		buf_ucount_t index = buf_get_element_count(&pool->slots);
		buf_push(&pool->relocation_table, &index);

		render_pass_pool_slot_t slot = 
		{
			.hash = hash,
			.handle = handle,
			.name = string_create(name)
		};
		buf_push(&pool->slots, &slot);
	}
	return RENDER_PASS_HANDLE_INVALID;
}

RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, void* api_specific, const char* name)
{
	render_pass_handle_t handle = render_pass_pool_create_slot(pool, pool->get_hash(api_specific), name);
	if(handle == RENDER_PASS_HANDLE_INVALID)
		return handle;
	render_pass_pool_slot_t* slot = render_pass_pool_get_slotH(pool, handle);
	slot->render_pass = render_pass_create(pool->context, api_specific);
	return handle;
}

RENDERER_API render_pass_handle_t render_pass_pool_add(render_pass_pool_t* pool, render_pass_t* pass, render_pass_hash_t hash, const char* name)
{
	render_pass_handle_t handle = render_pass_pool_create_slot(pool, hash, name);	
	if(handle == RENDER_PASS_HANDLE_INVALID)
		return handle;
	render_pass_pool_slot_t* slot = render_pass_pool_get_slotH(pool, handle);
	slot->render_pass = pass;
	return handle;
}

static render_pass_pool_slot_t* render_pass_pool_get_slot(render_pass_handle_t* pool, const char* name);

RENDERER_API render_pass_pool_handle_t render_pass_pool_get_handle(render_pass_pool_t* pool, const char* name)
{
	render_pass_pool_slot_t* slot = render_pass_pool_get_slot(pool, name);
	return (slot == NULL) ? RENDER_PASS_HANDLE_INVALID : slot->handle;
}

static render_pass_pool_slot_t* render_pass_pool_get_slot(render_pass_handle_t* pool, const char* name)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		render_pass_pool_slot_t* slot = buf_get_ptr_at(&pool->slots, i);
		if(strcmp(slot->name.data, name) == 0)
			return slot;
	}
	return NULL;
}

static render_pass_pool_slot_t* render_pass_pool_get_slotH(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	render_pass_handle_t index;
	buf_get_at(&pool->relocation_table, handle, &index);
	return buf_get_ptr_at(&pool->slots, &index);
}

RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	render_pass_pool_slot_t* slot = render_pass_pool_get_slotH(pool, handle);
	return (slot == NULL) ? NULL : slot->render_pass;
}

RENDERER_API render_pass_t* render_pass_pool_get(render_pass_pool_t* pool, const char* name)
{
	render_pass_pool_slot_t* slot = render_pass_pool_get_slot(pool, name);
	return (slot == NULL) ? NULL : slot->render_pass;
}
