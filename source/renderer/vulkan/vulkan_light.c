/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_light.c is a part of VulkanRenderer

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


#include <sge/internal/vulkan/vulkan_light.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_render_texture.h>
#include <sge/internal/vulkan/vulkan_shader.h>
#include <sge/internal/vulkan/vulkan_shader_library.h>
#include <sge/internal/vulkan/vulkan_material_library.h>
#include <sge/internal/vulkan/vulkan_camera_system.h>
#include <sge/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>

#include <hpml/vec4.h>
#include <hpml/affine_transformation.h>

/* constructors & destructors */

#define UNSUPPORTED_LIGHT_TYPE(type) LOG_FETAL_ERR("Unsupported light type: %u\n", type)

SGE_API vulkan_light_t* vulkan_light_new(memory_allocator_t* allocator, vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			AUTO light = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DIRECTIONAL_LIGHT, vulkan_directional_light_t);
			memzero(light, vulkan_directional_light_t);
			VULKAN_OBJECT_INIT(light, VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
			VULKAN_OBJECT_INIT(BASE(light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			return CAST_TO(vulkan_light_t*, light);
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			AUTO light = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_POINT_LIGHT, vulkan_point_light_t);
			memzero(light, vulkan_point_light_t);
			VULKAN_OBJECT_INIT(light, VULKAN_OBJECT_TYPE_POINT_LIGHT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
			VULKAN_OBJECT_INIT(BASE(light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			return CAST_TO(vulkan_light_t*, light);
		}
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO light = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SPOT_LIGHT, vulkan_spot_light_t);
			memzero(light, vulkan_spot_light_t);
			VULKAN_OBJECT_INIT(light, VULKAN_OBJECT_TYPE_SPOT_LIGHT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
			VULKAN_OBJECT_INIT(BASE(light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			return CAST_TO(vulkan_light_t*, light);
		}
		case VULKAN_LIGHT_TYPE_AMBIENT:
		{
			AUTO light = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_AMBIENT_LIGHT, vulkan_ambient_light_t);
			memzero(light, vulkan_ambient_light_t);
			VULKAN_OBJECT_INIT(light, VULKAN_OBJECT_TYPE_AMBIENT_LIGHT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
			return CAST_TO(vulkan_light_t*, light);
		}		default:
			UNSUPPORTED_LIGHT_TYPE(type);
	};
	return NULL;
}

static void add_fields_for_far_light(struct_descriptor_t* descriptor)
{
	struct_descriptor_add_field(descriptor, "projection", GLSL_TYPE_MAT4);
	struct_descriptor_add_field(descriptor, "view", GLSL_TYPE_MAT4);
	struct_descriptor_add_field(descriptor, "color", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "intensity", GLSL_TYPE_FLOAT);
	struct_descriptor_add_field(descriptor, "direction", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "shadow_index", GLSL_TYPE_UINT);
}

static void add_fields_for_point_light(struct_descriptor_t* descriptor)
{
	struct_descriptor_add_field(descriptor, "color", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "intensity", GLSL_TYPE_FLOAT);
	struct_descriptor_add_field(descriptor, "position", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "shadow_index", GLSL_TYPE_UINT);
}

static void add_fields_for_spot_light(struct_descriptor_t* descriptor)
{
	struct_descriptor_add_field(descriptor, "projection", GLSL_TYPE_MAT4);
	struct_descriptor_add_field(descriptor, "view", GLSL_TYPE_MAT4);
	struct_descriptor_add_field(descriptor, "color", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "intensity", GLSL_TYPE_FLOAT);
	struct_descriptor_add_field(descriptor, "position", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "direction", GLSL_TYPE_VEC3);
	struct_descriptor_add_field(descriptor, "angle", GLSL_TYPE_FLOAT);
	struct_descriptor_add_field(descriptor, "shadow_index", GLSL_TYPE_UINT);
}

static void setup_gpu_resources(vulkan_light_t* super)
{
	vulkan_light_t* light = VULKAN_LIGHT(super);
	// setup light struct definition
	OBJECT_INIT(&light->struct_definition, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(light->renderer->allocator, &light->struct_definition, "lightInfo", GLSL_TYPE_UNIFORM_BUFFER, GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT);
	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			add_fields_for_far_light(&light->struct_definition);
			break;
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			add_fields_for_point_light(&light->struct_definition);
			break;
		}
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			add_fields_for_spot_light(&light->struct_definition);
			break;
		}
		case VULKAN_LIGHT_TYPE_AMBIENT:
		{
			break;
		}
		default:
			UNSUPPORTED_LIGHT_TYPE(light->type);
	};
	struct_descriptor_end(light->renderer->allocator, &light->struct_definition);
}

SGE_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type)
{
	vulkan_light_t* light = vulkan_light_new(renderer->allocator, type);
	vulkan_light_create_no_alloc(renderer, type, light);
	return light;
}


