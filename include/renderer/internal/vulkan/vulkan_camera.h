/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_camera.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

	
/*
	CAMERA
	---------------------------

	Camera is responsible for capturing photos (frames) of the scene in which they are placed.

	A camera knows or at least allows to configure the following things:
		1. How wide we can capture (field of view)
		2. How far we can capture (far clip plane)
		3. How near we can capture (near clip plane)
		4. Where to render (render target)
		5. How much the render target size is
 */

#pragma once

#include <renderer/defines.h>
#include <renderer/color.h>
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_handles.h> 	// vulkan_render_pass_handle_t
#include <renderer/internal/vulkan/vulkan_types.h> 		// vulkan_render_target_render_technique_t
#include <renderer/internal/vulkan/vulkan_descriptor_set.h> 	// vulkan_descriptor_set_t
#include <renderer/internal/vulkan/vulkan_framebuffer.h> 		// vulkan_framebuffer_t
#include <renderer/event.h>
#include <renderer/dictionary.h>

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
	vulkan_render_pass_handle_t swapchain_depth_clear_pass;
	vulkan_render_pass_handle_t depth_clear_pass;
	vulkan_render_pass_handle_t swapchain_clear_pass;
} vulkan_camera_create_info_t;

typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_render_scene_t vulkan_render_scene_t;
typedef struct vulkan_texture_t vulkan_texture_t;
/* list of vulkan_framebuffer_t objects */
typedef BUFFER /* vulkan_framebuffer_t */ vulkan_framebuffer_list_t;
typedef struct vulkan_attachment_t vulkan_attachment_t;
typedef struct vulkan_framebuffer_attachments_layout_description_t vulkan_framebuffer_attachments_layout_description_t;
typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;
typedef struct vulkan_material_t vulkan_material_t;

/* stores framebuffer objects and attachment objects for a render pass registered with this camera */
typedef struct vulkan_camera_render_pass_t
{
	/* handle to the actual render pass in the render pass pool */
	vulkan_render_pass_handle_t handle;
	/* list of framebuffers for this render pass */
	vulkan_framebuffer_list_t framebuffers;
	u32 valid_framebuffer_count;
	/* number of framebuffers for the one face of the cube 
	 * it is always equal to framebuffers.count / camera.max_shot_count */
	u32 framebuffer_count_in_one_face;
	/* list of allocated attachments for this render pass */
	vulkan_attachment_t** allocated_attachments;
	/* number of attachment objects for this render pass */
	u32 allocated_attachment_count;
	/* list of VkImageView objects contains the above attachments's internal image view objects and swapchain image views */
	VkImageView* vo_image_views;
	/* number of VkImageView objects in the above list */
	union
	{
		u32 image_view_count;
		/* must equal to vulkan_framebuffer_attachments_layout_description_t::attachment_description_count */
		u32 attachment_count;
	};
	/* 2d list, list of extra attachments in the foreach framebuffer, such as swapchain image view */
	VkImageView* vo_supplementary_attachments;
	/* must equal to supplementary_attachment_bucket_count * supplementary_attachment_bucket_depth */
	u32 supplementary_attachment_count;
	u32 supplementary_attachment_bucket_count;
	u32 supplementary_attachment_bucket_depth;

	u32 depth_attachment_index;
	u32 swapchain_attachment_index;
} vulkan_camera_render_pass_t;

typedef BUFFER /* vulkan_camera_render_pass_t */ vulkan_camera_render_pass_list_t;

/* structure to hold vulkan_descriptor_set objects for one render pass */
typedef struct vulkan_render_pass_descriptor_sets_t
{
	vulkan_descriptor_set_t render_set;
	vulkan_descriptor_set_t* sub_render_sets;
	u32 sub_render_set_count;
} vulkan_render_pass_descriptor_sets_t;

/* structure to hold pair of render pass handles to identify correct edge/link in the render pass graph */
typedef struct vulkan_render_pass_handle_pair_t
{
	vulkan_render_pass_handle_t prev_handle;
	vulkan_render_pass_handle_t handle;
} vulkan_render_pass_handle_pair_t;

typedef dictionary_t /* <vulkan_render_pass_handle_pair_t, vulkan_render_pass_descriptor_sets_t> */ vulkan_render_pass_descriptor_sets_map_t;

typedef enum vulkan_camera_render_target_status_t
{
	VULKAN_CAMERA_RENDER_TARGET_STATUS_UNKOWN,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN,
	VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR
} vulkan_camera_render_target_status_t;

UNUSED_FUNCTION static const char* vulkan_camera_render_target_status_str(vulkan_camera_render_target_status_t status)
{
	switch(status)
	{
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN";
		case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR: return "VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR";
		default: return "<undefined>";
	}
}

