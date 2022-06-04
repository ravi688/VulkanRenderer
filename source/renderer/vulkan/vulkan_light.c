
#include <renderer/internal/vulkan/vulkan_light.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

// typedef struct vulkan_light_data_t
// {
// 	mat4_t(float) transform;
// 	mat4_t(float) projection;
// 	mat4_t(float) view;
// 	vec3_t(float) dir;
// 	vec3_t(float) color;
// 	float intensity;
// } vulkan_light_data_t;

/* constructors & destructors */
RENDERER_API vulkan_light_t* vulkan_light_new()
{
	vulkan_light_t* light = heap_new(vulkan_light_t);
	memset(light, 0, sizeof(vulkan_light_t));
	return light;
}

static void setup_gpu_resources(vulkan_light_t* light)
{
	// setup light struct definition
	strcpy(light->struct_definition.name, "lightInfo");
	struct_field_t* fields = heap_newv(struct_field_t, 6);
	memset(fields, 0, sizeof(struct_field_t) * 6);
	strcpy(fields[0].name, "transform");
	strcpy(fields[1].name, "projection");
	strcpy(fields[2].name, "view");
	strcpy(fields[3].name, "dir");
	strcpy(fields[4].name, "color");
	strcpy(fields[5].name, "intensity");
	for(int i = 0; i < 3; i++)
	{
		fields[i].type = STRUCT_FIELD_MAT4;
		fields[i].alignment = 16;
		fields[i].size = sizeof(float) * 16;
		if(i < 2)
		{
			fields[i + 3].type = STRUCT_FIELD_VEC3;
			fields[i + 3].alignment = 16;
			fields[i + 3].size = sizeof(float) * 3;
		}
	}
	fields[5].type = STRUCT_FIELD_FLOAT;
	fields[5].alignment = 4;
	fields[5].size = sizeof(float);
	light->struct_definition.fields = fields;
	light->struct_definition.field_count = 6;
	struct_descriptor_recalculate(&light->struct_definition);

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
	light->transform_handle = struct_descriptor_get_field_handle(&light->struct_definition, "transform");
	light->projection_handle = struct_descriptor_get_field_handle(&light->struct_definition, "projection");
	light->view_handle = struct_descriptor_get_field_handle(&light->struct_definition, "view");
	light->dir_handle = struct_descriptor_get_field_handle(&light->struct_definition, "dir");
	light->color_handle = struct_descriptor_get_field_handle(&light->struct_definition, "color");
	light->intensity_handle = struct_descriptor_get_field_handle(&light->struct_definition, "intensity");
}

RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type)
{
	vulkan_light_t* light = vulkan_light_new();
	vulkan_light_create_no_alloc(renderer, type, light);
	return light;
}

RENDERER_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light)
{
	memzero(light, vulkan_light_t);

	light->renderer = renderer;

	setup_gpu_resources(light);

	mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(0, 0.6f, -1.8f), mat4_rotation(float)(0, -90 * DEG2RAD, -22 * DEG2RAD));
	mat4_t(float) projection = mat4_ortho_projection(float)(0.04f, 10, 3, 1);
	mat4_t(float) view = mat4_inverse(float)(transform);
	vec3_t(float) dir = vec3_normalize(float)(vec3(float)(0, -1, 1));
	vec3_t(float) color = vec3_one(float)();
	float intensity = 1.0f;

	mat4_move(float)(&transform, mat4_transpose(float)(transform));
	mat4_move(float)(&projection, mat4_transpose(float)(projection));
	mat4_move(float)(&view, mat4_transpose(float)(view));
	struct_descriptor_set_mat4(&light->struct_definition, light->transform_handle, CAST_TO(float*, &transform));
	struct_descriptor_set_mat4(&light->struct_definition, light->projection_handle, CAST_TO(float*, &projection));
	struct_descriptor_set_mat4(&light->struct_definition, light->view_handle, CAST_TO(float*, &view));
	struct_descriptor_set_vec3(&light->struct_definition, light->dir_handle, CAST_TO(float*, &dir));
	struct_descriptor_set_vec3(&light->struct_definition, light->color_handle, CAST_TO(float*, &color));
	struct_descriptor_set_float(&light->struct_definition, light->intensity_handle, CAST_TO(float*, &intensity));
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
	heap_free(light->struct_definition.fields);
	// TODO
	// heap_free(light);
}

/* setters */
RENDERER_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t(float) position)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t(float) rotation)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_light_set_intensity(vulkan_light_t* lighit, float intensity)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t(float) color)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_light_set_projection(vulkan_light_t* light, mat4_t(float) projection)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_light_set_direction(vulkan_light_t* light, vec3_t(float) dir)
{
	NOT_IMPLEMENTED_FUNCTION();
}

