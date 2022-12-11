
#pragma once

#include <renderer/defines.h>

/* NOTE: vulkan_shader_type_t and vulkan_shader_stage_t 
	are identical they can be substituted with each other
*/

/* used by the followings: 
	1. vulkan shader loader
	2. vulkan shader
*/
typedef enum vulkan_shader_type_t
{
	VULKAN_SHADER_TYPE_VERTEX = 0,
	VULKAN_SHADER_TYPE_FRAGMENT = 1,
	VULKAN_SHADER_TYPE_GEOMETRY = 2,
	VULKAN_SHADER_TYPE_TESSELLATION = 3,
	VULKAN_SHADER_TYPE_MAX,

	VULKAN_SHADER_STAGE_VERTEX = VULKAN_SHADER_TYPE_VERTEX,
	VULKAN_SHADER_STAGE_FRAGMENT = VULKAN_SHADER_TYPE_FRAGMENT,
	VULKAN_SHADER_STAGE_GEOMETRY = VULKAN_SHADER_TYPE_GEOMETRY,
	VULKAN_SHADER_STAGE_TESSELLATION = VULKAN_SHADER_TYPE_TESSELLATION,
	VULKAN_SHADER_STAGE_MAX = VULKAN_SHADER_TYPE_MAX,

	VULKAN_SHADER_STAGE_VERTEX_BIT = BIT32(VULKAN_SHADER_STAGE_VERTEX),
	VULKAN_SHADER_STAGE_FRAGMENT_BIT = BIT32(VULKAN_SHADER_STAGE_FRAGMENT),
	VULKAN_SHADER_STAGE_GEOMETRY_BIT = BIT32(VULKAN_SHADER_STAGE_GEOMETRY),
	VULKAN_SHADER_STAGE_TESSELLATION_BIT = BIT32(VULKAN_SHADER_STAGE_TESSELLATION)
} vulkan_shader_type_t;

typedef vulkan_shader_type_t vulkan_shader_stage_t;

typedef enum vulkan_render_pass_type_t
{
	/*	there will be only one framebuffer in the renderpass. */
	VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER,
	/*	there can be multiple framebuffers (3 for tripple buffering). */
	VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET,
	VULKAN_RENDER_PASS_TYPE_MAX
} vulkan_render_pass_type_t;


/*
	defines the technique to be used while rendering to render target
*/
typedef enum vulkan_render_target_technique_t
{
	/* rendering will not happen but all the computations shall be done.
		this is useful when you want to just compute the just required data to 
		perform final rendering process; 
		you can capture a series of frame's data and then send it over a network
		to a remote device (possibly a GPU development board) to perform the final
		rendering where GPU performes the work without any programmable shader.

		NOTE: that the
	*/
	VULKAN_RENDER_TARGET_TECHNIQUE_UNDEFINED = 0,

	/* 	render target will be used as a copy target (drawing will be done on the swapchain buffer and then copied to the target
		this comes with a advantage that you can implement a mirror effect.
	*/
	VULKAN_RENDER_TARGET_TECHNIQUE_COPY,

	/* render target will be used a direct target (drawing will be done directly to the target
		this comes with a disadvantage that you can't implement a mirror effect.
	 */
	VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH,
	VULKAN_RENDER_TARGET_TECHNIQUE_MAX
} vulkan_render_target_technique_t;

