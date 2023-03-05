
/*
	CAMERA
	---------------------------

	Camera is responsible for capturing photos (frames) of the scene in which they are placed.

	A camera knows or at least allows to configure the following things:
		1. How wide we can capture (field of view)
		2. How far we can capture (far clip plane)
		3. How near we can capture (near clip plane)
		4. Where to render (render target)

 */

#pragma once

#include <renderer/defines.h>
#include <renderer/color.h>
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_handles.h> 	// vulkan_render_pass_handle_t
#include <renderer/internal/vulkan/vulkan_types.h> 		// vulkan_render_target_render_technique_t
#include <renderer/internal/vulkan/vulkan_descriptor_set.h> 	// vulkan_descriptor_set_t
#include <renderer/event.h>

#include <hpml/vec3.h>
#include <hpml/vec2.h>
#include <hpml/mat4.h>

typedef enum vulkan_camera_projection_type_t
{
	VULKAN_CAMERA_PROJECTION_TYPE_UNDEFINED,
	VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
	VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE,
	VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC
} vulkan_camera_projection_type_t;

#define VULKAN_CAMERA_RENDER_TARGET_SCREEN CAST_TO(vulkan_texture_t*, NULL)

typedef enum vulkan_camera_render_target_type_t
{
	VULKAN_CAMERA_RENDER_TARGET_TYPE_UNDEFINED = 0,
	VULKAN_CAMERA_RENDER_TARGET_TYPE_COLOR,
	VULKAN_CAMERA_RENDER_TARGET_TYPE_DEPTH
} vulkan_camera_render_target_type_t;

/* strucutre to hold camera create information */
typedef struct vulkan_camera_create_info_t
{
	/* projection type of the camera */
	vulkan_camera_projection_type_t projection_type;
	/* default render pass handle (to be used for clearing the screen or if there are no objects in the scene to render) */
	vulkan_render_pass_handle_t default_render_pass;
} vulkan_camera_create_info_t;

typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_render_scene_t vulkan_render_scene_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_framebuffer_t vulkan_framebuffer_t;

typedef struct vulkan_camera_t
{
	/* pointer to the vulkan api context */
	vulkan_renderer_t* renderer;

	/* clears values for the default render pass (for each attachment) */
	VkClearValue* clear_buffer;

	/* transform informations for each shot position (faces of the cube, otherwise just one face (2d)) */
	vulkan_buffer_t buffers[6];
	vulkan_descriptor_set_t sets[6];
	void* buffer_mappings[6];
	/* number of transform informations (1 if render target is not cube, otherwise 6) */
	union
	{
		u32 buffer_count;
		u32 set_count;
	};

	/* pointer to the default render pass object (created internally using the default render pass handle passed in create info struct) */
	vulkan_render_pass_t* default_render_pass;

	/* current color render target texture (NULL if the target is the swapchain color attachment (screen)) */
	vulkan_texture_t* color_render_target;
	/* current depth render target texture (NULL if the target is the backed depth attachment */
	vulkan_texture_t* depth_render_target;

	/* 2 dimensional jagged array of vulkan_framebuffer_t objects 
	 * dimensions: [max_shot_count][sum:{render_pass[i].required_framebuffer_count}] */
	BUFFER framebuffers;

	/* renderable area of the render target to which render the output */
	struct
	{
		struct
		{
			/* offset in the x axis (for now it is ignored) */
			u32 x;
			/* offset in the y axis (for now it is ignored) */
			u32 y;
		} offset;
		struct
		{
			/* width of the render area (starts from the offset.x and ends at (offset.x + width)) */
			u32 width;
			/* height of the render area (starts from the offset.y and ends at (offset.y + height)) */
			u32 height;
		} extent;
	} render_area;

	/* maximum number of shot that can be taken in this frame; 1 if current render target is not cube, otherwise 6 */
	u32 max_shot_count;
	/* index of the current shot being taken */
	u32 current_shot_index;
	/* number of shot taken in this frame */
	u32 shot_taken;

	/* 	uniform CameraInfo 
	   	{
			mat4 transform;
			mat4 projection;
			mat4 view;
			mat4 screen; 
	  	} 
	*/
	struct_descriptor_t struct_definition;
	struct_field_handle_t transform_handle;
	struct_field_handle_t projection_handle;
	struct_field_handle_t view_handle;
	struct_field_handle_t screen_handle;

	/* true, if this camera is active, otherwise false */
	bool is_active;
	/* projection type for this camera */
	vulkan_camera_projection_type_t projection_type;
	/* distance of far clip plane from this camera */
	float far_clip_plane;
	/* distance of near clip plane from this camera */
	float near_clip_plane;
	/* height: (for orthographic projection)
	 * field of view: (for perspective projection) in radians */
	union
	{
		float height;
		float field_of_view;
	};

	/* position of this camera in 3D space */
	vec3_t position;
	/* euler angle rotation of this camera in 3D space */
	vec3_t rotation;
	/* transform matrix for this camera (can be calculated from the above data (position and rotation)) */
	mat4_t transform;
	/* view matrix for this camera (inverse of the transform matrix) */
	mat4_t view;
	/* projection matrix for this camera */
	mat4_t projection;
	/* UI projection matrix for this camera (useful for rendering UI) */
	mat4_t screen;

	/* euler angle rotation for each shot (towards each face of the cube) 
	 * it is NULL initially, but once render target is set to a valid render texture then it's size becomes 6 */
	vec3_t* shot_rotations;

	/* even subscription handles */
	event_subscription_handle_t projection_recreate_handle;
	event_subscription_handle_t screen_projection_recreate_handle;
	event_subscription_handle_t framebuffers_recreate_handle;
	event_subscription_handle_t default_render_pass_refresh_handle;

} vulkan_camera_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_camera_t* vulkan_camera_new(memory_allocator_t* allocator);
RENDERER_API vulkan_camera_t* vulkan_camera_create(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info);
RENDERER_API void vulkan_camera_create_no_alloc(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info, vulkan_camera_t OUT camera);
RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera);
RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera);

