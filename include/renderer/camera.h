
#pragma once

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;
	typedef struct vulkan_render_queue_t vulkan_render_queue_t;
	typedef vulkan_render_queue_t render_queue_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;
	typedef struct opengl_render_queue_t opengl_render_queue_t;
	typedef opengl_render_queue_t render_queue_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;
	typedef struct directx_render_queue_t directx_render_queue_t;
	typedef directx_render_queue_t render_queue_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;
	typedef struct metal_render_queue_t metal_render_queue_t;
	typedef metal_render_queue_t render_queue_t;
#endif

#include <renderer/defines.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>

#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>

/* NOTE: should be in sync with vulkan_camera_projection_type_t */
typedef enum camera_projection_type_t
{
	CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
	CAMERA_PROJECTION_TYPE_PERSPECTIVE,
	CAMERA_PROJECTION_TYPE_STEROGRAPHIC
} camera_projection_type_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API camera_t* camera_new();
RENDERER_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type, float n_plane, float f_plane, float height_or_angle);
RENDERER_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, float n_plane, float f_plane, float height_or_angle, camera_t OUT camera);
RENDERER_API void camera_destroy(camera_t* camera);
RENDERER_API void camera_release_resources(camera_t* camera);

/* logic functions */
RENDERER_API void camera_render(camera_t* camera, render_queue_t* queue);

/* getters */
RENDERER_API vec3_t(float) camera_get_position(camera_t* camera);
RENDERER_API vec3_t(float) camera_get_rotation(camera_t* camera);
RENDERER_API vec2_t(float) camera_get_clip_planes(camera_t* camera);
RENDERER_API float camera_get_near_clip_plane(camera_t* camera);
RENDERER_API float camera_get_far_clip_plane(camera_t* camera);
RENDERER_API float camera_get_field_of_view(camera_t* camera);
RENDERER_API float camera_get_height(camera_t* camera);

/* setters */
RENDERER_API void camera_set_position(camera_t* camera, vec3_t(float) position);
RENDERER_API void camera_set_rotation(camera_t* camera, vec3_t(float) rotation);
RENDERER_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane);
RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov);
RENDERER_API void camera_set_height(camera_t* camera, float height);


END_CPP_COMPATIBLE