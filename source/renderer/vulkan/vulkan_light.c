
#include <renderer/internal/vulkan/vulkan_light.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

/* constructors & destructors */

#define UNSUPPORTED_LIGHT_TYPE(type) LOG_FETAL_ERR("Unsupported light type: %u\n", type)

RENDERER_API vulkan_light_t* vulkan_light_new(vulkan_light_type_t type)
{
	vulkan_light_t* light;
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
			light = CAST_TO(vulkan_light_t*, heap_new(vulkan_directional_light_t));
			memzero(light, vulkan_directional_light_t);
		break;
		case VULKAN_LIGHT_TYPE_POINT:
			light =	CAST_TO(vulkan_light_t*, heap_new(vulkan_point_light_t));
			memzero(light, vulkan_point_light_t);
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
			light = CAST_TO(vulkan_light_t*, heap_new(vulkan_spot_light_t));
			memzero(light, vulkan_spot_light_t);
		break;
		case VULKAN_LIGHT_TYPE_AMBIENT:
			light = CAST_TO(vulkan_light_t*, heap_new(vulkan_ambient_light_t));
			memzero(light, vulkan_ambient_light_t);
		break;
		default:
			UNSUPPORTED_LIGHT_TYPE(type);
	};
	light->type = type;
	return light;
}

static void setup_gpu_resources(vulkan_light_t* light)
{
	// setup light struct definition
	struct_descriptor_begin(&light->struct_definition, "lightInfo", GLSL_TYPE_BLOCK);
		struct_descriptor_add_field(&light->struct_definition, "projection", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&light->struct_definition, "view", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&light->struct_definition, "color", GLSL_TYPE_VEC3);
		struct_descriptor_add_field(&light->struct_definition, "intensity", GLSL_TYPE_FLOAT);

		switch(light->type)
		{
			case VULKAN_LIGHT_TYPE_DIRECTIONAL:
				struct_descriptor_add_field(&light->struct_definition, "direction", GLSL_TYPE_VEC3);
			break;
			case VULKAN_LIGHT_TYPE_POINT:
				struct_descriptor_add_field(&light->struct_definition, "position", GLSL_TYPE_VEC3);
			break;
			case VULKAN_LIGHT_TYPE_SPOT:
				struct_descriptor_add_field(&light->struct_definition, "angle", GLSL_TYPE_FLOAT);
				struct_descriptor_add_field(&light->struct_definition, "direction", GLSL_TYPE_VEC3);
				struct_descriptor_add_field(&light->struct_definition, "position", GLSL_TYPE_VEC3);
			break;
			case VULKAN_LIGHT_TYPE_AMBIENT:
			break;
			default:
				UNSUPPORTED_LIGHT_TYPE(light->type);
		};
	struct_descriptor_end(&light->struct_definition);

	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&light->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = light->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};

	vulkan_buffer_create_no_alloc(light->renderer, &create_info, &light->buffer);
	struct_descriptor_map(&light->struct_definition, vulkan_buffer_map(&light->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&light->renderer->global_set, VULKAN_DESCRIPTOR_BINDING_LIGHT, &light->buffer);

	// setup the field handles for faster access
	light->projection_handle = struct_descriptor_get_field_handle(&light->struct_definition, "projection");
	light->view_handle = struct_descriptor_get_field_handle(&light->struct_definition, "view");
	light->color_handle = struct_descriptor_get_field_handle(&light->struct_definition, "color");
	light->intensity_handle = struct_descriptor_get_field_handle(&light->struct_definition, "intensity");
	
	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			AUTO _light = CAST_TO(vulkan_directional_light_t*, light);
			_light->direction_handle = struct_descriptor_get_field_handle(&light->struct_definition, "direction");
		}
		break;
		case VULKAN_LIGHT_TYPE_POINT:
		{
			AUTO _light = CAST_TO(vulkan_point_light_t*, light);
			_light->position_handle = struct_descriptor_get_field_handle(&light->struct_definition, "position");
		}
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO _light = CAST_TO(vulkan_spot_light_t*, light);
			_light->angle_handle = struct_descriptor_get_field_handle(&light->struct_definition, "angle");
			_light->direction_handle = struct_descriptor_get_field_handle(&light->struct_definition, "direction");
			_light->position_handle = struct_descriptor_get_field_handle(&light->struct_definition, "position");
		}
		break;
		case VULKAN_LIGHT_TYPE_AMBIENT:
		break;
		default:
			UNSUPPORTED_LIGHT_TYPE(light->type);
	};
}

RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type)
{
	vulkan_light_t* light = vulkan_light_new(type);
	vulkan_light_create_no_alloc(renderer, type, light);
	return light;
}


static void vulkan_light_set_view(vulkan_light_t* light, mat4_t(float) view);
static void vulkan_light_set_projection(vulkan_light_t* light, mat4_t(float) projection);

