#pragma once

#include <renderer/defines.h>
//
// #if   defined(RENDERER_VULKAN_DRIVER)
// 	#include <renderer/internal/vulkan/vulkan_text.h>
// 	typedef vulkan_text_t text_t;
// 	typedef vulkan_text_string_handle_t text_string_handle_t;
// 	typedef vulkan_material_t material_t;
// #elif defined(RENDERER_OPENGL_DRIVER)
// 	#include <renderer/internal/opengl/opengl_text.h>
// 	typedef opengl_text_t text_t;
// 	typedef opengl_text_string_handle_t text_string_handle_t;
// 	typedef opengl_material_t material_t;
// #elif defined(RENDERER_DIRECTX_DRIVER)
// 	#include <renderer/internal/directx/directx_text.h>
// 	typedef directx_text_t text_t;
// 	typedef directx_text_string_handle_t text_string_handle_t;
// 	typedef directx_material_t material_t;
// #elif defined(RENDERER_METAL_DRIVER)
// 	#include <renderer/internal/metal/metal_text.h>
// 	typedef metal_text_t text_t;
// 	typedef metal_text_string_handle_t text_string_handle_t;
// 	typedef metal_material_t material_t;
// #endif
//
// /* must be in-sync with vulkan_text_type_t */
// typedef enum text_type_t
// {
//     TEXT_TYPE_BITMAP,
//     TEXT_TYPE_MESH
// } text_type_t;
//
// /* must be in-sync with vulkan_text_render_space_type_t */
// typedef enum text_render_space_type_t
// {
//     TEXT_RENDER_SPACE_TYPE_2D,
//     TEXT_RENDER_SPACE_TYPE_3D
// } text_render_space_type_t;
//
// /* must be in-sync with vulkan_text_render_surface_type_t */
// typedef enum text_render_surface_type_t
// {
//     TEXT_RENDER_SURFACE_TYPE_CAMERA,
//     TEXT_RENDER_SURFACE_TYPE_SCREEN
// } text_render_surface_type_t;
//
//
// BEGIN_CPP_COMPATIBLE
//
// RENDERER_API text_t* text_new(memory_allocator* allocator, text_type_t type);
// RENDERER_API text_t* text_create(renderer_t* renderer, text_type_t type, void* stencil);
// RENDERER_API void text_create_no_alloc(renderer_t* renderer, text_type_t type, void* stencil, text_t OUT text);
// RENDERER_API void text_destroy(text_t* text);
// RENDERER_API void text_release_resources(text_t* text);
//
// RENDERER_API void text_draw(text_t* text);
//
// RENDERER_API text_string_handle_t text_string_create(text_t* text);
// RENDERER_API void text_string_destroyH(text_t* text, text_string_handle_t handle);
//
// /* setters */
// RENDERER_API void text_set_point_size(text_t* text, u32 point_size);
// RENDERER_API void text_set_material(text_t* text, vulkan_material_t* material);
// RENDERER_API void text_set_render_space_type(text_t* text, text_render_space_type_t space_type);
// RENDERER_API void text_set_render_surface_type(text_t* text, text_render_surface_type_t surface_type);
// RENDERER_API void text_string_setH(text_t* text, text_string_handle_t handle, const char* string);
// RENDERER_API void text_string_set_point_sizeH(text_t* text, text_string_handle_t handle, u32 point_size);
// RENDERER_API void text_string_set_transformH(text_t* text, text_string_handle_t handle, mat4_t transform);
//
// /* getters */
// RENDERER_API u32 text_get_point_size(text_t* text);
// RENDERER_API const char* text_string_getH(text_t* text, text_string_handle_t handle);
// RENDERER_API u32 text_string_get_point_sizeH(text_t* text, text_string_handle_t handle);
// RENDERER_API mat4_t text_string_get_transformH(text_t* text, text_string_handle_t handle);
// RENDERER_API font_t* text_get_font(text_t* text);
//
// END_CPP_COMPATIBLE
