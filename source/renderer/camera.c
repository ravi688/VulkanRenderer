
#include <renderer/camera.h>

#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/struct_descriptor.h>

// typedef struct camera_data_t
// {
// 	mat4_t(float) transform;
// 	mat4_t(float) projection;
// 	mat4_t(float) view;
// } camera_data_t;

typedef struct camera_t
{
	renderer_t* context;
	// camera_data_t data;
	vulkan_buffer_t buffer;
	struct_descriptor_t struct_definition;
	struct_field_handle_t transform_handle;
	struct_field_handle_t projection_handle;
	struct_field_handle_t view_handle;
} camera_t;

RENDERER_API camera_t* camera_new()
{
	camera_t* camera = heap_new(camera_t);
	memset(camera, 0, sizeof(camera_t));
	return camera;
}

static void setup_gpu_resources(camera_t* camera)
{
	// setup camera struct definiton
	strcpy(camera->struct_definition.name, "cameraInfo");
	struct_field_t* fields = heap_newv(struct_field_t, 3);
	memset(fields, 0, sizeof(struct_field_t) * 3);
	strcpy(fields[0].name, "transform");
	strcpy(fields[0].name, "projection");
	strcpy(fields[1].name, "view");
	for(int i = 0; i < 3; i++)
	{
		fields[i].type = STRUCT_FIELD_MAT4;
		fields[i].alignment = 16;
		fields[i].size = sizeof(float) * 16;
	}
	camera->struct_definition.fields = fields;
	camera->struct_definition.field_count = 3;
	struct_descriptor_recalculate(&camera->struct_definition);

	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&camera->struct_definition),
		.usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharing_mode = camera->renderer->handle->sharing_mode,
		.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	}
	assert(create_info.size == (64 * 3));
	vulkan_buffer_create_no_alloc(camera->renderer->handle, &camera->buffer);
	struct_descriptor_map(&camera->buffer, vulkan_buffer_map(&camera->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&camera->global_set, camera->renderer->handle, VULKAN_DESCRIPTOR_SET_GLOBAL_CAMERA, &camera->buffer);

	camera->transform_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "transform");
	camera->projection_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "projection");
	camera->view_handle = struct_descriptor_get_field_handle(&camear->struct_definition, "view");
}

RENDERER_API camera_t* camera_create(renderer_t* context, projection_type_t projection_type, float height_or_angle)
{
	// for now only one camera could be in the entire application
	ASSERT_CALLED_ONCE();

	render_window_t* window = renderer_get_window(context);
	camera_t* camera = camera_new();
	camera->context = context;
	mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(-1.8f, 0.6f, 0), mat4_rotation(float)(0, 0, -22 * DEG2RAD));
	mat4_t(float) projection = mat4_persp_projection(float)(0.04f, 100, 60 * DEG2RAD, (float)window->width / (float)window->height);
	mat4_t(float) view = mat4_inverse(float)(camera->transform);

	setup_gpu_resources(camera);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, &transform);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->projection_handle, &projection);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->view_handle, &view);
	return camera;
}

RENDERER_API void camera_destroy(camera_t* camera)
{
	struct_descriptor_unmap(&camera->struct_definition);
	vulkan_buffer_unmap(&camera->buffer);
	vulkan_buffer_destroy(&camera->buffer);
}

RENDERER_API void camera_release_resources(camera_t* camera)
{
	vulkan_buffer_release_resources(&camera->buffer);
	heap_free(camera->struct_definition.fields);
	heap_free(camera);
}

/* getters */
RENDERER_API vec3_t(float) camera_get_position(camera_t* camera)
{
	mat4_t(float) m;
	struct_descriptor_get_mat4(&camera->struct_definition, camera->transform_handle, &m);
	return vec3_t(float)(m.m03, m.m13, m.m23);
}

RENDERER_API vec3_t(float) camera_get_rotation(camera_t* camera)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API float camera_get_aspect_ratio(camera_t* camera)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API float camera_get_field_of_view(camera_t* camera)
{
	NOT_IMPLEMENTED_FUNCTION();
}

/* setters */
RENDERER_API void camera_set_position(camera_t* camera, vec3_t(float) position)
{
	mat4_t(float) m;
	struct_descriptor_get_mat4(&camera->struct_definition, camera->transform_handle, &m);
	m.m03 = position.x;
	m.m13 = position.y;
	m.m23 = position.z;
	struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, &m);
}

RENDERER_API void camera_set_rotation(camera_t* camera, vec3_t(float) rotation)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void camera_set_aspect_ratio(camera_t* camera, float aspect_ratio)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov)
{
	NOT_IMPLEMENTED_FUNCTION();
}

