
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
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
		struct_descriptor_add_field(&camera->struct_definition, "transform", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "projection", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "view", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "screen", GLSL_TYPE_MAT4);
	struct_descriptor_end(&camera->struct_definition);

	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&camera->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = camera->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	assert(create_info.size == (64 * 4));
	vulkan_buffer_create_no_alloc(camera->renderer, &create_info, &camera->buffer);
	struct_descriptor_map(&camera->struct_definition, vulkan_buffer_map(&camera->buffer));
	
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = camera->renderer->vo_descriptor_pool,
		.layout = &camera->renderer->camera_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(camera->renderer, &set_create_info, &camera->set);

	vulkan_descriptor_set_write_uniform_buffer(&camera->set, VULKAN_DESCRIPTOR_BINDING_CAMERA_PROPERTIES, &camera->buffer);

	camera->transform_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "transform");
	camera->projection_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "projection");
	camera->view_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "view");
	camera->screen_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "screen");
}

static void vulkan_camera_set_screen_projection(vulkan_camera_t* camera, mat4_t(float) screen)
{
	camera->screen = screen;
	mat4_move(float)(&screen, mat4_transpose(float)(screen));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->screen_handle, CAST_TO(float*, &screen));
}

static void vulkan_camera_set_projection(vulkan_camera_t* camera, mat4_t(float) projection)
{
	camera->projection = projection;
	mat4_move(float)(&projection, mat4_transpose(float)(projection));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->projection_handle, CAST_TO(float*, &projection));
}

static void vulkan_camera_recalculate_projection(vulkan_camera_t* camera)
{
	render_window_t* window = vulkan_renderer_get_window(camera->renderer);
	mat4_t(float) projection;
	switch(camera->projection_type)
	{
		case VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE:
			mat4_move(float)(&projection, mat4_persp_projection(float)(camera->near_clip_plane, 
																	   camera->far_clip_plane, 
																	   camera->field_of_view, 
																	   (float)window->width / (float)window->height));
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC:
			mat4_move(float)(&projection, mat4_ortho_projection(float)(camera->near_clip_plane, 
																	   camera->far_clip_plane, 
																	   camera->height, 
																	   (float)window->width / (float)window->height));
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC:
			LOG_FETAL_ERR("VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC isn't supported yet!\n");
			break;
		default:
			LOG_FETAL_ERR("Unsupported camera projection type: %u\n", camera->projection_type);
	}
	vulkan_camera_set_projection(camera, projection);
}

static void vulkan_camera_recalculate_transform(vulkan_camera_t* camera)
{
	vulkan_camera_set_transform(camera, mat4_mul(float)(2,
		mat4_translation(float)(camera->position.x, camera->position.y, camera->position.z),
		mat4_rotation(float)(camera->rotation.x, camera->rotation.y, camera->rotation.z)));
}

static void vulkan_camera_recalculate_screen_projection(vulkan_camera_t* camera)
{
	render_window_t* window = vulkan_renderer_get_window(camera->renderer);
	vulkan_camera_set_screen_projection(camera, 
		mat4_ortho_projection(float)(-0.04f, 100, window->height, (float)window->width / window->height));
}

static void recreate_screen_projection(render_window_t* window, void* user_data)
{
	vulkan_camera_recalculate_screen_projection(CAST_TO(vulkan_camera_t*, user_data));
}

static void recreate_projection(render_window_t* window, void* user_data)
{
	vulkan_camera_recalculate_projection(CAST_TO(vulkan_camera_t*, user_data));
}

RENDERER_API vulkan_camera_t* vulkan_camera_create(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info)
{
	vulkan_camera_t* camera = vulkan_camera_new();
	vulkan_camera_create_no_alloc(renderer, create_info, camera);
	return camera;
}

RENDERER_API void vulkan_camera_create_no_alloc(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info, vulkan_camera_t OUT camera)
{
	memzero(camera, vulkan_camera_t);

	camera->renderer = renderer;
	camera->default_render_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->default_render_pass);
	camera->projection_type = create_info->projection_type;
	camera->is_active = true;
	camera->framebuffers = buf_create(sizeof(vulkan_framebuffer_t), 1, 0);

	render_window_t* window = vulkan_renderer_get_window(renderer);

	setup_gpu_resources(camera);

	camera->near_clip_plane = 0.04f;
	camera->far_clip_plane = 20.0f;
	switch(create_info->projection_type)
	{
		case VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE:
			camera->field_of_view = 65 DEG;
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC:
			camera->height = 5;
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC:
			LOG_FETAL_ERR("VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC isn't supported yet!\n");
			break;
		default:
			LOG_FETAL_ERR("Unsupported camera projection type: %u\n", create_info->projection_type);
	}
	vulkan_camera_recalculate_projection(camera);
	vulkan_camera_recalculate_screen_projection(camera);

	camera->position = vec3(float)(-1.8f, 0.6f, 0);
	camera->rotation = vec3(float)(0, 0, -22 DEG);
	vulkan_camera_recalculate_transform(camera);

	render_window_subscribe_on_resize(vulkan_renderer_get_window(renderer), recreate_projection, camera);
	render_window_subscribe_on_resize(vulkan_renderer_get_window(renderer), recreate_screen_projection, camera);

	log_msg("Vulkan Camera has been created successfully\n");
}

RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera)
{
	vulkan_descriptor_set_destroy(&camera->set);
	struct_descriptor_unmap(&camera->struct_definition);
	vulkan_buffer_unmap(&camera->buffer);
	vulkan_buffer_destroy(&camera->buffer);
	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	for(buf_ucount_t i = 0; i < count; i++)
		vulkan_framebuffer_destroy(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(&camera->framebuffers, i)));
	log_msg("Vulkan Camera has been destroyed successfully\n");
}

RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera)
{
	vulkan_descriptor_set_release_resources(&camera->set);
	vulkan_buffer_release_resources(&camera->buffer);
	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	for(buf_ucount_t i = 0; i < count; i++)
		vulkan_framebuffer_release_resources(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(&camera->framebuffers, i)));
	buf_free(&camera->framebuffers);
	// TODO
	// heap_free(camera);
}

/* logic functions */

RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth)
{
	vulkan_render_pass_set_clear(camera->default_render_pass, color, depth);
}

RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_texture_t* texture)
{
	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	switch(target_type)
	{
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_SWAPCHAIN:
			for(buf_ucount_t i = 0; i < count; i++)
				vulkan_framebuffer_restore_supplementary(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(&camera->framebuffers, i)));
		break;
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_TEXTURE:
			for(buf_ucount_t i = 0; i < count; i++)
				vulkan_framebuffer_set_supplementary(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(&camera->framebuffers, i)), 
					CAST_TO(vulkan_attachment_t*, texture));
		break;
	}
}

RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene)
{
	vulkan_render_pass_begin(camera->default_render_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
	vulkan_render_pass_end(camera->default_render_pass);
	if(scene != NULL)
		vulkan_render_scene_render(scene);
}

RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* target)
{

}

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_framebuffer_list_handle_t handle)
{
	assert(handle != VULKAN_FRAMEBUFFER_LIST_HANDLE_INVALID);
	return CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(&camera->framebuffers, handle));
}

RENDERER_API vulkan_framebuffer_list_handle_t vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* pass)
{
	if(pass->required_framebuffer_count <= 0)
		return VULKAN_FRAMEBUFFER_LIST_HANDLE_INVALID;
	buf_ucount_t handle = buf_get_element_count(&camera->framebuffers);
	for(u32 i = 0; i < pass->required_framebuffer_count; i++)
	{
		buf_push_pseudo(&camera->framebuffers, 1);
		vulkan_framebuffer_create_no_alloc(camera->renderer, pass, i, CAST_TO(vulkan_framebuffer_t*, buf_peek_ptr(&camera->framebuffers)));
	}
	return handle;
}

/* getters */

RENDERER_API mat4_t(float) vulkan_camera_get_view(vulkan_camera_t* camera)
{
	return camera->view;
}

RENDERER_API mat4_t(float) vulkan_camera_get_projection(vulkan_camera_t* camera)
{
	return camera->projection;
}

RENDERER_API mat4_t(float) vulkan_camera_get_transform(vulkan_camera_t* camera)
{
	return camera->transform;
}

RENDERER_API vec3_t(float) vulkan_camera_get_position(vulkan_camera_t* camera)
{
	return camera->position;
}

RENDERER_API vec3_t(float) vulkan_camera_get_rotation(vulkan_camera_t* camera)
{
	return camera->rotation;
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

RENDERER_API void vulkan_camera_set_transform(vulkan_camera_t* camera, mat4_t(float) transform)
{
	camera->transform = transform;
	mat4_t(float) m = mat4_transpose(float)(camera->transform);
	struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &m));
	camera->view = mat4_inverse(float)(transform);
	mat4_move(float)(&m, mat4_transpose(float)(camera->view));
	struct_descriptor_set_mat4(&camera->struct_definition, camera->view_handle, CAST_TO(float*, &m));
}

RENDERER_API void vulkan_camera_set_position(vulkan_camera_t* camera, vec3_t(float) position)
{
	camera->position = position;
	vulkan_camera_recalculate_transform(camera);
}

RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t(float) rotation)
{
	camera->rotation = rotation;
	vulkan_camera_recalculate_transform(camera);
}

RENDERER_API void vulkan_camera_set_clip_planes(vulkan_camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	camera->near_clip_plane = near_clip_plane;
	camera->far_clip_plane = far_clip_plane;
	vulkan_camera_recalculate_projection(camera);
}

RENDERER_API void vulkan_camera_set_field_of_view(vulkan_camera_t* camera, float fov)
{
	camera->field_of_view = fov;
	vulkan_camera_recalculate_projection(camera);
}