SGE_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			vulkan_directional_light_t* dir_light = VULKAN_DIRECTIONAL_LIGHT(light);
			VULKAN_OBJECT_SET_BASE(dir_light, true);
			VULKAN_OBJECT_MEMZERO(dir_light, vulkan_directional_light_t);
			VULKAN_OBJECT_INIT(BASE(dir_light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			break;
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			vulkan_point_light_t* point_light = VULKAN_POINT_LIGHT(light);
			VULKAN_OBJECT_SET_BASE(point_light, true);
			VULKAN_OBJECT_MEMZERO(point_light, vulkan_point_light_t);
			VULKAN_OBJECT_INIT(BASE(point_light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			break;
		}
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			vulkan_spot_light_t* spot_light = VULKAN_SPOT_LIGHT(light);
			VULKAN_OBJECT_SET_BASE(spot_light, true);
			VULKAN_OBJECT_MEMZERO(spot_light, vulkan_spot_light_t);
			VULKAN_OBJECT_INIT(BASE(spot_light), VULKAN_OBJECT_TYPE_LIGHT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
			break;
		}
		case VULKAN_LIGHT_TYPE_AMBIENT:
		{
			VULKAN_OBJECT_MEMZERO(light, vulkan_ambient_light_t);
			break;
		}
		default:
			UNSUPPORTED_LIGHT_TYPE(light->type);
	}

	/* TODO: this type of random code can't be afforded, split this vulkan_light class into directional, spot, point, and ambient */
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(light);

	light->renderer = renderer;
	/* initially the light will be active */
	light->is_active = true;
	light->position = vec3_zero();
	light->euler_rotation = vec3_zero();
	light->color = vec3_one();
	light->intensity = 1.0f;
	light->type = type;

	setup_gpu_resources(super);

	AUTO color = COLOR_WHITE;
	vulkan_light_set_color(super, REINTERPRET_TO(vec3_t, color));
	vulkan_light_set_intensity(super, 1.0f);
	vulkan_light_set_position(super, vec3(0, 0.6f, -1.8f));
	vulkan_light_set_rotation(super, vec3(0, -100 DEG, -10 DEG));
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_AMBIENT:
		{
			break;
		}
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			// vulkan_light_set_projection(super, mat4_ortho_projection(0.04f, 20.0f, 3, 1));
			break;
		}
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			vulkan_light_set_spot_angle(super, 30 DEG);
			// vulkan_light_set_projection(super, mat4_persp_projection(0.04f, 20.0f, VULKAN_SPOT_LIGHT(super)->angle, 1));
			break;
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			// vulkan_light_set_projection(super, mat4_persp_projection(0.04f, 20.0f, 65 DEG, 1));
			break;
		}
		default:
			UNSUPPORTED_LIGHT_TYPE(type);
	}
}

SGE_API void vulkan_light_destroy(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	struct_descriptor_unmap(&light->struct_definition);
	if(light->is_cast_shadow)
	{
		_debug_assert__(light->shadow_camera != NULL);
		_debug_assert__(light->shadow_shader != NULL);
		_debug_assert__(light->shadow_material != NULL);
		_debug_assert__(light->shadow_map != NULL);
		vulkan_render_texture_destroy(light->shadow_map);
		vulkan_render_texture_release_resources(light->shadow_map);
	}
}

SGE_API void vulkan_light_release_resources(vulkan_light_t* light)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(light);
	struct_descriptor_destroy(light->renderer->allocator, &light->struct_definition);
	if(VULKAN_OBJECT_IS_INTERNAL(super))
		memory_allocator_dealloc(light->renderer->allocator, super);
}

/* setters */

