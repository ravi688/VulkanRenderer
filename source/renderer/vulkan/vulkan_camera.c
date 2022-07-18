
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

#include <hpml/vec4.h>
#include <hpml/affine_transformation.h>

RENDERER_API vulkan_camera_t* vulkan_camera_new()
{
	vulkan_camera_t* camera = heap_new(vulkan_camera_t);
	memset(camera, 0, sizeof(vulkan_camera_t));
	return camera;
}

static void* create_buffer_and_map(vulkan_camera_t* camera, vulkan_buffer_t OUT buffer, vulkan_descriptor_set_t OUT set)
{
	// create uniform buffers and write to the descriptor set GLOBAL_SET at bindings GLOBAL_CAMERA and GLOBAL_LIGHT
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&camera->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = camera->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	assert(create_info.size == (64 * 4));
	vulkan_buffer_create_no_alloc(camera->renderer, &create_info, buffer);

	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = camera->renderer->vo_descriptor_pool,
		.layout = &camera->renderer->camera_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(camera->renderer, &set_create_info, set);
	vulkan_descriptor_set_write_uniform_buffer(set, VULKAN_DESCRIPTOR_BINDING_CAMERA_PROPERTIES, buffer);

	return vulkan_buffer_map(buffer);
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

	camera->transform_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "transform");
	camera->projection_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "projection");
	camera->view_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "view");
	camera->screen_handle = struct_descriptor_get_field_handle(&camera->struct_definition, "screen");
	
	camera->buffer_mappings[0] = create_buffer_and_map(camera, &camera->buffers[0], &camera->sets[0]);
	camera->buffer_count = 1;
}

void update_mat4(vulkan_camera_t* camera, struct_field_handle_t field_handle, float* data)
{
	for(u32 i = 0; i < camera->max_shot_count; i++)
	{
		struct_descriptor_map(&camera->struct_definition, camera->buffer_mappings[i]);
		struct_descriptor_set_mat4(&camera->struct_definition, field_handle, data);
	}
}

static void vulkan_camera_set_screen_projection(vulkan_camera_t* camera, mat4_t screen)
{
	camera->screen = screen;
	mat4_move(screen, mat4_transpose(screen));
	update_mat4(camera, camera->screen_handle, CAST_TO(float*, &screen));
}

static void vulkan_camera_set_projection(vulkan_camera_t* camera, mat4_t projection)
{
	camera->projection = projection;
	mat4_move(projection, mat4_transpose(projection));
	update_mat4(camera, camera->projection_handle, CAST_TO(float*, &projection));
}

static void vulkan_camera_recalculate_projection(vulkan_camera_t* camera)
{
	render_window_t* window = vulkan_renderer_get_window(camera->renderer);
	mat4_t projection;
	switch(camera->projection_type)
	{
		case VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE:
			mat4_move(projection, mat4_persp_projection(camera->near_clip_plane, 
																	   camera->far_clip_plane, 
																	   camera->field_of_view, 
																	   (float)window->width / (float)window->height));
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC:
			mat4_move(projection, mat4_ortho_projection(camera->near_clip_plane, 
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
	vulkan_camera_set_transform(camera, mat4_mul(2,
		mat4_translation(camera->position.x, camera->position.y, camera->position.z),
		mat4_rotation(camera->rotation.x, camera->rotation.y, camera->rotation.z)));
}

static void vulkan_camera_recalculate_screen_projection(vulkan_camera_t* camera)
{
	render_window_t* window = vulkan_renderer_get_window(camera->renderer);
	vulkan_camera_set_screen_projection(camera, 
		mat4_ortho_projection(-0.04f, 100, window->height, (float)window->width / window->height));
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
	camera->framebuffers = buf_create(sizeof(BUFFER), 1, 0);
	camera->shot_taken = 0;
	camera->current_shot_index = 0;

	// create framebuffer list for the shot 1 initially
	camera->max_shot_count = 1;
	BUFFER buffer = buf_new(vulkan_framebuffer_t);
	buf_push(&camera->framebuffers, &buffer);

	camera->clear_buffer = heap_newv(VkClearValue, camera->default_render_pass->attachment_count);
	memcpy(camera->clear_buffer, camera->default_render_pass->vo_clear_values, sizeof(VkClearValue) * camera->default_render_pass->attachment_count);

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

	camera->position = vec3(-1.8f, 0.6f, 0);
	camera->rotation = vec3(0, 0, -22 DEG);
	vulkan_camera_recalculate_transform(camera);

	render_window_subscribe_on_resize(vulkan_renderer_get_window(renderer), recreate_projection, camera);
	render_window_subscribe_on_resize(vulkan_renderer_get_window(renderer), recreate_screen_projection, camera);


	// register all the render passes with this camera
	vulkan_render_pass_pool_t* pass_pool = renderer->render_pass_pool;
	buf_ucount_t pass_count = vulkan_render_pass_pool_get_count(pass_pool);
	for(buf_ucount_t i = 0; i < pass_count; i++)
	{
		vulkan_render_pass_t* pass = vulkan_render_pass_pool_get_at(pass_pool, i);
		pass->framebuffer_list_handle = vulkan_camera_register_render_pass(camera, pass);
	}

	log_msg("Vulkan Camera has been created successfully\n");
}

RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera)
{
	struct_descriptor_unmap(&camera->struct_definition);
	for(u32 i = 0; i < camera->buffer_count; i++)
	{
		vulkan_descriptor_set_destroy(&camera->sets[i]);
		vulkan_buffer_unmap(&camera->buffers[i]);
		vulkan_buffer_destroy(&camera->buffers[i]);
	}
	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		BUFFER* buffer = CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, i));
		buf_ucount_t count1 = buf_get_element_count(buffer);
		for(buf_ucount_t j = 0; j < count1; j++)
			vulkan_framebuffer_destroy(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(buffer, j)));
	}
	log_msg("Vulkan Camera has been destroyed successfully\n");
}

RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera)
{
	for(u32 i = 0; i < camera->buffer_count; i++)
	{
		vulkan_descriptor_set_release_resources(&camera->sets[i]);
		vulkan_buffer_release_resources(&camera->buffers[i]);
	}
	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		BUFFER* buffer = CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, i));
		buf_ucount_t count1 = buf_get_element_count(buffer);
		for(buf_ucount_t j = 0; j < count1; j++)
			vulkan_framebuffer_release_resources(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(buffer, j)));
		buf_free(buffer);
	}
	buf_free(&camera->framebuffers);
	// TODO
	// heap_free(camera);
}

static void transition_target_layout_for_write(VkFormat format, vulkan_image_view_t* view)
{
	VkCommandBuffer cb = view->renderer->vo_command_buffers[view->renderer->swapchain->current_image_index];
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* oldLayout: */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				/* newLayout: */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_SHADER_READ_BIT,
				/* dstAccess: */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			break;
		case VK_FORMAT_D32_SFLOAT:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* oldLayout: */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				/* newLayout: */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_SHADER_READ_BIT,
				/* dstAccess: */ VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
			break;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any possible render target layout\n", format);
	}
}

static void transition_target_layout_for_sample(VkFormat format, vulkan_image_view_t* view)
{
	VkCommandBuffer cb = view->renderer->vo_command_buffers[view->renderer->swapchain->current_image_index];
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* oldLayout: */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				/* newLayout: */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				/* dstAccess: */ VK_ACCESS_SHADER_READ_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
			break;
		case VK_FORMAT_D32_SFLOAT:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* oldLayout: */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				/* newLayout: */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				/* dstAccess: */ VK_ACCESS_SHADER_READ_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
			break;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any possible render target layout\n", format);
	}
}