RENDERER_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light)
{
	memzero(light, vulkan_light_t);

	light->renderer = renderer;
	light->position = vec3_zero(float)();
	light->euler_rotation = vec3_zero(float)();
	light->rotation = mat4_identity(float)();
	light->projection = mat4_identity(float)();
	light->view = mat4_identity(float)();
	light->color = vec3_one(float)();
	light->intensity = 1.0f;
	light->type = type;

	setup_gpu_resources(light);

	AUTO color = COLOR_WHITE;
	vulkan_light_set_color(light, REINTERPRET_TO(vec3_t(float), color));
	vulkan_light_set_intensity(light, 1.0f);
	vulkan_light_set_position(light, vec3(float)(0, 0.6f, -1.8f));
	vulkan_light_set_rotation(light, vec3(float)(0, -100 DEG, -10 DEG));
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_AMBIENT:
		break;
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
			vulkan_light_set_projection(light, mat4_ortho_projection(float)(0.04f, 10, 3, 1));
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
			vulkan_light_set_spot_angle(light, 30 DEG);
			vulkan_light_set_projection(light, mat4_persp_projection(float)(0.04f, 20.0f, CAST_TO(vulkan_spot_light_t*, light)->angle, 1));
		break;
		case VULKAN_LIGHT_TYPE_POINT:
			vulkan_light_set_projection(light, mat4_persp_projection(float)(0.04f, 20.0f, 65 DEG, 1));
		break;
		default:
			UNSUPPORTED_LIGHT_TYPE(type);
	}
}

RENDERER_API void vulkan_light_destroy(vulkan_light_t* light)
{
	struct_descriptor_unmap(&light->struct_definition);
	vulkan_buffer_unmap(&light->buffer);
	vulkan_buffer_destroy(&light->buffer);
}

RENDERER_API void vulkan_light_release_resources(vulkan_light_t* light)
{
	vulkan_buffer_release_resources(&light->buffer);
	// TODO
	// heap_free(light);
}

/* setters */

static void vulkan_light_set_view(vulkan_light_t* light, mat4_t(float) view)
{
	light->view = view;
	mat4_t(float) DTO = mat4_transpose(float)(light->view);
	struct_descriptor_set_mat4(&light->struct_definition, light->view_handle, CAST_TO(float*, &DTO));
}

static void vulkan_light_set_projection(vulkan_light_t* light, mat4_t(float) projection)
{
	light->projection = projection;
	mat4_t(float) DTO = mat4_transpose(float)(projection);
	struct_descriptor_set_mat4(&light->struct_definition, light->projection_handle, CAST_TO(float*, &DTO));
}

RENDERER_API void vulkan_light_set_spot_angle(vulkan_light_t* light, float angle)
{
	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_SPOT:
			AUTO _light = CAST_TO(vulkan_spot_light_t*, light);
			_light->angle = angle;
			struct_descriptor_set_float(&light->struct_definition, _light->angle_handle, &angle);
		break;
		default:
			log_wrn("You are trying to set spot angle for the light which isn't a spot light\n");
		break;
	}
}

RENDERER_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t(float) position)
{
	mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), light->rotation);
	vulkan_light_set_view(light, mat4_inverse(float)(transform));
	light->position = position;
	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_AMBIENT:
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO _light = CAST_TO(vulkan_spot_light_t*, light);
			struct_descriptor_set_vec3(&light->struct_definition, _light->position_handle, CAST_TO(float*, &position));
		}
		case VULKAN_LIGHT_TYPE_POINT:
		{
			AUTO _light = CAST_TO(vulkan_point_light_t*, light);
			struct_descriptor_set_vec3(&light->struct_definition, _light->position_handle, CAST_TO(float*, &position));
		}
		break;
		default:
			UNSUPPORTED_LIGHT_TYPE(light->type);
	}
}

RENDERER_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t(float) rotation)
{
	light->euler_rotation = rotation;
	mat4_move(float)(&light->rotation, mat4_rotation(float)(rotation.x, rotation.y, rotation.z));
	vec3_t(float) position = light->position;
	
	mat4_t(float) view = mat4_inverse(float)(mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), light->rotation));
	vulkan_light_set_view(light, view);

	switch(light->type)
	{
		case VULKAN_LIGHT_TYPE_DIRECTIONAL:
		{
			AUTO _light = CAST_TO(vulkan_directional_light_t*, light);
			vec4_t(float) dir = mat4_mul_vec4(float)(mat4_transpose(float)(view), 1, 0, 0, 0);
			_light->direction = vec3(float)(dir.x, dir.y, dir.z);
			struct_descriptor_set_vec3(&light->struct_definition, _light->direction_handle, CAST_TO(float*, &dir));
		}
		break;
		case VULKAN_LIGHT_TYPE_SPOT:
		{
			AUTO _light = CAST_TO(vulkan_spot_light_t*, light);
			vec4_t(float) dir = mat4_mul_vec4(float)(mat4_transpose(float)(view), 1, 0, 0, 0);
			_light->direction = vec3(float)(dir.x, dir.y, dir.z);
			struct_descriptor_set_vec3(&light->struct_definition, _light->direction_handle, CAST_TO(float*, &dir));
		}
		break;
	}
}

RENDERER_API void vulkan_light_set_intensity(vulkan_light_t* light, float intensity)
{
	light->intensity = intensity;
	struct_descriptor_set_float(&light->struct_definition, light->intensity_handle, &light->intensity);
}

RENDERER_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t(float) color)
{
	light->color = color;
	struct_descriptor_set_vec3(&light->struct_definition, light->color_handle, CAST_TO(float*, &light->color));
}