SGE_API void vulkan_light_set_spot_angle(vulkan_light_t* light, float angle)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(super);
	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO _light = VULKAN_SPOT_LIGHT(super);
			_light->angle = angle;
			if(light->is_cast_shadow)
			{
				_debug_assert__(light->shadow_camera != NULL);
				vulkan_camera_set_field_of_view(light->shadow_camera, _light->angle);
			}
			break;
		}
		default:
			log_wrn("You are trying to set spot angle for the light which isn't a spot light, light type: %u\n", VULKAN_LIGHT(light)->type);
		break;
	}
	light->is_dirty = true;
}

SGE_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t position)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(super);
	light->position = position;
	light->is_dirty = true;
	if(light->is_cast_shadow)
	{
		_debug_assert__(light->shadow_camera != NULL);
		if(light->type == VULKAN_LIGHT_TYPE_POINT)
			vulkan_camera_set_position_cube(light->shadow_camera, light->position);
		else
			vulkan_camera_set_position(light->shadow_camera, light->position);
	}
}

SGE_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t rotation)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(super);
	light->euler_rotation = rotation;
	mat4_move(light->rotation, mat4_rotation(rotation.x, rotation.y, rotation.z));
	vec3_t position = light->position;
	
	mat4_t view = mat4_inverse(mat4_mul(2, mat4_translation(position.x, position.y, position.z), light->rotation));

	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			AUTO _light = VULKAN_DIRECTIONAL_LIGHT(super);
			vec4_t dir = mat4_mul_vec4(mat4_transpose(view), 1, 0, 0, 0);
			_light->direction = vec3(dir.x, dir.y, dir.z);
		}
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO _light = VULKAN_SPOT_LIGHT(super);
			vec4_t dir = mat4_mul_vec4(mat4_transpose(view), 1, 0, 0, 0);
			_light->direction = vec3(dir.x, dir.y, dir.z);
		}
		break;
		case VULKAN_LIGHT_TYPE_POINT:
		{
			debug_log_warning("You're tyring to set rotation of a point light, for a point light direction or rotation is not defined in V3D");
			break;
		}
		default:
		{
			DEBUG_LOG_FETAL_ERROR("Unexpected vulkan_light_type_t : %u, this type of light is still not supported", light->type);
			break;
		}
	}

	light->is_dirty = true;

	if(light->is_cast_shadow)
	{
		_debug_assert__(light->shadow_camera != NULL);
		vulkan_camera_set_rotation(light->shadow_camera, light->euler_rotation);
	}
}

static const char* get_shadow_shader_path(vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_SPOT: return "shaders/presets/lights/spot_light.shadowmap.sb";
		case VULKAN_LIGHT_TYPE_POINT: return "shaders/presets/lights/point_light.shadowmap.sb";
		case VULKAN_LIGHT_TYPE_DIRECTIONAL: return "shaders/presets/lights/directional_light.shadowmap.sb";
		default:
			DEBUG_LOG_FETAL_ERROR("This vulkan_light_type_t(%u) doesn't support shadow casting", type);
			break;
	};
	return "";
}

static vulkan_render_texture_type_t get_render_texture_type(vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_SPOT: return VULKAN_RENDER_TEXTURE_TYPE_DEPTH;
		case VULKAN_LIGHT_TYPE_POINT: return VULKAN_RENDER_TEXTURE_TYPE_DEPTH_CUBE;
		case VULKAN_LIGHT_TYPE_DIRECTIONAL: return VULKAN_RENDER_TEXTURE_TYPE_DEPTH;
		default:
			DEBUG_LOG_FETAL_ERROR("This vulkan_light_type_t(%u) doesn't support shadow casting", type);
			break;
	}
	return VULKAN_RENDER_TEXTURE_TYPE_UNDEFINED;
}

static vulkan_camera_projection_type_t  get_camera_projection_type(vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_SPOT: return VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE;
		case VULKAN_LIGHT_TYPE_POINT: return VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE;
		case VULKAN_LIGHT_TYPE_DIRECTIONAL: return VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
		default:
			DEBUG_LOG_FETAL_ERROR("This vulkan_light_type_t(%u) doesn't support shadow casting", type);
			break;
	}
	return VULKAN_CAMERA_PROJECTION_TYPE_UNDEFINED;
}

