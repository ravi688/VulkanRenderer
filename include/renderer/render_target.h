
#pragma once


/* NOTE: this should be in sync with vulkan_render_target_type_t */
typedef enum render_target_type_t
{
	RENDER_TARGET_TYPE_UNDEFINED = 0
	RENDER_TARGET_TYPE_COLOR_TEXTURE,
	RENDER_TARGET_TYPE_DEPTH_TEXTURE,
	RENDER_TARGET_TYPE_COLOR_CUBEMAP,
	RENDER_TARGET_TYPE_DEPTH_CUBEMAP
} render_target_type_t;


typedef enum render_target_render_technique_t
{
	RENDER_TARGET_RENDER_TECHNIQUE_COPY = 0,
	RENDER_TARGET_RENDER_TECHNIQUE_ATTACH
} render_target_render_technique_t;