/* logic functions */
#define VULKAN_CAMERA_CLEAR_FLAG_DONT_CARE 1
#define VULKAN_CAMERA_CLEAR_FLAG_CLEAR 0
RENDERER_API void vulkan_camera_begin(vulkan_camera_t* camera);
RENDERER_API void vulkan_camera_end(vulkan_camera_t* camera);
RENDERER_API bool vulkan_camera_capture(vulkan_camera_t* camera, u32 clear_flags);
RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth);
RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_texture_t* texture);
RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene, u64 queue_mask);
RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* texture);

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_framebuffer_list_handle_t handle);
RENDERER_API vulkan_framebuffer_list_handle_t vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* pass);


/* getters */
static FORCE_INLINE bool vulkan_camera_is_offscreen(vulkan_camera_t* camera)
{
	return (camera->color_render_target != NULL) || (camera->depth_render_target != NULL);
}
static FORCE_INLINE bool vulkan_camera_is_active(vulkan_camera_t* camera)
{
	return camera->is_active;
}
RENDERER_API mat4_t vulkan_camera_get_view(vulkan_camera_t* camera);
RENDERER_API mat4_t vulkan_camera_get_transform(vulkan_camera_t* camera);
RENDERER_API mat4_t vulkan_camera_get_projection(vulkan_camera_t* camera);
RENDERER_API vec3_t vulkan_camera_get_position(vulkan_camera_t* camera);
RENDERER_API vec3_t vulkan_camera_get_rotation(vulkan_camera_t* camera);
RENDERER_API vec2_t vulkan_camera_get_clip_planes(vulkan_camera_t* camera);
static FORCE_INLINE float vulkan_camera_get_near_clip_plane(vulkan_camera_t* camera) { return vulkan_camera_get_clip_planes(camera).x; }
static FORCE_INLINE float vulkan_camera_get_far_clip_plane(vulkan_camera_t* camera) { return vulkan_camera_get_clip_planes(camera).y; }
RENDERER_API float vulkan_camera_get_field_of_view(vulkan_camera_t* camera);
static FORCE_INLINE float vulkan_camera_get_height(vulkan_camera_t* camera) { return vulkan_camera_get_field_of_view(camera); }

/* setters */
static FORCE_INLINE void vulkan_camera_set_active(vulkan_camera_t* camera, bool is_active)
{
	camera->is_active = is_active;
}
RENDERER_API void vulkan_camera_set_transform(vulkan_camera_t* camera, mat4_t transform);
RENDERER_API void vulkan_camera_set_position_cube(vulkan_camera_t* camera, vec3_t position);
RENDERER_API void vulkan_camera_set_position(vulkan_camera_t* camera, vec3_t position);
RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t rotation);
RENDERER_API void vulkan_camera_set_clip_planes(vulkan_camera_t* camera, float near_clip_plane, float far_clip_plane);
RENDERER_API void vulkan_camera_set_field_of_view(vulkan_camera_t* camera, float fov);
static FORCE_INLINE void vulkan_camera_set_height(vulkan_camera_t* camera, float height) { vulkan_camera_set_field_of_view(camera, height); }

END_CPP_COMPATIBLE