SGE_API void vulkan_light_set_active(vulkan_light_t* light, bool is_active)
{
	light = VULKAN_LIGHT(light);
	light->is_active = is_active;
	/* TODO: clear the shadow map render texture with 0.0 depth values so that object will be rendered black if only this light existed 
	 * NOTE: it might be possible that we are clearing the render texture while it is still being used in the previous frame, 
	 * in that case we may wait for the previous frame to complete. */
}

static void setup_shadow_camera_properties(vulkan_camera_t* shadow_camera, vulkan_light_t* light)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(light);
	vulkan_camera_set_clear(shadow_camera, COLOR_RED, 1.0f);
	if(light->type == VULKAN_LIGHT_TYPE_POINT)
		vulkan_camera_set_position_cube(shadow_camera, light->position);
	else if(light->type != VULKAN_LIGHT_TYPE_FAR)
		vulkan_camera_set_position(shadow_camera, vec3_zero());
	else
		vulkan_camera_set_position(shadow_camera, light->position);
	vulkan_camera_set_rotation(shadow_camera, light->euler_rotation);
	if(light->type == VULKAN_LIGHT_TYPE_POINT)
		vulkan_camera_set_field_of_view(shadow_camera, 90 DEG);
	else if(light->type == VULKAN_LIGHT_TYPE_SPOT)
		vulkan_camera_set_field_of_view(shadow_camera, VULKAN_SPOT_LIGHT(super)->angle);
}

SGE_API void vulkan_light_set_cast_shadow(vulkan_light_t* light, bool is_cast_shadow)
{
	vulkan_light_t* super = light;
	light = VULKAN_LIGHT(light);
	light->is_cast_shadow = is_cast_shadow;
	if(is_cast_shadow && (light->shadow_shader == NULL))
	{
		AUTO slib = light->renderer->shader_library;
		AUTO mlib = light->renderer->material_library;
		AUTO csys = light->renderer->camera_system;

		/* create render texture */
		vulkan_render_texture_create_info_t create_info = 
		{
			/* TODO: the width, and height should be functions of the area span illuminated by this light and the quality of shadow rendering */
			.width = 512,
			.height = 512,
			.depth = 1,
			.channel_count = 4,
			.type = get_render_texture_type(light->type)
		};
		light->shadow_map = vulkan_render_texture_create(light->renderer, &create_info);

		/* create camera to render onto the shadow map render texture */
		light->shadow_camera = vulkan_camera_system_getH(csys, vulkan_camera_system_create_camera(csys, get_camera_projection_type(light->type)));
		/* and set the shadow map render texture as render target for this camera */
		vulkan_camera_set_render_target(light->shadow_camera, VULKAN_CAMERA_RENDER_TARGET_TYPE_DEPTH, VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, light->shadow_map);
		setup_shadow_camera_properties(light->shadow_camera, super);

		/* load the shadow map shader */
		vulkan_shader_load_info_t load_info =
		{
			.path = get_shadow_shader_path(light->type),
			.is_vertex_attrib_from_file = true
		};
		AUTO shader_handle = vulkan_shader_library_load_shader(slib, &load_info, "BuiltIn:Light:ShadowMapShader");
		light->shadow_shader = vulkan_shader_library_getH(slib, shader_handle);
		
		/* create material for it */
		light->shadow_material = vulkan_material_library_getH(mlib, 
									vulkan_material_library_create_materialH(mlib, shader_handle, "BuiltIn:Light:ShadowMapShader"));

		light->is_shadowmap_dirty = true;
	}
}

SGE_API void vulkan_light_set_intensity(vulkan_light_t* light, float intensity)
{
	light = VULKAN_LIGHT(light);
	light->intensity = intensity;
	light->is_dirty = true;
}

SGE_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t color)
{
	light = VULKAN_LIGHT(light);
	light->color = color;
	light->is_dirty = true;
}

SGE_API vulkan_light_type_t vulkan_light_get_type(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	return light->type;
}