/* logic functions */
RENDERER_API void vulkan_camera_begin(vulkan_camera_t* camera)
{
	if(vulkan_camera_is_offscreen(camera))
	{
		if(camera->color_render_target != VULKAN_CAMERA_RENDER_TARGET_SCREEN)
		{
			if((camera->color_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
			{
				for(u32 i = 0; i < 6; i++)
					transition_target_layout_for_write(camera->color_render_target->image.vo_format, &camera->color_render_target->image_views[i]);
			}
			else transition_target_layout_for_write(camera->color_render_target->image.vo_format, &camera->color_render_target->image_view);
		}

		if(camera->depth_render_target != VULKAN_CAMERA_RENDER_TARGET_SCREEN)
		{
			if((camera->depth_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
			{
				for(u32 i = 0; i < 6; i++)
					transition_target_layout_for_write(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_views[i]);
			}
			else transition_target_layout_for_write(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_view);
		}
	}
	camera->current_shot_index = 0;
	camera->shot_taken = 0;
}

RENDERER_API bool vulkan_camera_capture(vulkan_camera_t* camera, u32 clear_flags)
{
	bool is_capture = camera->shot_taken < camera->max_shot_count;

	if(!is_capture)
		return false;

	camera->current_shot_index = camera->shot_taken;
	camera->shot_taken++;

	switch(clear_flags)
	{
		case VULKAN_CAMERA_CLEAR_FLAG_CLEAR:
			AUTO clear_buffer = camera->default_render_pass->vo_clear_values;
			camera->default_render_pass->vo_clear_values = camera->clear_buffer;
			vulkan_render_pass_begin(camera->default_render_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
			vulkan_render_pass_end(camera->default_render_pass);
			camera->default_render_pass->vo_clear_values = clear_buffer;
		break;
	}

	return true;
}

RENDERER_API void vulkan_camera_end(vulkan_camera_t* camera)
{
	if(vulkan_camera_is_offscreen(camera))
	{
		if(camera->color_render_target != VULKAN_CAMERA_RENDER_TARGET_SCREEN)
		{
			if((camera->color_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
			{
				for(u32 i = 0; i < 6; i++)
					transition_target_layout_for_sample(camera->color_render_target->image.vo_format, &camera->color_render_target->image_views[i]);
			}
			else transition_target_layout_for_sample(camera->color_render_target->image.vo_format, &camera->color_render_target->image_view);
		}

		if(camera->depth_render_target != VULKAN_CAMERA_RENDER_TARGET_SCREEN)
		{
			if((camera->depth_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
			{
				for(u32 i = 0; i < 6; i++)
					transition_target_layout_for_sample(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_views[i]);
			}
			else transition_target_layout_for_sample(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_view);
		}
	}
}

RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth)
{
	vulkan_render_pass_set_clear_indirect(camera->default_render_pass, color, depth, camera->clear_buffer);
}

RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_texture_t* texture)
{
	if((texture != VULKAN_CAMERA_RENDER_TARGET_SCREEN) && ((texture->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE))
	{
		camera->max_shot_count = 6;
		if(camera->shot_rotations == NULL)
		{
			camera->shot_rotations = heap_newv(vec3_t, camera->max_shot_count);
			camera->shot_rotations[0] = vec3(0, -90 DEG, 0); // right
			camera->shot_rotations[1] = vec3(0, 90 DEG, 0); 	// left
			camera->shot_rotations[2] = vec3(0, 0, 90 DEG); // bottom
			camera->shot_rotations[3] = vec3(0, 0, -90 DEG); 	// top
			camera->shot_rotations[4] = vec3(0, 0, 0); 		// front
			camera->shot_rotations[5] = vec3(0, 180 DEG, 0); // back

			/* NOTE: we can't get the pointer to the peek element because the buffer will resize itself
				upon the push so the ptr will become invalid to access */
			buf_ucount_t peek_handle = buf_get_element_count(&camera->framebuffers) - 1;
			buf_ucount_t count = buf_get_element_count(buf_peek_ptr(&camera->framebuffers));
			for(u32 i = 0; i < 5; i++)
			{
				BUFFER buffer = buf_new(vulkan_framebuffer_t);
				for(u32 j = 0; j < count; j++)
				{
					buf_push_pseudo(&buffer, 1);
					vulkan_framebuffer_t* framebuffer = CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, peek_handle)), j));
					vulkan_framebuffer_create_no_alloc(camera->renderer, framebuffer->pass, framebuffer->id, CAST_TO(vulkan_framebuffer_t*, buf_peek_ptr(&buffer)));
				}
				buf_push(&camera->framebuffers, &buffer);

				camera->buffer_mappings[i + 1] = create_buffer_and_map(camera, &camera->buffers[i + 1], &camera->sets[i + 1]);
				memcpy(camera->buffer_mappings[i + 1], camera->buffer_mappings[0], struct_descriptor_sizeof(&camera->struct_definition));
			}
			camera->buffer_count = 6;
			vulkan_camera_set_position_cube(camera, camera->position);
		}
	}
	else
		camera->max_shot_count = 1;


	buf_ucount_t count = buf_get_element_count(&camera->framebuffers);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		BUFFER* framebuffers = CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, i));
		buf_ucount_t framebuffer_count = buf_get_element_count(framebuffers);
		switch(target_type)
		{
			case VULKAN_CAMERA_RENDER_TARGET_TYPE_COLOR:
				if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
					for(buf_ucount_t j = 0; j < framebuffer_count; j++)
						vulkan_framebuffer_restore_supplementary(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(framebuffers, j)));
				else
				{
					vulkan_image_view_t* view;
					if((texture->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
						view = &texture->image_views[i];
					else
						view = &texture->image_view;
					for(buf_ucount_t j = 0; j < framebuffer_count; j++)
						vulkan_framebuffer_set_supplementary(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(framebuffers, j)), view);
				}

			break;
			case VULKAN_CAMERA_RENDER_TARGET_TYPE_DEPTH:
				if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
					for(buf_ucount_t j = 0; j < framebuffer_count; j++)
						vulkan_framebuffer_restore_depth(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(framebuffers, j)));
				else
				{
					vulkan_image_view_t* view;
					if((texture->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
						view = &texture->image_views[i];
					else
						view = &texture->image_view;
					for(buf_ucount_t j = 0; j < framebuffer_count; j++)
						vulkan_framebuffer_set_depth(CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(framebuffers, j)), view);
				}
			break;
			default:
				LOG_FETAL_ERR("Invalid vulkan camera render target type: %u\n", target_type);
		}
	}

	switch(target_type)
	{
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_COLOR:
			if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
				camera->color_render_target = NULL;
			else camera->color_render_target = texture;
		break;
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_DEPTH:
			if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
				camera->depth_render_target = NULL;
			else camera->depth_render_target = texture;
		break;
	}
}

RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene, u64 queue_mask)
{
	// vulkan_render_pass_begin(camera->default_render_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
	// vulkan_render_pass_end(camera->default_render_pass);
	// if(scene != NULL)
	// 	vulkan_render_scene_render(scene, queue_mask);
}

RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* target)
{

}

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_framebuffer_list_handle_t handle)
{
	assert(handle != VULKAN_FRAMEBUFFER_LIST_HANDLE_INVALID);
	return CAST_TO(vulkan_framebuffer_t*, buf_get_ptr_at(CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, camera->current_shot_index)), handle));
}

RENDERER_API vulkan_framebuffer_list_handle_t vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* pass)
{
	if(pass->required_framebuffer_count <= 0)
		return VULKAN_FRAMEBUFFER_LIST_HANDLE_INVALID;
	buf_ucount_t handle = buf_get_element_count(CAST_TO(BUFFER*, buf_peek_ptr(&camera->framebuffers)));
	for(u32 j = 0; j < camera->max_shot_count; j++)
	{
		for(u32 i = 0; i < pass->required_framebuffer_count; i++)
		{
			BUFFER* buffer = CAST_TO(BUFFER*, buf_get_ptr_at(&camera->framebuffers, j));
			buf_push_pseudo(buffer, 1);
			vulkan_framebuffer_create_no_alloc(camera->renderer, pass, i, CAST_TO(vulkan_framebuffer_t*, buf_peek_ptr(buffer)));
		}
	}
	return handle;
}

/* getters */

RENDERER_API mat4_t vulkan_camera_get_view(vulkan_camera_t* camera)
{
	return camera->view;
}

RENDERER_API mat4_t vulkan_camera_get_projection(vulkan_camera_t* camera)
{
	return camera->projection;
}

RENDERER_API mat4_t vulkan_camera_get_transform(vulkan_camera_t* camera)
{
	return camera->transform;
}

RENDERER_API vec3_t vulkan_camera_get_position(vulkan_camera_t* camera)
{
	return camera->position;
}

RENDERER_API vec3_t vulkan_camera_get_rotation(vulkan_camera_t* camera)
{
	return camera->rotation;
}

RENDERER_API vec2_t vulkan_camera_get_clip_planes(vulkan_camera_t* camera)
{
	return vec2(camera->near_clip_plane, camera->far_clip_plane);
}

RENDERER_API float vulkan_camera_get_field_of_view(vulkan_camera_t* camera)
{
	return camera->field_of_view;
}

/* setters */

RENDERER_API void vulkan_camera_set_transform(vulkan_camera_t* camera, mat4_t transform)
{
	camera->transform = transform;
	mat4_t m = mat4_transpose(camera->transform);
	update_mat4(camera, camera->transform_handle, CAST_TO(float*, &m));
	camera->view = mat4_inverse(transform);
	mat4_move(m, mat4_transpose(camera->view));
	update_mat4(camera, camera->view_handle, CAST_TO(float*, &m));
}

RENDERER_API void vulkan_camera_set_position_cube(vulkan_camera_t* camera, vec3_t position)
{
	camera->position = position;
	assert(camera->max_shot_count == 6);
	for(u32 i = 0; i < 6; i++)
	{
		struct_descriptor_map(&camera->struct_definition, camera->buffer_mappings[i]);
		mat4_t transform = mat4_mul(2, mat4_translation(camera->position.x, camera->position.y, camera->position.z),
			mat4_rotation(camera->shot_rotations[i].x, camera->shot_rotations[i].y , camera->shot_rotations[i].z));
		mat4_t view = mat4_inverse(transform);
		mat4_move(transform, mat4_transpose(transform));
		struct_descriptor_set_mat4(&camera->struct_definition, camera->transform_handle, CAST_TO(float*, &transform));
		mat4_move(view, mat4_transpose(view));
		struct_descriptor_set_mat4(&camera->struct_definition, camera->view_handle, CAST_TO(float*, &view));
	}
}

RENDERER_API void vulkan_camera_set_position(vulkan_camera_t* camera, vec3_t position)
{
	camera->position = position;
	vulkan_camera_recalculate_transform(camera);
}

RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t rotation)
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

