/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_light.h is a part of VulkanRenderer

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


#pragma once

#include <sge/defines.h>
#include <sge/struct_descriptor.h>
#include <sge/internal/vulkan/vulkan_object.h>

#include <hpml/vec3.h>
#include <hpml/mat4.h>

typedef enum vulkan_light_type_t
{
	VULKAN_LIGHT_TYPE_UNDEFINED = 0,
	VULKAN_LIGHT_TYPE_DIRECTIONAL = 1,
	VULKAN_LIGHT_TYPE_FAR = VULKAN_LIGHT_TYPE_DIRECTIONAL,
	VULKAN_LIGHT_TYPE_POINT = 2,
	VULKAN_LIGHT_TYPE_OMIN = VULKAN_LIGHT_TYPE_POINT,
	VULKAN_LIGHT_TYPE_SPOT = 3,
	VULKAN_LIGHT_TYPE_AREA = 4,
	VULKAN_LIGHT_TYPE_CURVE = 5,
	VULKAN_LIGHT_TYPE_VOLUME = 6,
	VULKAN_LIGHT_TYPE_AMBIENT = 7
} vulkan_light_type_t;

typedef struct vulkan_texture_t vulkan_texture_t;
typedef vulkan_texture_t vulkan_render_texture_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_material_t vulkan_material_t;
typedef struct vulkan_camera_t vulkan_camera_t;

typedef struct vulkan_light_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;

	/* the reason we can't have only single shadow camera, shader or material
	 * is that, we need separate depth buffers for each light as they have different positions and configurations in space,
	 * and VkFramebuffer objects constructed with specific VkImageView(VkImage)[or called attachment] objects and they can't be modified after that,
	 * so we need separate VkFramebuffer objects for each shadow map (VkImage/VkImageView).
	 * And since each light can have different ways of rendering the depth values (programmable), we need separate shaders for them, hence shaders.
	 * shadow_map <-- shadow_shader <-- shadow_material <-- shadow_camera */
	/* shadow map render target (it will be used as a depth buffer attachment for render passes) */
	vulkan_render_texture_t* shadow_map;
	/* shader to generate shadow map (render depth values to the shadow map render target) */
	vulkan_shader_t* shadow_shader;
	/* material create out of the above 'shadow_shader', it will be used over all the objects in the scene which are affected 
	 * by this light */
	vulkan_material_t* shadow_material;
	/* camera to hold framebuffers for the shadowmap, and record command buffers for rendering the shadow map */
	vulkan_camera_t* shadow_camera;

	/* true means it this light generates shadow map, false means it doesn't */
	bool is_cast_shadow;
	/* true means this light illumates the scene and may or may not cast shadows (since casting shadows also depends on is_cast_shadow),
	 * false means, it doesn't illumate the scene. 
	 * NOTE: if the value of 'is_active' is false then it has two cases
	 * 			1. The scene will be completely black if the scene uses lights 
	 * 			2. The scene will be rendered based on the base colors if the scene doesn't use lights */
	bool is_active;
	/* is dispatachable data out of date? 
	 * set to true if any of functions modifying spot angle (for spot light), direction, etc. are called 
	 * set to false when vulkan_light_get_dispatchable_data is called */
	bool is_dirty;
	/* set to true if the shadowmap recreated,
	 * set to false when vulkan_light_get_shadowmap function is called  */
	bool is_shadowmap_dirty;

	struct_descriptor_t struct_definition;

	/* internal use only, however can be used externally also! */
	vulkan_light_type_t type;

	// cpu side cache to reduce GPU memory access
	vec3_t color;
	float intensity;

	vec3_t position;
	mat4_t rotation;
	vec3_t euler_rotation;

} vulkan_light_t;

/* performs dynamic casting (expensive), use only when you don't know the source type */
#define VULKAN_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)
#define VULKAN_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_LIGHT) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_LIGHT_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)
#define VULKAN_LIGHT_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)

typedef vulkan_light_t vulkan_ambient_light_t;

typedef struct vulkan_directional_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
	vec3_t direction;
} vulkan_directional_light_t;

typedef vulkan_directional_light_t vulkan_far_light_t;

#define VULKAN_DIRECTIONAL_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_directional_light_t*, VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT, ptr)
#define VULKAN_DIRECTIONAL_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_directional_light_t*, VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT, ptr)
#define VULKAN_FAR_LIGHT(ptr) VULKAN_DIRECTIONAL_LIGHT(ptr)
#define VULKAN_FAR_LIGHT_CONST(ptr) VULKAN_DIRECTIONAL_LIGHT_CONST(ptr)

typedef struct vulkan_point_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
} vulkan_point_light_t;

#define VULKAN_POINT_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_point_light_t*, VULKAN_OBJECT_TYPE_POINT_LIGHT, ptr)
#define VULKAN_POINT_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_point_light_t*, VULKAN_OBJECT_TYPE_POINT_LIGHT, ptr)

typedef struct vulkan_spot_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
	float angle;
	vec3_t direction;
} vulkan_spot_light_t;

#define VULKAN_SPOT_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_spot_light_t*, VULKAN_OBJECT_TYPE_SPOT_LIGHT, ptr)
#define VULKAN_SPOT_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_spot_light_t*, VULKAN_OBJECT_TYPE_SPOT_LIGHT, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API vulkan_light_t* vulkan_light_new(memory_allocator_t* allocator, vulkan_light_type_t type);
SGE_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type);
SGE_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light);
SGE_API void vulkan_light_destroy(vulkan_light_t* light);
SGE_API void vulkan_light_release_resources(vulkan_light_t* light);

/* setters */
SGE_API void vulkan_light_set_active(vulkan_light_t* light, bool is_active);
SGE_API void vulkan_light_set_cast_shadow(vulkan_light_t* light, bool is_cast_shadow);
SGE_API void vulkan_light_set_intensity(vulkan_light_t* light, float intensity);
SGE_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t color);
SGE_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t position);
SGE_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t rotation);
SGE_API void vulkan_light_set_spot_angle(vulkan_light_t* light, float angle);

/* getters */
SGE_API vulkan_light_type_t vulkan_light_get_type(vulkan_light_t* light);
SGE_API struct_descriptor_t* vulkan_light_get_struct_def(vulkan_light_t* light);
SGE_API vulkan_material_t* vulkan_light_get_shadow_material(vulkan_light_t* light);
SGE_API vulkan_texture_t* vulkan_light_get_shadow_map(vulkan_light_t* light);
SGE_API vulkan_camera_t* vulkan_light_get_shadow_camera(vulkan_light_t* light);
SGE_API bool vulkan_light_is_active(vulkan_light_t* light);
SGE_API bool vulkan_light_is_cast_shadow(vulkan_light_t* light);
SGE_API bool vulkan_light_is_dirty(vulkan_light_t* light);
SGE_API bool vulkan_light_is_shadow_map_dirty(vulkan_light_t* light);
SGE_API void vulkan_light_get_dispatchable_data(vulkan_light_t* light, u32 shadowmap_index, void* const out_buffer);
SGE_API u32 vulkan_light_get_dispatchable_data_size(vulkan_light_t* light);

SGE_API void vulkan_light_begin(vulkan_light_t* light);
SGE_API bool vulkan_light_irradiate(vulkan_light_t* light);
SGE_API void vulkan_light_end(vulkan_light_t* light);

END_CPP_COMPATIBLE