SGE_API struct_descriptor_t* vulkan_light_get_struct_def(vulkan_light_t* light)
{ 
	light = VULKAN_LIGHT(light);
	return &light->struct_definition;
}
SGE_API vulkan_material_t* vulkan_light_get_shadow_material(vulkan_light_t* light)
{ 
	light = VULKAN_LIGHT(light);
	return light->shadow_material;
}
SGE_API vulkan_texture_t* vulkan_light_get_shadow_map(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	light->is_shadowmap_dirty = false;
	return light->shadow_map;
}
SGE_API vulkan_camera_t* vulkan_light_get_shadow_camera(vulkan_light_t* light)
{ 
	light = VULKAN_LIGHT(light);
	return light->shadow_camera;
}
SGE_API bool vulkan_light_is_active(vulkan_light_t* light)
{ 
	light = VULKAN_LIGHT(light);
	return light->is_active;
}
SGE_API bool vulkan_light_is_cast_shadow(vulkan_light_t* light)
{ 
	light = VULKAN_LIGHT(light);
	return light->is_cast_shadow;
}

SGE_API bool vulkan_light_is_dirty(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	return light->is_dirty;
}

SGE_API bool vulkan_light_is_shadow_map_dirty(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	return light->is_shadowmap_dirty;
}

typedef struct point_light_dispatchable_data_t
{
	/* offset 0 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t color;
	/* offset 12 */
	ALIGN_AS(GLSL_STD140_FLOAT_ALIGN) f32 intensity;
	/* offset 16 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t position;
	/* offset 28 */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 shadow_index;
} point_light_dispatchable_data_t ALIGN_AS(U32_NEXT_MULTIPLE(GLSL_STD140_VEC3_ALIGN, 16));

#define SIZEOF_POINT_LIGHT_DISPATCHABLE_DATA_T 32 /* bytes */
#define STRIDE_POINT_LIGHT_DISPATCHABLE_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_POINT_LIGHT_DISPATCHABLE_DATA_T, ALIGN_OF(point_light_dispatchable_data_t))

typedef struct spot_light_dispatchable_data_t
{
	/* offset 0 */
	ALIGN_AS(GLSL_STD140_MAT4_ALIGN) _mat4_t proj;
	/* offset 64 */
	ALIGN_AS(GLSL_STD140_MAT4_ALIGN) _mat4_t view;
	/* offset 128 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t color;
	/* offset 140 */
	ALIGN_AS(GLSL_STD140_FLOAT_ALIGN) f32 intensity;
	/* offset 144 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t position;
	/* offset 160 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t direction;
	/* offset 172 */
	ALIGN_AS(GLSL_STD140_FLOAT_ALIGN) f32 angle;
	/* offset 176 */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 shadow_index;
} spot_light_dispatchable_data_t ALIGN_AS(U32_NEXT_MULTIPLE(GLSL_STD140_MAT4_ALIGN, 16));

#define SIZEOF_SPOT_LIGHT_DISPATCHABLE_DATA_T 180 /* bytes */
#define STRIDE_SPOT_LIGHT_DISPATCHABLE_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_SPOT_LIGHT_DISPATCHABLE_DATA_T, ALIGN_OF(spot_light_dispatchable_data_t))

typedef struct far_light_dispatchable_data_t
{
	/* offset 0 */
	ALIGN_AS(GLSL_STD140_MAT4_ALIGN) _mat4_t proj;
	/* offset 64 */
	ALIGN_AS(GLSL_STD140_MAT4_ALIGN) _mat4_t view;
	/* offset 128 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t color;
	/* offset 140 */
	ALIGN_AS(GLSL_STD140_FLOAT_ALIGN) f32 intensity;
	/* offset 144 */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t direction;
	/* offset 156 */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 shadow_index;
} far_light_dispatchable_data_t ALIGN_AS(U32_NEXT_MULTIPLE(GLSL_STD140_MAT4_ALIGN, 16));

#define SIZEOF_FAR_LIGHT_DISPATCHABLE_DATA_T 160 /* bytes */
#define STRIDE_FAR_LIGHT_DISPATCHABLE_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_FAR_LIGHT_DISPATCHABLE_DATA_T, ALIGN_OF(far_light_dispatchable_data_t))

static void get_spot_light_dispatchable_data(vulkan_light_t* light, u32 shadowmap_index, spot_light_dispatchable_data_t* const data)
{
	vulkan_spot_light_t* spot_light = VULKAN_SPOT_LIGHT(light);
	light = VULKAN_LIGHT(light);
	_debug_assert__(light->is_cast_shadow);
	_debug_assert__(light->shadow_camera != NULL);
	data->proj = mat4_transpose(vulkan_camera_get_projection(light->shadow_camera)).raw4x4f32;
	data->view = mat4_transpose(vulkan_camera_get_view(light->shadow_camera)).raw4x4f32;
	data->color = light->color;
	data->intensity = light->intensity;
	data->position = light->position;
	data->direction = spot_light->direction;
	data->angle = spot_light->angle;
	data->shadow_index = shadowmap_index;
}

