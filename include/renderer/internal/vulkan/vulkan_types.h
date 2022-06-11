
#pragma once

typedef enum vulkan_shader_type_t
{
	VULKAN_SHADER_TYPE_VERTEX = 0,
	VULKAN_SHADER_TYPE_FRAGMENT = 1,
	VULKAN_SHADER_TYPE_GEOMETRY = 2,
	VULKAN_SHADER_TYPE_TESSELLATION = 3
} vulkan_shader_type_t;

typedef enum vulkan_render_pass_type_t
{
	VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER,
	VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET
} vulkan_render_pass_type_t;
