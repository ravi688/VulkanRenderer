
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

RENDERER_API vulkan_camera_t* vulkan_camera_new()
{
	vulkan_camera_t* camera = heap_new(vulkan_camera_t);
	memset(camera, 0, sizeof(vulkan_camera_t));
	return camera;
}

static void setup_gpu_resources(vulkan_camera_t* camera)
{
	// setup camera struct definiton
	struct_descriptor_begin(&camera->struct_definition, "cameraInfo", GLSL_TYPE_BLOCK);
		struct_descriptor_add_field_mat4(&camera->struct_definition, "transform");
		struct_descriptor_add_field_mat4(&camera->struct_definition, "projection");
		struct_descriptor_add_field_mat4(&camera->struct_definition, "view");
	struct_descriptor_end(&camera->struct_definition);

	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&camera->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = camera->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	assert(create_info.size == (64 * 3));
	vulkan_buffer_create_no_alloc(camera->renderer, &create_info, &camera->buffer);
	struct_descriptor_map(&camera->struct_definition, vulkan_buffer_map(&camera->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&camera->renderer->global_set, VULKAN_DESCRIPTOR_BINDING_CAMERA, &camera->buffer);

	camera->transform_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "transform");
	camera->projection_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "projection");
	camera->view_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "view");
}

static void recreate_projection(render_window_t* window, void* user_data)
{
	vulkan_camera_t* camera = user_data;
	mat4_t(float) projection = mat4_persp_projection(float)(camera->near_clip_plane, camera->far_clip_plane, camera->field_of_view, (float)window->width/window->height);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->projection_handle, CAST_TO(float*, &projection));
	// mat4_move(float)(&projectionMatrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
	// mat4_move(float)(&game->screenSpaceMatrix, mat4_ortho_projection(float)(-0.04f, 100, window->height, (float)window->width / window->height));
}

RENDERER_API vulkan_camera_t* vulkan_camera_create(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info)
{
	vulkan_camera_t* camera = vulkan_camera_new();
	vulkan_camera_create_no_alloc(renderer, create_info, camera);
	return camera;
}

RENDERER_API void vulkan_camera_create_no_alloc(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info, vulkan_camera_t OUT camera)
{
	// for now only one camera could be in the entire application
	ASSERT_CALLED_ONCE();

	memzero(camera, vulkan_camera_t);

	camera->renderer = renderer;
	camera->far_clip_plane = create_info->far_clip_plane;
	camera->near_clip_plane = create_info->near_clip_plane;
	camera->field_of_view = create_info->field_of_view;
	camera->default_render_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->default_render_pass);

	render_window_t* window = vulkan_renderer_get_window(renderer);

	setup_gpu_resources(camera);

	// mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(0, 0.6f, -1.8f), mat4_rotation(float)(0, -90 * DEG2RAD, -22 * DEG2RAD));
	// mat4_t(float) projection = mat4_ortho_projection(float)(0.04f, 20, 5, 1);
	mat4_t(float) transform = mat4_mul(float)(2, mat4_translation(float)(-1.8f, 0.6f, 0), mat4_rotation(float)(0, 0, -22 * DEG2RAD));
	mat4_t(float) projection = mat4_persp_projection(float)(create_info->near_clip_plane, create_info->far_clip_plane, create_info->field_of_view, (float)window->width / (float)window->height);
	mat4_t(float) view = mat4_inverse(float)(transform);

	mat4_move(float)(&transform, mat4_transpose(float)(transform));
	mat4_move(float)(&projection, mat4_transpose(float)(projection));
	mat4_move(float)(&view, mat4_transpose(float)(view));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &transform));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->projection_handle, CAST_TO(float*, &projection));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->view_handle, CAST_TO(float*, &view));

	render_window_subscribe_on_resize(vulkan_renderer_get_window(renderer), recreate_projection, camera);

	log_msg("Vulkan Camera has been created successfully\n");
}

RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera)
{
	struct_descriptor_unmap(&camera->struct_definition);
	vulkan_buffer_unmap(&camera->buffer);
	vulkan_buffer_destroy(&camera->buffer);
	log_msg("Vulkan Camera has been destroyed successfully\n");
}

RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera)
{
	vulkan_buffer_release_resources(&camera->buffer);
	// TODO
	// heap_free(camera);
}

/* logic functions */

RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth)
{
	vulkan_render_pass_set_clear(camera->default_render_pass, color, depth);
}

RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_queue_t* queue)
{
	vulkan_render_pass_begin(camera->default_render_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN);
	vulkan_render_pass_end(camera->default_render_pass);
}

/* getters */
RENDERER_API vec3_t(float) vulkan_camera_get_position(vulkan_camera_t* camera)
{
	mat4_t(float) m;
	struct_descriptor_get_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &m));
	return vec3(float)(m.m03, m.m13, m.m23);
}

RENDERER_API vec3_t(float) vulkan_camera_get_rotation(vulkan_camera_t* camera)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API vec2_t(float) vulkan_camera_get_clip_planes(vulkan_camera_t* camera)
{
	return vec2(float)(camera->near_clip_plane, camera->far_clip_plane);
}

RENDERER_API float vulkan_camera_get_field_of_view(vulkan_camera_t* camera)
{
	return camera->field_of_view;
}

/* setters */
RENDERER_API void vulkan_camera_set_position(vulkan_camera_t* camera, vec3_t(float) position)
{
	mat4_t(float) m;
	struct_descriptor_get_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &m));
	m.m03 = position.x;
	m.m13 = position.y;
	m.m23 = position.z;
	struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &m));
	mat4_t(float) v = mat4_inverse(float)(m);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->view_handle, CAST_TO(float*, &v));
}

RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t(float) rotation)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void vulkan_camera_set_clip_planes(vulkan_camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	camera->near_clip_plane = near_clip_plane;
	camera->far_clip_plane = far_clip_plane;
	recreate_projection(vulkan_renderer_get_window(camera->renderer), camera);
}

RENDERER_API void vulkan_camera_set_field_of_view(vulkan_camera_t* camera, float fov)
{
	camera->field_of_view = fov;
	recreate_projection(vulkan_renderer_get_window(camera->renderer), camera);
}

