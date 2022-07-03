
#pragma once

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_camera.h>
	typedef vulkan_camera_t camera_t;
	typedef vulkan_render_scene_t render_scene_t;
	typedef vulkan_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN VULKAN_CAMERA_RENDER_TARGET_SCREEN
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_camera.h>
	typedef opengl_camera_t camera_t;
	typedef opengl_render_scene_t render_scene_t;
	typedef opengl_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN OPENGL_CAMERA_RENDER_TARGET_SCREEN
#elif RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_camera.h>
	typedef directx_camera_t camera_t;
	typedef directx_render_scene_t render_scene_t;
	typedef directx_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN DIRECTX_CAMERA_RENDER_TARGET_SCREEN
#elif RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_camera.h>
	typedef metal_camera_t camera_t;
	typedef metal_render_scene_t render_scene_t;
	typedef metal_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN METAL_CAMERA_RENDER_TARGET_SCREEN
#endif

#include <renderer/defines.h>
#include <renderer/color.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>

#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>

#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

/* NOTE: should be in sync with vulkan_camera_projection_type_t */
typedef enum camera_projection_type_t
{
	CAMERA_PROJECTION_TYPE_UNDEFINED,
	CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
	CAMERA_PROJECTION_TYPE_PERSPECTIVE,
	CAMERA_PROJECTION_TYPE_STEROGRAPHIC
} camera_projection_type_t;

typedef enum camera_render_target_type_t
{
	CAMERA_RENDER_TARGET_TYPE_UNDEFINED = 0,
	CAMERA_RENDER_TARGET_TYPE_COLOR,
	CAMERA_RENDER_TARGET_TYPE_DEPTH
} camera_render_target_type_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API camera_t* camera_new();
RENDERER_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type);
RENDERER_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, camera_t OUT camera);
RENDERER_API void camera_destroy(camera_t* camera);
RENDERER_API void camera_release_resources(camera_t* camera);

/* logic functions */
RENDERER_API void camera_set_clear(camera_t* camera, color_t color, float depth);
RENDERER_API void camera_set_active(camera_t* camera, bool is_active);
RENDERER_API void camera_set_render_target(camera_t* camera, camera_render_target_type_t target_type, texture_t* texture);
RENDERER_API bool camera_is_active(camera_t* camera);
RENDERER_API void camera_render(camera_t* camera, render_scene_t* scene, u64 queue_mask);
RENDERER_API void camera_render_to_texture(camera_t* camera, render_scene_t* scene, texture_t* texture);

/* getters */
RENDERER_API mat4_t(float) camera_get_view(camera_t* camera);
RENDERER_API mat4_t(float) camera_get_transform(camera_t* camera);
RENDERER_API mat4_t(float) camera_get_projection(camera_t* camera);
RENDERER_API vec3_t(float) camera_get_position(camera_t* camera);
RENDERER_API vec3_t(float) camera_get_rotation(camera_t* camera);
RENDERER_API vec2_t(float) camera_get_clip_planes(camera_t* camera);
RENDERER_API float camera_get_near_clip_plane(camera_t* camera);
RENDERER_API float camera_get_far_clip_plane(camera_t* camera);
RENDERER_API float camera_get_field_of_view(camera_t* camera);
RENDERER_API float camera_get_height(camera_t* camera);

/* setters */
RENDERER_API void camera_set_transform(camera_t* camera, mat4_t(float) transform);
RENDERER_API void camera_set_position(camera_t* camera, vec3_t(float) position);
RENDERER_API void camera_set_rotation(camera_t* camera, vec3_t(float) rotation);
RENDERER_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane);
RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov);
RENDERER_API void camera_set_height(camera_t* camera, float height);


END_CPP_COMPATIBLE