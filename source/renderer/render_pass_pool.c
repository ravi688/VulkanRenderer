
#include <renderer/render_pass_pool.h>
#include <renderer/memory_allocator.h>

RENDERER_API render_pass_pool_t* render_pass_pool_new()
{
	render_pass_pool_t* pool = heap_new(render_pass_pool_t);
	memset(pool, 0, sizeof(render_pass_pool_t));
	return pool;
}

RENDERER_API render_pass_pool_t* render_pass_pool_create(render_pass_comparer_t comparer)
{
	render_pass_pool_t* pool = render_pass_pool_new();
	pool->is_identical = comparer;
	pool->render_passes = buf_create(sizeof(render_pass_t), 1, 0);
	return pool;
}

RENDERER_API render_pass_handle_t render_pass_pool_add(render_pass_pool_t* pool, render_pass_t* pass)
{
	buf_ucount_t index = buf_find_index_of(&pool->render_passes, pass, pool->is_identical);
	if(index == BUF_INVALID_INDEX)
	{
		index = buf_get_element_count(&pool->render_passes);
		buf_push(&pool->render_passes, pass);
	}
	else
		LOG_MSG("Identical render pass already existing in the pool, skipping\n");
	return index;
}

RENDERER_API render_pass_t* render_pass_pool_get(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	return buf_get_ptr_at(&pool->render_passes, handle);
}

RENDERER_API void render_pass_pool_remove(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	/* For now, we must not remove any object from the pool,
		doing so will invalid some render pass handles
	*/
	LOG_FETAL_ERR("render_pass_pool_remove isn't implemented yet!\n");	
	// bool result = buf_remove(&pool->render_passes, pass, pool->is_identical);
	// if(!result)
	// 	LOG_WRN("You are trying to remove a render pass from the pool which doesn't exist in the pool\n");
}

RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool)
{
	buf_clear(&pool->render_passes, NULL);
}

RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool)
{
	buf_free(&pool->render_passes);
	heap_free(pool);
}

