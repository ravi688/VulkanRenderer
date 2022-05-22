
#include <renderer/internal/vulkan/vulkan_light.h>
#include <renderer/memory_allocator.h>

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
	strcpy(light->struct_definiton.name, "lightInfo");
	fields = heap_newv(struct_field_t, 6);
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
	light->struct_definiton.fields = fields;
	light->struct_definiton.field_count = 6;
	struct_descriptor_recalculate(&light->struct_definiton);

	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&light->struct_definiton),
		.usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharing_mode = light->renderer->handle->sharing_mode,
		.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	}
	vulkan_buffer_create_no_alloc(light->renderer->handle, &light->buffer);
	struct_descriptor_map(&light->buffer, vulkan_buffer_map(&light->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&light->renderer->handle->global_set, light->renderer->handle, VULKAN_DESCRIPTOR_SET_GLOBAL_LIGHT, &light->buffer);

	// setup the field handles for faster access
	light->transform_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "transform");
	light->projection_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "projection");
	light->view_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "view");
	light->dir_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "dir");
	light->color_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "color");
	light->intensity_handle = struct_descriptor_get_field_handle(&light->struct_definiton, "intensity");
}

RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer)
{
	vulkan_light_t* light = vulkan_light_new();
	vulkan_light_create_no_alloc(renderer, light);
	return light;
}

RENDERER_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_t OUT light)
{
	light->renderer = renderer;

	setup_gpu_resources(light);

	mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(-4.0f, 4.0f, 0 ), mat4_rotation(float)(-20 DEG, -90 DEG, 0));
	mat4_t(float) projection = mat4_ortho_projection(float)(5, 20, 10, 1);
	mat4_t(float) view = mat4_inverse(float)(transform);
	vec3_t(float) dir = vec3_normalize(float)(vec3(float)(1, -1, 3));
	vec3_t(float) color = vec3_one(float)();
	float intensity = 1.0f;
	struct_descriptor_set_mat4(&light->struct_definiton, light->transform_handle, &transform);
	struct_descriptor_set_mat4(&light->struct_definiton, light->projection_handle, &projection);
	struct_descriptor_set_mat4(&light->struct_definiton, light->view_handle, &view);
	struct_descriptor_set_vec3(&light->struct_definition, light->dir_handle, &dir);
	struct_descriptor_set_vec3(&light->struct_definition, light->color_handle, &color);
	struct_descriptor_set_float(&light->struct_definition, light->intensity_handle, &intensity);
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
	heap_free(light);
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