typedef enum vulkan_camera_render_target_binding_type_t
{
	/* the framebuffers will be created with this render target along with other render targets / attachments */
	VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED,
	/* the framebuffers will be created with this render target only (but with other internal attachments which are required by the renderpasses) */
	VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE
} vulkan_camera_render_target_binding_type_t;

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
	vulkan_render_pass_t* swapchain_depth_clear_pass;
	vulkan_render_pass_t* depth_clear_pass;
	vulkan_render_pass_t* depth_write_pass;
	vulkan_render_pass_t* swapchain_clear_pass;
	
	vulkan_camera_render_target_status_t render_target_status;

	/* current color render target texture (NULL if the target is the swapchain color attachment (screen)) */
	vulkan_texture_t* color_render_target;
	/* current depth render target texture (NULL if the target is the backed depth attachment */
	vulkan_texture_t* depth_render_target;

	/* (backed depth attachment) depth attachment common across all render passes, which require depth stencil attachment, registered with this camera */
	vulkan_attachment_t* default_depth_attachment;

	/* depth attachment */
	vulkan_attachment_t* current_depth_attachment;

	/* material to render depth, it is loaded upon calling vulkan_camera_set_render_target as DEPTH when the target is not screen */
	vulkan_material_t* depth_material;

	/* framebuffers holding only 'default_depth_attachment', there can be max 6 depth framebuffers (for each cube face) */
	vulkan_framebuffer_t depth_write_framebuffers[6];
	/* same as above but for depth clear pass  */
	vulkan_framebuffer_t depth_clear_framebuffers[6];
	/* number of instantiated / created framebuffer objects in the above list */
	u32 depth_framebuffer_count;

	bool is_color_supported;
	bool is_depth_supported;

	/* how does the color render target binds to the framebuffer,
	 * if external depth render target is present then whether to share the framebuffer with it or not,
	 * otherwise it has no meaning */
	vulkan_camera_render_target_binding_type_t color_binding_type;
	/* how does the depth render target binds to the framebuffer 
	 * if external depth render target is present then whether to share the framebuffer with color target 
	 * (either swapchain or external) or not,
	 * if external depth render target is not present but default depth render target is being used then 
	 * it has no meaning.
	 * Also if the camera doesn't supports depth rendering then it has no meaning in this case too. */
	vulkan_camera_render_target_binding_type_t depth_binding_type;

	/* list of registered render pass */
	vulkan_camera_render_pass_list_t render_passes;
	/* list of all the descriptor sets belonging to each render pass registered with this camera  */
	vulkan_render_pass_descriptor_sets_map_t render_pass_descriptor_sets;

	/* size of the render target */
	struct 
	{
		/* width of the render target (render window width in case of rendering to a window) */
		u32 width;
		/* height of the render target (render window height in case of rendering to a window) */
		u32 height;
	} render_target_size, prev_render_target_size, allocated_attachment_size;

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

	/* true, if the render area is defined as relative by calling vulkan_camera_set_render_area_relative(...)
	 * otherwise, false.
	 */
	bool is_render_area_relative;
	/* true, if the render area is equal to the size of the render target (full size), otherwise false */
	bool is_render_area_full;

	/* renderable area of the render target relative to the global viewport (size of the swapchain)  */
	struct
	{
		struct
		{
			/* absolute_x = x * width of the swapchain image */
			u32 x;
			/* absolute_y = y * height of the swapchian image */
			u32 y;
		} offset;

		struct
		{
			/* absolute_width = width * width of the swapchain image */
			u32 width;
			/* absolute_height = height * height of the swapchain image */
			u32 height;
		} extent;
	} render_area_relative;

	/* maximum number of shot that can be taken in this frame; 1 if current render target is not cube, otherwise 6 */
	u32 max_shot_count;
	/* index of the current shot being taken */
	u32 current_shot_index;
	/* number of shot taken in this frame */
	u32 shot_taken;


	/* TODO: 
	 * 		struct_descriptor_t 		camera_info_struct;
	 *									{
	 *		struct_field_handle_t 			transform_field;
	 * 		struct_field_handle_t			projection_field;
	 * 		struct_field_handle_t			view_field;
	 * 		struct_field_handle_t			screen_field;
	 * 									};
	 */
	/* 	uniform CameraInfo 
	 *  	{
	 *		mat4 transform;
	 *		mat4 projection;
	 *		mat4 view;
	 *		mat4 screen; 
	 * 	} 
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
	event_subscription_handle_t render_area_recalculate_handle;
	event_subscription_handle_t attachments_recreate_handle;
	event_subscription_handle_t descriptors_rewrite_handle;
	event_subscription_handle_t supplementary_attachments_recopy_handle;
	event_subscription_handle_t framebuffers_recreate_handle;

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
RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_camera_render_target_binding_type_t binding_type, vulkan_texture_t* texture);
RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene, u64 queue_mask);
RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* texture);

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_render_pass_handle_t handle);
RENDERER_API vulkan_render_pass_descriptor_sets_t* vulkan_camera_get_descriptor_sets(vulkan_camera_t* camera, vulkan_render_pass_handle_t prev_handle, vulkan_render_pass_handle_t handle);
RENDERER_API void vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* render_pass, vulkan_render_pass_handle_t prev_pass_handle, vulkan_framebuffer_attachments_layout_description_t* framebuffer_layout_description);
RENDERER_API vulkan_camera_render_pass_t* vulkan_camera_get_camera_render_pass_from_pass_handle(vulkan_camera_t* camera, vulkan_render_pass_handle_t handle);

/* getters */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_camera_is_depth_render_only(vulkan_camera_t* camera)
{
	return (camera->depth_render_target != NULL) && (camera->color_render_target == NULL) && (camera->depth_material != NULL);	
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
/* 	offset_x: offset in the x direction relative to the global viewport
	offset_y: offset in the y direction relative to the global viewport 
	width: width of the render area following the offset_x
	height: height of the render area following the offset_y 
	NOTE: calling this function overrides any values (and any effects) set by vulkan_camera_set_render_area_relative(...) function.
*/
RENDERER_API void vulkan_camera_set_render_area(vulkan_camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height);
/*	offset_x: valid values are from 0 to 100
	offset_y: valid values are from 0 to 100
	width: valid values are from 0 to 100
	height: valid values are from 0 to 100
	NOTE: calling this function overrides any values set by vulkan_camera_set_render_area(...) function.
 */
RENDERER_API void vulkan_camera_set_render_area_relative(vulkan_camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height);
/*	sets the render area to the size of the render window */
RENDERER_API void vulkan_camera_set_render_area_default(vulkan_camera_t* camera);

END_CPP_COMPATIBLE
