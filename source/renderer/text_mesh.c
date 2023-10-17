#include <renderer/text_mesh.h>
#include <renderer/internal/vulkan/vulkan_text_mesh.h>
#include <renderer/renderer.h>

// constructors and destructors
RENDERER_API text_mesh_t* text_mesh_new(memory_allocator_t* allocator)
{
    return vulkan_text_mesh_new(allocator);
}

RENDERER_API text_mesh_t* text_mesh_create(renderer_t* renderer, glyph_mesh_pool_t* pool)
{
    return vulkan_text_mesh_create(VULKAN_RENDERER(renderer), pool);
}

RENDERER_API void text_mesh_destroy(text_mesh_t* text)
{
    vulkan_text_mesh_destroy(text);
}

RENDERER_API void text_mesh_release_resources(text_mesh_t* text)
{
    vulkan_text_mesh_release_resources(text);
}


// logic functions
RENDERER_API void text_mesh_draw(text_mesh_t* text)
{
    vulkan_text_mesh_draw(text);
}

// constructors and destructors
RENDERER_API text_mesh_string_handle_t text_mesh_string_create(text_mesh_t* text_mesh)
{
    return vulkan_text_mesh_string_create(text_mesh);
}

RENDERER_API void text_mesh_string_destroyH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    vulkan_text_mesh_string_destroyH(text_mesh, handle);
}


// setters
RENDERER_API void text_mesh_set_point_size(text_mesh_t* text, u32 point_size)
{
    vulkan_text_mesh_set_point_size(text, point_size);
}

RENDERER_API void text_mesh_set_material(text_mesh_t* text, material_t* material)
{
    vulkan_text_mesh_set_material(text, material);
}

RENDERER_API void text_mesh_set_render_space_type(text_mesh_t* text, text_mesh_render_space_type_t space_type)
{
    vulkan_text_mesh_set_render_space_type(text, space_type);
}

RENDERER_API void text_mesh_set_render_surface_type(text_mesh_t* text, text_mesh_render_surface_type_t surface_type)
{
    vulkan_text_mesh_set_render_surface_type(text, surface_type);
}

RENDERER_API void text_mesh_string_setH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, const char* string)
{
    vulkan_text_mesh_string_setH(text_mesh, handle, string);
}

RENDERER_API void text_mesh_string_set_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle, u32 point_size)
{
    vulkan_text_mesh_string_set_point_sizeH(text, handle, point_size);
}

RENDERER_API void text_mesh_string_set_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, mat4_t transform)
{
    vulkan_text_mesh_string_set_transformH(text_mesh, handle, transform);
}


// getters
RENDERER_API u32 text_mesh_get_point_size(text_mesh_t* text)
{
    return vulkan_text_mesh_get_point_size(text);
}

RENDERER_API const char* text_mesh_string_getH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_getH(text_mesh, handle);
}

RENDERER_API u32 text_mesh_string_get_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_get_point_sizeH(text, handle);
}

RENDERER_API mat4_t text_mesh_string_get_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_get_transformH(text_mesh, handle);
}
