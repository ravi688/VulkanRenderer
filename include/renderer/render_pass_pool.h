
#pragma once

#include <renderer/defines.h>
#include <renderer/string.h>
#include <bufferlib/buffer.h>

#include <renderer/render_pass.h>

typedef buf_ucount_t render_pass_handle_t;
typedef u64 render_pass_hash_t;
typedef render_pass_hash_t (*render_pass_hash_function_t)(void* api_specific);

typedef struct render_pass_pool_slot_t
{
	string_t name;
	render_pass_hash_t hash;
	render_pass_t* render_pass;
} render_pass_pool_slot_t;

typedef struct render_pass_pool_t
{
	render_pass_hash_function_t get_hash;	// hash function
	BUFFER relocation_table; 				// relocation table
	BUFFER slots; 							// list of render_pass_pool_slot_t
} render_pass_pool_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new();
RENDERER_API render_pass_pool_t* render_pass_pool_create(render_pass_hash_function_t hash_function);
RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool);
RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool);

/* logic functions */
RENDERER_API render_pass_handle_t render_pass_create_pass(render_pass_pool_t* pool, void* api_specific, const char* name);
RENDERER_API render_pass_handle_t render_pass_add(render_pass_pool_t* pool, render_pass_t* pass, render_pass_hash_t hash, const char* name);
RENDERER_API render_pass_handle_t render_pass_get_handle(render_pass_pool_t* pool, const char* name);
RENDERER_API const char* render_pass_get_nameH(render_pass_pool_t* pool, render_pass_handle_t handle);
RENDERER_API render_pass_t* render_pass_get(render_pass_pool_t* pool, const char* name);
RENDERER_API render_pass_t* render_pass_getH(render_pass_pool_t* pool, render_pass_handle_t handle);


END_CPP_COMPATIBLE
