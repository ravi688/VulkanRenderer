
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/string.h>

#include <renderer/internal/vulkan/vulkan_handles.h> // vulkan_material_handle_t, vulkan_shader_handle_t

typedef struct vulkan_shader_library_t vulkan_shader_library_t;
typedef struct vulkan_material_t vulkan_material_t;

typedef struct vulkan_material_library_slot_t
{
	string_t name;
	vulkan_material_t* material;
	vulkan_material_handle_t handle;
} vulkan_material_library_slot_t;

typedef struct vulkan_material_library_t
{
	vulkan_renderer_t* renderer;
	vulkan_shader_library_t* shader_library;
	BUFFER relocation_table;
	BUFFER materials;
} vulkan_material_library_t;

/* constructors & destructors */
RENDERER_API vulkan_material_library_t* vulkan_material_library_new();
RENDERER_API vulkan_material_library_t* vulkan_material_library_create(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library);
RENDERER_API void vulkan_material_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library, vulkan_material_library_t OUT library);
RENDERER_API void vulkan_material_library_destroy(vulkan_material_library_t* library);
RENDERER_API void vulkan_material_library_release_resources(vulkan_material_library_t* library);

/* logic functions */
RENDERER_API vulkan_material_handle_t vulkan_material_library_create_material(vulkan_material_library_t* library, const char* shader_name, const char* material_name);
RENDERER_API vulkan_material_handle_t vulkan_material_library_create_materialH(vulkan_material_library_t* library, vulkan_shader_handle_t handle, const char* material_name);
RENDERER_API vulkan_material_handle_t vulkan_material_library_load_material(vulkan_material_library_t* library, const char* file_path);
RENDERER_API bool vulkan_material_library_destroy_material(vulkan_material_library_t* library, const char* material_name);
RENDERER_API bool vulkan_material_library_destroy_materialH(vulkan_material_library_t* library, vulkan_material_handle_t handle);
RENDERER_API BUFFER* vulkan_material_library_serialize(vulkan_material_library_t* library);
RENDERER_API vulkan_material_library_t* vulkan_material_library_deserialize(void* bytes, u64 length);
RENDERER_API bool vulkan_material_library_deserialize_no_alloc(void* bytes, u64 length, vulkan_material_library_t* out_library);

/* getters */
RENDERER_API vulkan_material_handle_t vulkan_material_library_get_handle(vulkan_material_library_t* library, const char* material_name);
RENDERER_API vulkan_material_t* vulkan_material_library_get(vulkan_material_library_t* library, const char* material_name);
RENDERER_API vulkan_material_t* vulkan_material_library_getH(vulkan_material_library_t* library, vulkan_material_handle_t handle);