static void get_point_light_dispatchable_data(vulkan_light_t* light, u32 shadowmap_index, point_light_dispatchable_data_t* const data)
{
	light = VULKAN_LIGHT(light);
	data->color = light->color;
	data->intensity = light->intensity;
	data->position = light->position;
	data->shadow_index = shadowmap_index;
}

static void get_far_light_dispatchable_data(vulkan_light_t* light, u32 shadowmap_index, far_light_dispatchable_data_t* const data)
{
	vulkan_far_light_t* far_light = VULKAN_FAR_LIGHT(light);
	light = VULKAN_LIGHT(light);
	data->proj = mat4_transpose(vulkan_camera_get_projection(light->shadow_camera)).raw4x4f32;
	data->view = mat4_transpose(vulkan_camera_get_view(light->shadow_camera)).raw4x4f32;
	data->color = light->color;
	data->intensity = light->intensity;
	data->direction = far_light->direction;
	data->shadow_index = shadowmap_index;
}

SGE_API void vulkan_light_get_dispatchable_data(vulkan_light_t* light, u32 shadowmap_index, void* const out_buffer)
{
	vulkan_light_t* _light = VULKAN_LIGHT(light);
	switch(_light->type)
	{
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			spot_light_dispatchable_data_t data;
			get_spot_light_dispatchable_data(light, shadowmap_index, &data);
			_debug_assert__(SIZEOF_SPOT_LIGHT_DISPATCHABLE_DATA_T <= sizeof(data));
			memcpy(out_buffer, &data, SIZEOF_SPOT_LIGHT_DISPATCHABLE_DATA_T);
			break;
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			point_light_dispatchable_data_t data;
			get_point_light_dispatchable_data(light, shadowmap_index, &data);
			_debug_assert__(SIZEOF_POINT_LIGHT_DISPATCHABLE_DATA_T <= sizeof(data));
			memcpy(out_buffer, &data, SIZEOF_POINT_LIGHT_DISPATCHABLE_DATA_T);
			break;
		}
		case VULKAN_LIGHT_TYPE_FAR:
		{
			far_light_dispatchable_data_t data;
			get_far_light_dispatchable_data(light, shadowmap_index, &data);
			_debug_assert__(SIZEOF_FAR_LIGHT_DISPATCHABLE_DATA_T <= sizeof(data));
			memcpy(out_buffer, &data, SIZEOF_FAR_LIGHT_DISPATCHABLE_DATA_T);
			break;
		}
		default:
			DEBUG_LOG_FETAL_ERROR("Dispatchable data can't be built for vulkan_light_type_t(%u)", _light->type);
	}
	_light->is_dirty = false;
}

SGE_API u32 vulkan_light_get_dispatchable_data_size(vulkan_light_t* light)
{
	AUTO type = vulkan_light_get_type(light);
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_SPOT: return SIZEOF_SPOT_LIGHT_DISPATCHABLE_DATA_T;
		case VULKAN_LIGHT_TYPE_POINT: return SIZEOF_POINT_LIGHT_DISPATCHABLE_DATA_T;
		case VULKAN_LIGHT_TYPE_FAR: return SIZEOF_FAR_LIGHT_DISPATCHABLE_DATA_T;
		default:
			DEBUG_LOG_FETAL_ERROR("Dispatchable data size isn't defined for vulkan_light_type_t(%u)", type);
	}
	return 0;
}

SGE_API void vulkan_light_begin(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	if(light->is_cast_shadow)
		vulkan_camera_begin(light->shadow_camera);
}

SGE_API bool vulkan_light_irradiate(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	if(light->is_cast_shadow)
		return vulkan_camera_capture(light->shadow_camera, VULKAN_CAMERA_CLEAR_FLAG_CLEAR);
	return false;
}

SGE_API void vulkan_light_end(vulkan_light_t* light)
{
	light = VULKAN_LIGHT(light);
	if(light->is_cast_shadow)
		vulkan_camera_end(light->shadow_camera);
}
