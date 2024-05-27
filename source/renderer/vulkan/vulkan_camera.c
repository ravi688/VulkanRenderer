/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_camera.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

#include <hpml/vec4.h>
#include <hpml/affine_transformation.h>

RENDERER_API vulkan_camera_t* vulkan_camera_new(memory_allocator_t* allocator)
{
	vulkan_camera_t* camera = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_CAMERA, vulkan_camera_t);
	memzero(camera, vulkan_camera_t);
	VULKAN_OBJECT_INIT(camera, VULKAN_OBJECT_TYPE_CAMERA, VULKAN_OBJECT_NATIONALITY_INTERNAL);
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
	_debug_assert__(create_info.size == (64 * 4));
	VULKAN_OBJECT_INIT(buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_buffer_create_no_alloc(camera->renderer, &create_info, buffer);

	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = camera->renderer->vo_descriptor_pool,
		.layout = &camera->renderer->camera_set_layout
	};
	vulkan_descriptor_set_create_no_alloc_ext(camera->renderer, &set_create_info, set);
	vulkan_descriptor_set_write_uniform_buffer(set, VULKAN_DESCRIPTOR_BINDING_CAMERA_PROPERTIES, buffer);

	return vulkan_buffer_map(buffer);
}

static void setup_camera_info_uniform_buffer(vulkan_camera_t* camera)
{
	// setup camera struct definiton
	OBJECT_INIT(&camera->struct_definition, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(camera->renderer->allocator, &camera->struct_definition, "cameraInfo", GLSL_TYPE_UNIFORM_BUFFER);
		struct_descriptor_add_field(&camera->struct_definition, "transform", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "projection", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "view", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&camera->struct_definition, "screen", GLSL_TYPE_MAT4);
	struct_descriptor_end(camera->renderer->allocator, &camera->struct_definition);

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

static void set_screen_projection(vulkan_camera_t* camera, mat4_t screen)
{
	camera->screen = screen;
	mat4_move(screen, mat4_transpose(screen));
	update_mat4(camera, camera->screen_handle, CAST_TO(float*, &screen));
}

static void set_persp_projection(vulkan_camera_t* camera, mat4_t projection)
{
	camera->projection = projection;
	mat4_move(projection, mat4_transpose(projection));
	update_mat4(camera, camera->projection_handle, CAST_TO(float*, &projection));
}

static void recalculate_projection_matrices(vulkan_camera_t* camera)
{
	/* recalculate perspective projection matrix */
	mat4_t projection;
	switch(camera->projection_type)
	{
		case VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE:
			mat4_move(projection, mat4_persp_projection(camera->near_clip_plane, 
																	   camera->far_clip_plane, 
																	   camera->field_of_view, 
																	   (float)camera->render_area.extent.width / (float)camera->render_area.extent.height));
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC:
			mat4_move(projection, mat4_ortho_projection(camera->near_clip_plane, 
																	   camera->far_clip_plane, 
																	   camera->height, 
																	   (float)camera->render_area.extent.width / (float)camera->render_area.extent.height));
			break;
		case VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC:
			LOG_FETAL_ERR("VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC isn't supported yet!\n");
			break;
		default:
			LOG_FETAL_ERR("Unsupported camera projection type: %u\n", camera->projection_type);
	}
	set_persp_projection(camera, projection);

	/* recalculate screen projection matrix */
	set_screen_projection(camera, 
		mat4_ortho_projection(-0.04f, 100, camera->render_area.extent.height, (float)camera->render_area.extent.width / (float)camera->render_area.extent.height));
}

static void recalculate_transform(vulkan_camera_t* camera)
{
	vulkan_camera_set_transform(camera, mat4_mul(2,
		mat4_translation(camera->position.x, camera->position.y, camera->position.z),
		mat4_rotation(camera->rotation.x, camera->rotation.y, camera->rotation.z)));
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_camera_is_render_area_absolute(vulkan_camera_t* camera)
{
	return (!camera->is_render_area_relative) && (!camera->is_render_area_full);
}

static void recalculate_render_area_from_relative(vulkan_camera_t* camera);

static void set_render_target_size(vulkan_camera_t* camera, u32 width, u32 height)
{
	camera->prev_render_target_size.width = camera->render_target_size.width;
	camera->prev_render_target_size.height = camera->render_target_size.height;
	camera->render_target_size.width = width;
	camera->render_target_size.height = height;
	
	if(camera->is_render_area_relative)
	{
		_debug_assert__(camera->is_render_area_full == false);
		recalculate_render_area_from_relative(camera);
	}
	else if(camera->is_render_area_full)
	{
		_debug_assert__(!vulkan_camera_is_render_area_absolute(camera));
		/* set the render area to the size of the render window */
		vulkan_camera_set_render_area_default(camera);
	}
}

static void recalculate_render_target_size(void* _window, void* user_data)
{
	AUTO window = CAST_TO(render_window_t*, _window);
	set_render_target_size(VULKAN_CAMERA(user_data), window->width, window->height);
}

CAN_BE_UNUSED_FUNCTION static bool is_default_depth_attachment(vulkan_camera_t* camera)
{
	return (camera->default_depth_attachment != NULL) && (camera->default_depth_attachment == camera->current_depth_attachment);
}

static bool is_depth_render_target(vulkan_camera_t* camera)
{
	return (camera->depth_render_target != NULL) && IS_PTR_EQUAL(VULKAN_TEXTURE(camera->depth_render_target), camera->current_depth_attachment);
}

static bool is_image_size_equal_to(vulkan_image_t* image, u32 width, u32 height)
{
	return (image->width == width) && (image->height == height);
}

static bool is_texture_size_equal_to(vulkan_texture_t* texture, u32 width, u32 height)
{
	return (texture->width == width) && (texture->height == height);
}

static void recreate_allocated_attachments(vulkan_camera_t* camera)
{
	/* attachment recreate information that has to be updated (and that is also a reason to recreate attachments) */
	vulkan_attachment_recreate_info_t info = 
	{
		.width = camera->render_target_size.width,
		.height = camera->render_target_size.height
	};

	/* for each camera render pass */
	u32 pass_count = buf_get_element_count(&camera->render_passes);
	for(u32 i = 0; i < pass_count; i++)
	{
		/* for each allocated attachments for the render pass */
		AUTO render_pass = buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i);
		for(u32 i = 0; i < render_pass->allocated_attachment_count; i++)
		{
			AUTO attachment = render_pass->allocated_attachments[i];
			if(attachment != NULL)
				vulkan_attachment_recreate(render_pass->allocated_attachments[i], &info);
		}
	}

	/* update the allocated attachment size */
	camera->allocated_attachment_size.width = camera->render_target_size.width;
	camera->allocated_attachment_size.height = camera->render_target_size.height;

	debug_log_info("Allocated attachment recreate success");
}

static void resize_default_depth_attachment(vulkan_camera_t* camera, u32 width, u32 height)
{
	if(!is_image_size_equal_to(&camera->default_depth_attachment->image, width, height))
	{
		vulkan_attachment_recreate_info_t recreate_info = 
		{
			.width = width,
			.height = height
		};
		vulkan_attachment_recreate(camera->default_depth_attachment, &recreate_info);
	}
}

static void recreate_attachments(void* window, void* user_data)
{
	AUTO camera = VULKAN_CAMERA(user_data);

	/* recreate the allocated attachments*/
	recreate_allocated_attachments(camera);

	/* if default depth attachment is ever created and it is in use right now then recreate it also */
	_debug_assert__((camera->default_depth_attachment == NULL) || is_default_depth_attachment(camera));
	resize_default_depth_attachment(camera, camera->render_target_size.width, camera->render_target_size.height);
}

static void write_render_pass_descriptors(vulkan_camera_t* camera, vulkan_render_pass_descriptor_sets_t* descriptor_sets, vulkan_camera_render_pass_t* prev_camera_pass, vulkan_camera_render_pass_t* camera_pass)
{
	vulkan_render_pass_pool_t* pass_pool = camera->renderer->render_pass_pool;

	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pass_pool, camera_pass->handle);

	debug_if(slot->input.input_attachment_count != 0)
		_debug_assert__(prev_camera_pass != NULL);

	if(prev_camera_pass != NULL)
	{
		u32 count = slot->create_info->render_set_binding_count;
		vulkan_shader_resource_description_t* render_set_bindings = slot->create_info->render_set_bindings;
		for(u32 i = 0; i < count; i++)
		{
			_debug_assert__(slot->input.input_attachments[i] >= prev_camera_pass->supplementary_attachment_bucket_count);
			vulkan_descriptor_set_write_texture(&descriptor_sets->render_set, render_set_bindings[i].binding_number,
				CAST_TO(vulkan_texture_t*, prev_camera_pass->allocated_attachments[slot->input.input_attachments[i] - prev_camera_pass->supplementary_attachment_bucket_count]));
		}
	}

	// write subpass descriptions
	u32 subpass_count = slot->render_pass->subpass_count;

	debug_if(subpass_count > 0)
		_debug_assert__(slot->input.subpass_inputs != NULL);

	for(u32 i = 0; i < subpass_count; i++)
	{
		u32 binding_count = slot->create_info->subpasses[i].sub_render_set_binding_count;
		vulkan_shader_resource_description_t* sub_render_set_bindings = slot->create_info->subpasses[i].sub_render_set_bindings;
		for(u32 j = 0; j < binding_count; j++)
		{
			_debug_assert__(slot->input.subpass_inputs[i].input_attachments[j] >= camera_pass->supplementary_attachment_bucket_count);
			vulkan_descriptor_set_write_texture(&descriptor_sets->sub_render_sets[i], sub_render_set_bindings[j].binding_number,
				CAST_TO(vulkan_texture_t*, camera_pass->allocated_attachments[slot->input.subpass_inputs[i].input_attachments[j] - camera_pass->supplementary_attachment_bucket_count]));
		}
	}
}

static void rewrite_descriptors(void* window, void* user_data)
{
	vulkan_camera_t* camera = VULKAN_CAMERA(user_data);

	vulkan_render_pass_graph_t* pass_graph = &camera->renderer->render_pass_pool->pass_graph;
	vulkan_render_pass_graph_node_handle_list_t* pass_node_handles = vulkan_render_pass_graph_get_or_build_optimized_path(pass_graph);
	u32 pass_count = buf_get_element_count(pass_node_handles);
	for(u32 i = 0; i < pass_count; i++)
	{
		vulkan_render_pass_graph_node_handle_t node_handle;
		buf_get_at(pass_node_handles, i, &node_handle);
		AUTO node = buf_get_ptr_at_typeof(&pass_graph->nodes, vulkan_render_pass_graph_node_t, node_handle);

		if(node->prev_pass_count == 0)
			write_render_pass_descriptors(camera, 
				vulkan_camera_get_descriptor_sets(camera, VULKAN_RENDER_PASS_HANDLE_INVALID, node->pass_handle), 
				NULL, vulkan_camera_get_camera_render_pass_from_pass_handle(camera, node->pass_handle));

		u32 next_count = buf_get_element_count(&node->next_pass_node_handles);
		for(u32 j = 0; j < next_count; j++)
		{
			vulkan_render_pass_graph_node_handle_t next_node_handle;
			buf_get_at(&node->next_pass_node_handles, j, &next_node_handle);
			AUTO next_node = buf_get_ptr_at_typeof(&pass_graph->nodes, vulkan_render_pass_graph_node_t, next_node_handle);
			write_render_pass_descriptors(camera, 
				vulkan_camera_get_descriptor_sets(camera, node->pass_handle, next_node->pass_handle),
				vulkan_camera_get_camera_render_pass_from_pass_handle(camera, node->pass_handle), 
				vulkan_camera_get_camera_render_pass_from_pass_handle(camera, next_node->pass_handle));
		}
	}

	debug_log_info("Descriptors rewrite sucess");
}

/* holds the information to be copied again into the vulkan_camera_render_pass_t object */
typedef struct vulkan_supplementary_attachments_copy_info_t
{
	VkImageView* vo_supplementary_attachments;
	u32 supplementary_attachment_count;
	u32 supplementary_attachment_bucket_count;
	u32 supplementary_attachment_bucket_depth;
} vulkan_supplementary_attachments_copy_info_t;

RENDERER_API void camera_render_pass_recopy_supplementary_attachments(memory_allocator_t* allocator, vulkan_camera_render_pass_t* render_pass, vulkan_supplementary_attachments_copy_info_t* info)
{
	if((info->supplementary_attachment_bucket_count != render_pass->supplementary_attachment_bucket_count)
		|| (info->supplementary_attachment_bucket_depth != render_pass->supplementary_attachment_bucket_depth))
	{
		if(render_pass->supplementary_attachment_count > 0)
			memory_allocator_dealloc(allocator, render_pass->vo_supplementary_attachments);
		render_pass->supplementary_attachment_bucket_count = info->supplementary_attachment_bucket_count;
		render_pass->supplementary_attachment_bucket_depth = info->supplementary_attachment_bucket_depth;
		render_pass->supplementary_attachment_count = info->supplementary_attachment_count;
		_debug_assert__((render_pass->supplementary_attachment_bucket_count * render_pass->supplementary_attachment_bucket_depth) == render_pass->supplementary_attachment_count);
		render_pass->vo_supplementary_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, render_pass->supplementary_attachment_count);
	}
	_debug_assert__((render_pass->supplementary_attachment_bucket_count * render_pass->supplementary_attachment_bucket_depth) == render_pass->supplementary_attachment_count);
	if(render_pass->supplementary_attachment_count > 0)
		memcopyv(render_pass->vo_supplementary_attachments, info->vo_supplementary_attachments, VkImageView, render_pass->supplementary_attachment_count);
}

static void recopy_supplementary_attachments(void* _window, void* user_data)
{
	AUTO camera = VULKAN_CAMERA(user_data);

	/* copy information */
	vulkan_supplementary_attachments_copy_info_t copy_info = 
	{
		.supplementary_attachment_bucket_count = 1,
		.supplementary_attachment_bucket_depth = camera->renderer->swapchain->image_count,
		.supplementary_attachment_count = camera->renderer->swapchain->image_count /* x 1 */,
		.vo_supplementary_attachments = camera->renderer->swapchain->vo_image_views
	};

	/* for each camera render pass */
	u32 pass_count = buf_get_element_count(&camera->render_passes);
	for(u32 i = 0; i < pass_count; i++)
	{
		AUTO pass = buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i);
		/* only update the swapchain images for render passes which write to them, 
		 * if they don't, meaning no swapchain index, then don't update/add.*/
		if(pass->swapchain_attachment_index != U32_MAX)
			camera_render_pass_recopy_supplementary_attachments(camera->renderer->allocator, pass, &copy_info);
	}

	debug_log_info("Supplementary attachments copy success");
}

static void create_or_recreate_framebuffers_for_camera_pass(vulkan_camera_t* camera, vulkan_camera_render_pass_t* pass);

static void recreate_framebuffers(void* _window, void* user_data)
{
	AUTO camera = VULKAN_CAMERA(user_data);

	/* for each pass */
	u32 pass_count = buf_get_element_count(&camera->render_passes);
	for(u32 i = 0; i < pass_count; i++)
		create_or_recreate_framebuffers_for_camera_pass(camera, buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i));

	debug_log_info("Framebuffer recreate success");
}

RENDERER_API vulkan_camera_t* vulkan_camera_create(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info)
{
	vulkan_camera_t* camera = vulkan_camera_new(renderer->allocator);
	vulkan_camera_create_no_alloc(renderer, create_info, camera);
	return camera;
}

static bool render_pass_handle_pair_comparer(void* value, void* cursor)
{
	AUTO pair1 = CAST_TO(vulkan_render_pass_handle_pair_t*, value);
	AUTO pair2 = CAST_TO(vulkan_render_pass_handle_pair_t*, cursor);
	return (pair1->handle == pair2->handle) && (pair1->prev_handle == pair2->prev_handle);
}

RENDERER_API void vulkan_camera_create_no_alloc(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info, vulkan_camera_t OUT camera)
{
	VULKAN_OBJECT_MEMZERO(camera, vulkan_camera_t);

	camera->renderer = renderer;
	camera->swapchain_depth_clear_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->swapchain_depth_clear_pass);
	camera->depth_clear_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->depth_clear_pass);
	camera->swapchain_clear_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->swapchain_clear_pass);
	camera->projection_type = create_info->projection_type;
	camera->is_active = true;
	camera->render_passes = memory_allocator_buf_new(renderer->allocator, vulkan_camera_render_pass_t);
	camera->render_pass_descriptor_sets = dictionary_create(vulkan_render_pass_handle_pair_t, vulkan_render_pass_descriptor_sets_t, 1, render_pass_handle_pair_comparer);
	camera->shot_taken = 0;
	camera->current_shot_index = 0;
	// set the max shot count equal to 1 by default (it can never be zero)
	camera->max_shot_count = 1;
	/* initially the camera will render to the screen (swapchain image) */
	camera->is_render_area_full = true;
	
	/* sets up the cameraInfo uniform buffer and handles to every field in that such as transform, view, and projection matrices */
	setup_camera_info_uniform_buffer(camera);

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

	render_window_t* window = vulkan_renderer_get_window(renderer);
	set_render_target_size(camera, window->width, window->height);
	camera->allocated_attachment_size.width = camera->render_target_size.width;
	camera->allocated_attachment_size.height = camera->render_target_size.height;
	camera->prev_render_target_size.width = camera->render_target_size.width;
	camera->prev_render_target_size.height = camera->render_target_size.height;

	camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH;
	camera->color_binding_type = VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED;
	camera->depth_binding_type = VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED;

	/* create a copy of clear values so that the clear values can be modified, by calling vulkan_camera_set_clear(),  without affecting the original clear values */
	camera->clear_buffer = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_CLEAR_VALUE_ARRAY, VkClearValue, camera->swapchain_depth_clear_pass->attachment_count);
	memcopyv(camera->clear_buffer, camera->swapchain_depth_clear_pass->vo_clear_values, VkClearValue, camera->swapchain_depth_clear_pass->attachment_count);


	/* calculate transform matrices for the following position and rotation and upload to the unifrom buffer (cameraInfo.transform) in the GPU */
	camera->position = vec3(-1.8f, 0.6f, 0);
	camera->rotation = vec3(0, 0, -22 DEG);
	recalculate_transform(camera);

	// register all the render passes with this camera
	vulkan_render_pass_pool_t* pass_pool = renderer->render_pass_pool;
	vulkan_render_pass_graph_t* pass_graph = &renderer->render_pass_pool->pass_graph;
	vulkan_render_pass_graph_dump(pass_graph);
	vulkan_render_pass_graph_node_handle_list_t* pass_node_handles = vulkan_render_pass_graph_get_or_build_optimized_path(pass_graph);
	u32 pass_count = buf_get_element_count(pass_node_handles);
	for(u32 i = 0; i < pass_count; i++)
	{
		vulkan_render_pass_graph_node_handle_t node_handle;
		buf_get_at(pass_node_handles, i, &node_handle);
		AUTO node = buf_get_ptr_at_typeof(&pass_graph->nodes, vulkan_render_pass_graph_node_t, node_handle);
		if(node->prev_pass_count == 0)
			vulkan_camera_register_render_pass(camera, vulkan_render_pass_pool_getH(pass_pool, node->pass_handle), VULKAN_RENDER_PASS_HANDLE_INVALID,
												&vulkan_render_pass_pool_get_slotH(pass_pool, node->pass_handle)->create_info->framebuffer_layout_description);
		u32 next_count = buf_get_element_count(&node->next_pass_node_handles);
		for(u32 j = 0; j < next_count; j++)
		{
			vulkan_render_pass_graph_node_handle_t next_node_handle;
			buf_get_at(&node->next_pass_node_handles, j, &next_node_handle);
			AUTO next_node = buf_get_ptr_at_typeof(&pass_graph->nodes, vulkan_render_pass_graph_node_t, next_node_handle);
			vulkan_camera_register_render_pass(camera, vulkan_render_pass_pool_get_at(pass_pool, next_node->pass_handle), node->pass_handle,
												&vulkan_render_pass_pool_get_slot_at(pass_pool, next_node->pass_handle)->create_info->framebuffer_layout_description);
		}
	}

	/* recalculate render area and render target size */
	event_subscription_create_info_t subscription = 
	{
		.handler = EVENT_HANDLER(recalculate_render_target_size),
		.handler_data = (void*)camera,
		.wait_for = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH_BIT
	};
	camera->render_area_recalculate_handle = event_subscribe(renderer->window->on_resize_event, &subscription);
	
	/* recreate allocated attachments for each camera render pass */
	subscription.handler = EVENT_HANDLER(recreate_attachments);
	subscription.wait_for = SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT
							| SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	camera->attachments_recreate_handle = event_subscribe(renderer->window->on_resize_event, &subscription);

	subscription.handler = EVENT_HANDLER(rewrite_descriptors);
	subscription.wait_for = SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT | SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	subscription.signal = SIGNAL_NOTHING_BIT;
	camera->descriptors_rewrite_handle = event_subscribe(renderer->window->on_resize_event, &subscription);
	
	/* copy the supplementary attachments (from the swapchain) again to each camera render pass */
	subscription.handler = EVENT_HANDLER(recopy_supplementary_attachments);
	subscription.wait_for = SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT
							| SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT;
	camera->supplementary_attachments_recopy_handle = event_subscribe(renderer->window->on_resize_event, &subscription);
	
	/* recreate framebuffers for each camera render pass */
	subscription.handler = EVENT_HANDLER(recreate_framebuffers);
	subscription.wait_for = SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH_BIT 
							| SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT 
							| SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT;
	camera->framebuffers_recreate_handle = event_subscribe(renderer->window->on_resize_event, &subscription);

	log_msg("Vulkan Camera has been created successfully\n");
}

RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera)
{
	/* unsubscribe window resize event handles */
	event_unsubscribe(camera->renderer->window->on_resize_event, camera->render_area_recalculate_handle);
	event_unsubscribe(camera->renderer->window->on_resize_event, camera->attachments_recreate_handle);
	event_unsubscribe(camera->renderer->window->on_resize_event, camera->descriptors_rewrite_handle);
	event_unsubscribe(camera->renderer->window->on_resize_event, camera->supplementary_attachments_recopy_handle);
	event_unsubscribe(camera->renderer->window->on_resize_event, camera->framebuffers_recreate_handle);

	struct_descriptor_unmap(&camera->struct_definition);
	for(u32 i = 0; i < camera->buffer_count; i++)
	{
		vulkan_descriptor_set_destroy(&camera->sets[i]);
		vulkan_buffer_unmap(&camera->buffers[i]);
		vulkan_buffer_destroy(&camera->buffers[i]);
		vulkan_buffer_release_resources(&camera->buffers[i]);
	}
	buf_ucount_t pass_count = buf_get_element_count(&camera->render_passes);
	for(buf_ucount_t i = 0; i < pass_count; i++)
	{
		AUTO pass = buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i);
		u32 framebuffer_count = buf_get_element_count(&pass->framebuffers);
		for(buf_ucount_t j = 0; j < framebuffer_count; j++)
		{
			AUTO framebuffer = buf_get_ptr_at_typeof(&pass->framebuffers, vulkan_framebuffer_t, j);
			vulkan_framebuffer_destroy(framebuffer);
			vulkan_framebuffer_release_resources(framebuffer);
		}
		buf_clear(&pass->framebuffers, NULL);
		for(buf_ucount_t j = 0; j < pass->allocated_attachment_count; j++)
		{
			vulkan_attachment_t* attachment = pass->allocated_attachments[j];
			if(attachment != NULL)
				vulkan_attachment_destroy(attachment);
		}
	}

	if(camera->default_depth_attachment != NULL)
		vulkan_attachment_destroy(camera->default_depth_attachment);

	/* destroy depth write and clear framebuffers */
	for(u32 i = 0; i < camera->depth_framebuffer_count; i++)
	{
		vulkan_framebuffer_destroy(&camera->depth_write_framebuffers[i]);
		vulkan_framebuffer_release_resources(&camera->depth_write_framebuffers[i]);
		vulkan_framebuffer_destroy(&camera->depth_clear_framebuffers[i]);
		vulkan_framebuffer_release_resources(&camera->depth_clear_framebuffers[i]);
	}
	
	/* destroy the render sets */
	u32 count = dictionary_get_count(&camera->render_pass_descriptor_sets);
	for(u32 i = 0; i < count; i++)
	{
		AUTO sets = CAST_TO(vulkan_render_pass_descriptor_sets_t*, dictionary_get_value_ptr_at(&camera->render_pass_descriptor_sets, i));
		vulkan_descriptor_set_destroy(&sets->render_set);
		vulkan_descriptor_set_release_resources(&sets->render_set);
		for(u32 j = 0; j < sets->sub_render_set_count; j++)
		{
			vulkan_descriptor_set_destroy(&sets->sub_render_sets[j]);
			vulkan_descriptor_set_release_resources(&sets->sub_render_sets[j]);
		}

		memory_allocator_dealloc(camera->renderer->allocator, sets->sub_render_sets);
	}
	dictionary_clear(&camera->render_pass_descriptor_sets);

	log_msg("Vulkan Camera has been destroyed successfully\n");
}

RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera)
{
	if(camera->shot_rotations != NULL)
		memory_allocator_dealloc(camera->renderer->allocator, camera->shot_rotations);
	struct_descriptor_destroy(camera->renderer->allocator, &camera->struct_definition);

	for(u32 i = 0; i < camera->buffer_count; i++)
	{
		vulkan_descriptor_set_release_resources(&camera->sets[i]);
		vulkan_buffer_release_resources(&camera->buffers[i]);
	}
	buf_ucount_t count = buf_get_element_count(&camera->render_passes);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		AUTO pass = buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i);
		buf_free(&pass->framebuffers);
		for(u32 j = 0; j < pass->allocated_attachment_count; j++)
		{
			vulkan_attachment_t* attachment = pass->allocated_attachments[j];
			if(attachment != NULL)
				vulkan_attachment_release_resources(attachment);
		}
		if(pass->allocated_attachment_count > 0)
			memory_allocator_dealloc(camera->renderer->allocator, pass->allocated_attachments);
		if(pass->supplementary_attachment_count > 0)
			memory_allocator_dealloc(camera->renderer->allocator, pass->vo_supplementary_attachments);
		memory_allocator_dealloc(camera->renderer->allocator, pass->vo_image_views);
	}

	if(camera->default_depth_attachment != NULL)
		vulkan_attachment_release_resources(camera->default_depth_attachment);

	buf_free(&camera->render_passes);

	dictionary_free(&camera->render_pass_descriptor_sets);

	_debug_assert__(camera->swapchain_depth_clear_pass != NULL);
	if(camera->swapchain_depth_clear_pass->attachment_count > 0)
		memory_allocator_dealloc(camera->renderer->allocator, camera->clear_buffer);

	if(VULKAN_OBJECT_IS_INTERNAL(camera))
		memory_allocator_dealloc(camera->renderer->allocator, camera);
}

UNUSED_FUNCTION static void transition_target_layout_for_write(VkFormat format, vulkan_image_view_t* view)
{
	VkCommandBuffer cb = view->renderer->vo_command_buffers[view->renderer->current_frame_index];
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* VK_IMAGE_LAYOUT_UNDEFINED: we don't care about the previous results because the results would be overwritten anyway */
				/* oldLayout: */ VK_IMAGE_LAYOUT_UNDEFINED,
				/* newLayout: */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_SHADER_READ_BIT,
				/* dstAccess: */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			break;
		case VK_FORMAT_D32_SFLOAT:
			vulkan_command_image_layout_transition(cb, view->image->vo_handle,
				&view->vo_subresource_range,
				/* VK_IMAGE_LAYOUT_UNDEFINED: we don't care about the previous results because the results would be overwritten anyway */
				/* oldLayout: */ VK_IMAGE_LAYOUT_UNDEFINED,
				/* newLayout: */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				/* srcAccess: */ VK_ACCESS_MEMORY_READ_BIT,
				/* dstAccess: */ VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				/* srcStage: */ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
			break;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any possible render target layout\n", format);
	}
}

static void transition_target_layout_for_sample(VkFormat format, vulkan_image_view_t* view)
{
	VkCommandBuffer cb = view->renderer->vo_command_buffers[view->renderer->current_frame_index];
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
				/* srcStage: */ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				/* dstStage: */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
			break;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any possible render target layout\n", format);
	}
}

/* logic functions */
RENDERER_API void vulkan_camera_begin(vulkan_camera_t* camera)
{
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
		{
			switch(camera->render_target_status)
			{
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH: 
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH: 
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH: 
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH: 
					{
						AUTO clear_buffer = camera->swapchain_depth_clear_pass->vo_clear_values;
						camera->swapchain_depth_clear_pass->vo_clear_values = camera->clear_buffer;
						vulkan_render_pass_begin(camera->swapchain_depth_clear_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
						vulkan_render_pass_end(camera->swapchain_depth_clear_pass);
						camera->swapchain_depth_clear_pass->vo_clear_values = clear_buffer;
					break;
					}
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH: 
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH: 
					{
						AUTO clear_buffer = camera->depth_clear_pass->vo_clear_values;
						camera->depth_clear_pass->vo_clear_values = camera->clear_buffer + 1;
						vulkan_render_pass_begin(camera->depth_clear_pass, 0, camera);
						vulkan_render_pass_end(camera->depth_clear_pass);
						camera->depth_clear_pass->vo_clear_values = clear_buffer;
					break;
					}
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN: 
				case VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR: 
					{
						AUTO clear_buffer = camera->swapchain_clear_pass->vo_clear_values;
						camera->swapchain_clear_pass->vo_clear_values = camera->clear_buffer;
						if(camera->render_target_status == VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN)
							vulkan_render_pass_begin(camera->swapchain_clear_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
						else
							vulkan_render_pass_begin(camera->swapchain_clear_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
						vulkan_render_pass_end(camera->swapchain_clear_pass);
						camera->swapchain_clear_pass->vo_clear_values = clear_buffer;
					break;
					}
				default:
				{
					DEBUG_LOG_FETAL_ERROR("Unhandled vulkan_camera_render_target_status_t: %u", camera->render_target_status);
					break;
				}
			}
		}
		break;
	}

	return true;
}

RENDERER_API void vulkan_camera_end(vulkan_camera_t* camera)
{
	if(camera->color_render_target != NULL)
	{
		if((camera->color_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
		{
			for(u32 i = 0; i < 6; i++)
				transition_target_layout_for_sample(camera->color_render_target->image.vo_format, &camera->color_render_target->image_views[i]);
		}
		else transition_target_layout_for_sample(camera->color_render_target->image.vo_format, &camera->color_render_target->image_view);
	}

	if(camera->depth_render_target != NULL)
	{
		if((camera->depth_render_target->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE)
		{
			for(u32 i = 0; i < 6; i++)
				transition_target_layout_for_sample(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_views[i]);
		}
		else transition_target_layout_for_sample(camera->depth_render_target->image.vo_format, &camera->depth_render_target->image_view);
	}
}

RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth)
{
	vulkan_render_pass_set_clear_indirect(camera->swapchain_depth_clear_pass, color, depth, camera->clear_buffer);
}

typedef enum invalid_operation_type_t
{
	COLOR_AND_DEPTH_RENDER_TARGET_DIMENSION_MISMATCH
} invalid_operation_type_t;

static void invalid_operation(invalid_operation_type_t type)
{
	switch(type)
	{
		case COLOR_AND_DEPTH_RENDER_TARGET_DIMENSION_MISMATCH:
			debug_assert__(false, "Color and Depth render target dimensions matched!\n"
							 	 	"If you want to render to color and depth render texture simultaneously then both the textures must have same dimensions");
		break;
	}
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool is_texture_cube(vulkan_texture_t* texture)
{
	return (texture->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool is_attachment_cube(vulkan_camera_t* camera, vulkan_attachment_t* attachment)
{
	_debug_assert__(attachment != NULL);
	/* TODO: there should be DYNAMIC_CAST(vulkan_texture_t*, attachment) */
	// NOTE: calling is_attachment_cube casts the vulkan_attachment_t object to vulkan_texture_t internally (here), it results in invalid memory access
	// so, before calling is_attachment_cube, make sure the camera->current_depth_attachment is really of type vulkan_texture_t, i.e. set by the external client/user
	// INFO: if camera->default_depth_attachment holds non null value, then it points to a vulkan_attachment_t object
	//		 if camera->depth_render_target holds non null value, then it points to a vulkan_texture_t object
	return (attachment == CAST_TO(vulkan_attachment_t*, camera->depth_render_target)) && is_texture_cube(CAST_TO(vulkan_texture_t*, attachment));
}


typedef enum update_mask_t
{
	UPDATE_COLOR_IMAGE_BIT = BIT32(0),
	UPDATE_DEPTH_IMAGE_BIT = BIT32(1),
	UPDATE_ALLOCATED_ATTACHMENTS_BIT = BIT32(2)
} update_mask_t;

static void update_image_views_for_camera_pass(vulkan_camera_t* camera, vulkan_camera_render_pass_t* pass, update_mask_t mask, u32 image_index, u32 cube_face_index)
{
	u32 counter = 0;

	/* update color image */
	if((mask & UPDATE_COLOR_IMAGE_BIT) && (pass->swapchain_attachment_index != U32_MAX))
	{
		if(camera->color_render_target != NULL)
		{
			if((image_index == U32_MAX) && is_texture_cube(camera->color_render_target))
				pass->vo_image_views[counter] = camera->color_render_target->image_views[cube_face_index].vo_handle;
			else if(image_index != U32_MAX)
				pass->vo_image_views[counter] = camera->color_render_target->image_view.vo_handle;
		}
		else if(image_index != U32_MAX)
		{
			_debug_assert__(pass->swapchain_attachment_index == 0);
			pass->vo_image_views[counter] = camera->renderer->swapchain->vo_image_views[image_index];
		}
		counter++;
	}
	else if(pass->swapchain_attachment_index != U32_MAX)
		counter++;

	/* update images allocated by the render pass */

	/* if there is a depth attachment required for this render pass then it has already been created as a common attachment so no need to count it
	 * as it is placed in the last always */
	u32 allocated_attachment_count = (pass->depth_attachment_index != U32_MAX) ? (pass->allocated_attachment_count - 1) : pass->allocated_attachment_count;

	if(mask & UPDATE_ALLOCATED_ATTACHMENTS_BIT)
	{
		for(u32 i = 0; i < allocated_attachment_count; i++)
		{
			_debug_assert__((i + counter) < pass->image_view_count);
			_debug_assert__(pass->allocated_attachments[i] != NULL);
			pass->vo_image_views[i + counter] = pass->allocated_attachments[i]->image_view.vo_handle;
		}
	}

	counter += allocated_attachment_count;

	/* update depth image */
	if((mask & UPDATE_DEPTH_IMAGE_BIT) && (pass->depth_attachment_index != U32_MAX))
	{
		_debug_assert__(pass->attachment_count != 0);
		_debug_assert__(pass->depth_attachment_index == (pass->attachment_count - 1));
		vulkan_attachment_t* private_depth_atachment = pass->allocated_attachments[pass->depth_attachment_index - ((pass->swapchain_attachment_index != U32_MAX) ? 1 : 0)];
		vulkan_attachment_t* depth_attachment = NULL;

		/* if this pass has it's own depth attachment then use this one */
		if(private_depth_atachment != NULL)
			depth_attachment = private_depth_atachment;
		/* otherwise use provided by the camera (external depth texture or default depth attachment) */
		else
		{
			_debug_assert__(camera->current_depth_attachment != NULL);
			depth_attachment = camera->current_depth_attachment;
		}

		_debug_assert__(depth_attachment != NULL);

		if((image_index == U32_MAX) && is_attachment_cube(camera, depth_attachment))
		{
			_debug_assert__(is_depth_render_target(camera) == true);
			pass->vo_image_views[counter++] = CAST_TO(vulkan_texture_t*, depth_attachment)->image_views[cube_face_index].vo_handle;
		}
		else if(image_index != U32_MAX)
			pass->vo_image_views[counter++] = depth_attachment->image_view.vo_handle;
	}
}

static bool INLINE_IF_RELEASE_MODE is_pass_supports_color_rendering(vulkan_camera_render_pass_t* pass)
{
	return pass->swapchain_attachment_index != U32_MAX;
}

static bool INLINE_IF_RELEASE_MODE is_pass_supports_depth_rendering(vulkan_camera_render_pass_t* pass)
{
	return pass->depth_attachment_index != U32_MAX;
}

static void create_or_recreate_framebuffers_for_camera_pass(vulkan_camera_t* camera, vulkan_camera_render_pass_t* pass)
{
	/* destroy all the valid framebuffers (which were created ealier and can be destroyed) */
	_debug_assert__(pass->valid_framebuffer_count <= buf_get_element_count(&pass->framebuffers));
	for(u32 i = 0; i < pass->valid_framebuffer_count; i++)
	{
		AUTO framebuffer = buf_get_ptr_at_typeof(&pass->framebuffers, vulkan_framebuffer_t, i);
		vulkan_framebuffer_destroy(framebuffer);
		vulkan_framebuffer_release_resources(framebuffer);
	}

	/* recreate the just destroyed framebuffers and create the framebuffers which were invalid (never created earlier) */

	vulkan_framebuffer_create_info_t create_info = 
	{
		.render_pass = vulkan_render_pass_pool_getH(camera->renderer->render_pass_pool, pass->handle),
		.width = camera->render_target_size.width,
		.height = camera->render_target_size.height,
		.attachment_count = pass->image_view_count,
		.attachments = pass->vo_image_views
	};

	update_mask_t cube_update_mask = 0;
	update_mask_t update_mask = 0;

	if(is_pass_supports_color_rendering(pass))
	{
		if((camera->color_render_target != NULL) && is_texture_cube(camera->color_render_target))
			cube_update_mask |= UPDATE_COLOR_IMAGE_BIT;
		else
			update_mask |= UPDATE_COLOR_IMAGE_BIT;
	}

	if(is_pass_supports_depth_rendering(pass))
	{
		if((camera->depth_render_target != NULL) && is_texture_cube(camera->depth_render_target))
			cube_update_mask |= UPDATE_DEPTH_IMAGE_BIT;
		else
			update_mask |= UPDATE_DEPTH_IMAGE_BIT;
	}

	update_image_views_for_camera_pass(camera, pass, UPDATE_ALLOCATED_ATTACHMENTS_BIT, 0 /* ignored */, 0 /* ignored */);

	/* maximum possible valid frambuffers */
	u32 max_framebuffer_count = camera->max_shot_count * pass->framebuffer_count_in_one_face;
	
	/* allocate more space for framebuffer objects if max shot count has increased (hence the max_framebuffer_count) */
	if(max_framebuffer_count > pass->valid_framebuffer_count)
		buf_push_pseudo(&pass->framebuffers, max_framebuffer_count - pass->valid_framebuffer_count);

	for(u32 i = 0, k = 0; i < camera->max_shot_count; i++)
	{
		update_image_views_for_camera_pass(camera, pass, cube_update_mask, U32_MAX, i);

		for(u32 j = 0; j < pass->framebuffer_count_in_one_face; j++, k++)
		{
			update_image_views_for_camera_pass(camera, pass, update_mask, j, U32_MAX);

			create_info.id = j;
			vulkan_framebuffer_create_no_alloc_ext(camera->renderer, &create_info, buf_get_ptr_at_typeof(&pass->framebuffers, vulkan_framebuffer_t, k));
		}
	}

	pass->valid_framebuffer_count = max(pass->valid_framebuffer_count, buf_get_element_count(&pass->framebuffers));
}

static void set_active_all_on_resize_subscriptions(vulkan_camera_t* camera, bool is_active)
{
	AUTO window = vulkan_renderer_get_window(camera->renderer);
	event_set_subscription_active(window->on_resize_event, camera->render_area_recalculate_handle, is_active);
	event_set_subscription_active(window->on_resize_event, camera->attachments_recreate_handle, is_active);
	event_set_subscription_active(window->on_resize_event, camera->descriptors_rewrite_handle, is_active);
	event_set_subscription_active(window->on_resize_event, camera->supplementary_attachments_recopy_handle, is_active);
	event_set_subscription_active(window->on_resize_event, camera->framebuffers_recreate_handle, is_active);	
}

static void update_image_views_and_recreate_framebuffers(vulkan_camera_t* camera)
{
	if((camera->current_depth_attachment != NULL) && (camera->color_render_target != NULL))
	{
		_debug_assert__(is_image_size_equal_to(&camera->current_depth_attachment->image, camera->color_render_target->width, camera->color_render_target->height));
		
		if((camera->current_depth_attachment != NULL) && is_attachment_cube(camera, camera->current_depth_attachment) && (!is_texture_cube(camera->color_render_target)))
			debug_log_warning("Depth attachment is of type CUBE while color is 2D, imagine how the camera could ever render on these types of targets?");
	}

	/* resize the allocated attachments for the render passes if the dimensions are outdated */
	if((camera->render_target_size.width !=  camera->allocated_attachment_size.width)
		|| (camera->render_target_size.height != camera->allocated_attachment_size.height))
	{
		recreate_allocated_attachments(camera);
		/* relink the inputs and outputs across the render passes */
		rewrite_descriptors(NULL, (void*)camera);
	}

	/* recreate framebuffers to redirect the color and depth output to the supplied render targets
	 * i.e camera->current_depth_attachment and camera->color_render_target (if not NULL otherwise swapchain images) */
	u32 pass_count = buf_get_element_count(&camera->render_passes);
	for(u32 i = 0; i < pass_count; i++)
		create_or_recreate_framebuffers_for_camera_pass(camera, buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, i));
}

static void vulkan_camera_set_color_render_target_screen(vulkan_camera_t* camera, vulkan_camera_render_target_binding_type_t binding_type)
{
	camera->color_render_target = NULL;
	AUTO window = vulkan_renderer_get_window(camera->renderer);
	set_render_target_size(camera, window->width, window->height);

	if((camera->current_depth_attachment != NULL) 
		&& is_depth_render_target(camera) 
		&& is_image_size_equal_to(&camera->current_depth_attachment->image,
			camera->render_target_size.width, camera->render_target_size.height))
	{
		debug_log_warning("Depth atachment is external (not resizable) while color attachment is swapchain image, undefined results might occur after resizing the window");
		update_image_views_and_recreate_framebuffers(camera);
		camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH;
	}
	else if(camera->current_depth_attachment != NULL)
	{
		_debug_assert__(camera->current_depth_attachment == camera->default_depth_attachment);

		resize_default_depth_attachment(camera, camera->render_target_size.width, camera->render_target_size.height);
		update_image_views_and_recreate_framebuffers(camera);
		camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH;
	}
	else
	{
		update_image_views_and_recreate_framebuffers(camera);
		camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN;
	}
	set_active_all_on_resize_subscriptions(camera, true);
}

static void vulkan_camera_set_color_render_target_texture(vulkan_camera_t* camera, vulkan_camera_render_target_binding_type_t binding_type, vulkan_texture_t* texture)
{
	_debug_assert__(texture != NULL);

	if(binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE)
	{
		if(camera->is_depth_supported && (camera->depth_binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED))
			debug_log_warning("It is not possible to Exclusively render color values, as the render passes might require depth buffer for correct rendering");
		else if(camera->is_depth_supported && (camera->depth_binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE))
		{
			debug_log_error("Color and Depth render target both have binding type as EXCLUSIVE which is not allowed");
			return;
		}
	}

	_debug_assert__(binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED);

	/* update color render target and render target size */
	camera->color_render_target = texture;
	set_render_target_size(camera, texture->width, texture->height);

	/* if vulkan_camera_set_render_target(RENDER_TARGET_TYPE_DEPTH, <non null value>) has already been called
	 * and now the user is calling vulkan_camera_set_render_target(RENDER_TARGET_TYPE_COLOR, <non null value>)
	 * That means the user wants to draw on external textures for both the values depth and color. 
	 * But if the dimensions of those external textures doesn't match then drawing over the two textures is not possible. */
	if(camera->depth_render_target != NULL)
	{
		_debug_assert__(is_depth_render_target(camera) == true);

		/* If the image dimensions are equal (external depth and color render targets) then draw over both of them */
		if(is_texture_size_equal_to(camera->depth_render_target, texture->width, texture->height))
		{
			/* So, recreate framebuffers with the updated camera->current_depth_attachment and the existing camera->color_render_target */
			/* Also, recreate the allocated attachments if the previous dimension of the allocated attachments doesn't match with the new dimensions */
			update_image_views_and_recreate_framebuffers(camera);
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH;
		}
		/* But if the dimensions of those external textures doesn't match then drawing over the two textures is not possible. */
		else
			invalid_operation(COLOR_AND_DEPTH_RENDER_TARGET_DIMENSION_MISMATCH);
	}
	/* else if the current depth attachment is the default depth attachment then try resizing it then setup duplex rendering */
	else if(camera->default_depth_attachment != NULL)
	{
		_debug_assert__(is_default_depth_attachment(camera) == true);

		/* If the deafult depth attachment's size is different from color render target's size then resize it */
		resize_default_depth_attachment(camera, texture->width, texture->height);

		/* now depth and color rendering both are possible, hence setup it */
		update_image_views_and_recreate_framebuffers(camera);
		camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH;
	}
	else
	{
		update_image_views_and_recreate_framebuffers(camera);
		camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR;
	}

	/* diactivate all the subscriptions to the window resize as the external color render target is not allowed to be resized */
	set_active_all_on_resize_subscriptions(camera, false);
}

static void vulkan_camera_set_depth_render_target_screen(vulkan_camera_t* camera, vulkan_camera_render_target_binding_type_t binding_type)
{
	/* update depth render target, current depth attachment and render target size */
	camera->depth_render_target = NULL;
	camera->current_depth_attachment = camera->default_depth_attachment;
	AUTO window = vulkan_renderer_get_window(camera->renderer);
	set_render_target_size(camera, window->width, window->height);

	if(camera->color_render_target != NULL)
	{
		if(is_image_size_equal_to(&camera->color_render_target->image, window->width, window->height))
		{
			if(camera->current_depth_attachment == NULL)
				camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_COLOR;
			else
			{
				resize_default_depth_attachment(camera, camera->render_target_size.width, camera->render_target_size.height);
				camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_DEPTH;
			}
			update_image_views_and_recreate_framebuffers(camera);
		}
		else
			invalid_operation(COLOR_AND_DEPTH_RENDER_TARGET_DIMENSION_MISMATCH);
	}
	else
	{
		if(camera->current_depth_attachment == NULL)
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN;
		else
		{
			resize_default_depth_attachment(camera, camera->render_target_size.width, camera->render_target_size.height);				
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH;
		}
		update_image_views_and_recreate_framebuffers(camera);

		set_active_all_on_resize_subscriptions(camera, true);
	}
}


static void vulkan_camera_create_or_recreate_depth_framebuffers(vulkan_camera_t* camera, u32 width, u32 height)
{
	_debug_assert__(camera->current_depth_attachment != NULL);
	_debug_assert__(camera->depth_material != NULL);
	_debug_assert__(camera->depth_write_pass != NULL);
	_debug_assert__(camera->depth_clear_pass != NULL);
	_debug_assert__(camera->max_shot_count <= 6);
	_debug_assert__(is_image_size_equal_to(&camera->current_depth_attachment->image, width, height));
	_debug_assert__((camera->render_target_size.width == width) && (camera->render_target_size.height == height));

	UNUSED_VARIABLE AUTO pass_handle = camera->depth_material->shader->render_passes[0].handle;

	bool is_cube = is_attachment_cube(camera, camera->current_depth_attachment);

	/* create framebuffer to hold the depth attachment */
	vulkan_framebuffer_create_info_t create_info = 
	{
		.width = width,
		.height = height,
		.id = 0,
		.attachment_count = 1,
		.attachments = is_cube ? NULL : &camera->current_depth_attachment->image_view.vo_handle
	};

	/* create framebuffer objects for the remaining shots if not created ealier */
	for(u32 i = camera->depth_framebuffer_count; i < camera->max_shot_count; i++)
	{
		if(is_cube)
			/* we can use VULKAN_TEXTURE() cast here, as if the attachment is cube, then it is guaranteed to be a vulkan_texture_t object */
			create_info.attachments = &VULKAN_TEXTURE(camera->current_depth_attachment)->image_views[i].vo_handle;
		create_info.render_pass = camera->depth_write_pass;
		vulkan_framebuffer_create_no_alloc_ext(camera->renderer, &create_info, &camera->depth_write_framebuffers[i]);
		create_info.render_pass = camera->depth_clear_pass;
		vulkan_framebuffer_create_no_alloc_ext(camera->renderer, &create_info, &camera->depth_clear_framebuffers[i]);
	}

	/* recreate framebuffers objects which were created ealier to match the width and height with newly created framebuffer objects */
	for(u32 i = 0; i < camera->depth_framebuffer_count; i++)
	{
		vulkan_framebuffer_destroy(&camera->depth_write_framebuffers[i]);
		vulkan_framebuffer_release_resources(&camera->depth_write_framebuffers[i]);
		/* WARNING: the following 2 lines have been added without understanding the full context, you may need to delete them if things do not work as intended */
		vulkan_framebuffer_destroy(&camera->depth_clear_framebuffers[i]);
		vulkan_framebuffer_release_resources(&camera->depth_clear_framebuffers[i]);

		if(is_cube)
			/* we can use VULKAN_TEXTURE() cast here, as if the attachment is cube, then it is guaranteed to be a vulkan_texture_t object */
			create_info.attachments = &VULKAN_TEXTURE(camera->current_depth_attachment)->image_views[i].vo_handle;
		create_info.render_pass = camera->depth_write_pass;
		vulkan_framebuffer_create_no_alloc_ext(camera->renderer, &create_info, &camera->depth_write_framebuffers[i]);
		create_info.render_pass = camera->depth_clear_pass;
		vulkan_framebuffer_create_no_alloc_ext(camera->renderer, &create_info, &camera->depth_clear_framebuffers[i]);
	}

	/* update the depth framebuffer count (increase it if max shot count has increased but not decrease it in anyway) */
	camera->depth_framebuffer_count = max(camera->depth_framebuffer_count, camera->max_shot_count);
}

static void vulkan_camera_set_depth_render_target_texture(vulkan_camera_t* camera, vulkan_camera_render_target_binding_type_t binding_type, vulkan_texture_t* texture)
{
	_debug_assert__(texture != NULL);

	if(binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE)
	{
		if(camera->color_binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE)
		{
			debug_log_error("Color and depth render target both have binding type as EXCLUSIVE, which is not possible");
			return;
		}
		else if(camera->color_render_target != NULL)
			debug_log_warning("Camera is no longer rendering to the supplied external color render target as depth binding type is EXCLUSIVE");
	}
	
	/* update depth render target, current depth attachment and render target size */
	camera->depth_render_target = texture;
	/* use REINTERPRET_CAST here as we are sure that vulkan_attachment_t is a subset of vulkan_texture_t 
	 * TODO: merge vulkan_attachment_t and vulkan_texture_t into one or some organized types */
	camera->current_depth_attachment = REINTERPRET_CAST(vulkan_attachment_t*, camera->depth_render_target);
	set_render_target_size(camera, texture->width, texture->height);

	if(binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED)
	{
		if(camera->color_binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE)
		{
			debug_log_error("Color render target has binding type as EXCLUSIVE (i.e. it can't be shared)");
			return;
		}

		camera->depth_binding_type = VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED;

		/* if vulkan_camera_set_render_target(RENDER_TARGET_TYPE_COLOR, <non null value>) has already been called
		 * and now the user is calling vulkan_camera_set_render_target(RENDER_TARGET_TYPE_DEPTH, <non null value>)
		 * That means the user wants to draw on external textures for both the values color and depth. */
		if(camera->color_render_target != NULL)
		{
			/* If the image dimensions are equal (external depth and color render targets) then draw over both of them */
			if(is_texture_size_equal_to(camera->color_render_target, texture->width, texture->height))
			{
				/* So, recreate framebuffers with the updated camera->current_depth_attachment and the existing camera->color_render_target */
				update_image_views_and_recreate_framebuffers(camera);
				camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_EXTERNAL_COLOR_EXTERNAL_DEPTH;
				return;
			}
			/* But if the dimensions of those external textures doesn't match then drawing over the two textures is not possible. */
			else
				invalid_operation(COLOR_AND_DEPTH_RENDER_TARGET_DIMENSION_MISMATCH);
		}
		else if((camera->allocated_attachment_size.width == texture->width) && (camera->allocated_attachment_size.height == texture->height))
		{
			debug_log_warning("Depth atachment is external (not resizable) while color attachment is swapchain image, undefined results might occur after resizing the window");
			/* So, recreate framebuffers with the updated camera->current_depth_attachment and the swapchain image */
			update_image_views_and_recreate_framebuffers(camera);
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_EXTERNAL_DEPTH;
			return;
		}
	}

	/* Otherwise only render depth values to the texture (no need to render color values)
	 * so we can use the builtins/depth_shader.v3dshader to render all the objects efficiently with just one depth render pass */

	/* setup for depth only rendering (simplex) */

	_debug_assert__(binding_type == VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE);

	/* if depth material is not loaded then load it (but just once) 
	 * this depth material will be used to render all objects using this camera 
	 * we are using a separate depth shader because there is no need to render colors 
	 * as we are requested only to render depth values */
	if(camera->depth_material == NULL)
	{
		debug_log_info("Loading depth_shader.sb ...");
		vulkan_shader_load_info_t load_info = 
		{
			.path = "shaders/builtins/depth_shader.sb",
			.is_vertex_attrib_from_file = true
		};
		camera->depth_material = vulkan_material_library_getH(camera->renderer->material_library, 
			vulkan_material_library_create_materialH(camera->renderer->material_library, 
			vulkan_shader_library_load_shader(camera->renderer->shader_library, 
				&load_info, "BuiltIn:DepthShader"), "BuiltIn:DepthMaterial"));

		/* for now there must be just one render pass in the depth_shader.sb */
		_debug_assert__(camera->depth_material->shader->render_pass_count == 1);

		camera->depth_write_pass = vulkan_render_pass_pool_getH(camera->renderer->render_pass_pool, camera->depth_material->shader->render_passes[0].handle);
	}
	
	vulkan_camera_create_or_recreate_depth_framebuffers(camera, texture->width, texture->height);
	camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH;
	camera->depth_binding_type = VULKAN_CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE;

	/* since we are only writing to a depth attachment (which is external and non-resizable), we have to disable window resize callbacks */
	set_active_all_on_resize_subscriptions(camera, false);
}

RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_camera_render_target_binding_type_t binding_type, vulkan_texture_t* texture)
{	
	if((texture != VULKAN_CAMERA_RENDER_TARGET_SCREEN) && ((texture->type & VULKAN_TEXTURE_TYPE_CUBE) == VULKAN_TEXTURE_TYPE_CUBE))
	{
		/* rendering to cube requires the images (faces of the cube) to be square (sides must be same) */
		debug_assert__(texture->width == texture->height, "%lu != %lu, Cube textures must have their width and height equal (square)",
			texture->width, texture->height);

		camera->max_shot_count = 6;
		if(camera->shot_rotations == NULL)
		{
			camera->shot_rotations = memory_allocator_alloc_obj_array(camera->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VEC3_ARRAY, vec3_t, camera->max_shot_count);
			camera->shot_rotations[0] = vec3(0, -90 DEG, 0); // right
			camera->shot_rotations[1] = vec3(0, 90 DEG, 0); 	// left
			camera->shot_rotations[2] = vec3(0, 0, 90 DEG); // bottom
			camera->shot_rotations[3] = vec3(0, 0, -90 DEG); 	// top
			camera->shot_rotations[4] = vec3(0, 0, 0); 		// front
			camera->shot_rotations[5] = vec3(0, 180 DEG, 0); // back

			/* we also need to duplicate the uniform buffers (that contains shot transform information) 5 times more */
			for(u32 i = 0; i < 5; i++)
			{
				camera->buffer_mappings[i + 1] = create_buffer_and_map(camera, &camera->buffers[i + 1], &camera->sets[i + 1]);
				memcpy(camera->buffer_mappings[i + 1], camera->buffer_mappings[0], struct_descriptor_sizeof(&camera->struct_definition));
			}
			/* update the number of buffer count (it is now duplicated to 6) */
			camera->buffer_count = 6;
			/* copy the values from the very first buffer to the newly created (duplicated) buffers */
			vulkan_camera_set_position_cube(camera, camera->position);
		}
	}
	else
		camera->max_shot_count = 1;

	/* setting to a different render targets might also destroy existing framebuffer objects 
	 * , as per vulkan spec, VkFramebuffer object can only be destroyed if nothing is use it, 
	 * therefore, make sure all of the command buffers have finished their execution and no one is referencing the VkFramebuffer objects */
	vulkan_renderer_wait_idle(camera->renderer);

	switch(target_type)
	{
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_COLOR:
		{
			if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
				vulkan_camera_set_color_render_target_screen(camera, binding_type);
			else
				vulkan_camera_set_color_render_target_texture(camera, binding_type, texture);

			break;
		}
		case VULKAN_CAMERA_RENDER_TARGET_TYPE_DEPTH:
		{
			if(!camera->is_depth_supported)
			{
				debug_log_warning("Camera doesn't support rendering to depth attachment/texture");
				break;
			}
			if(texture == VULKAN_CAMERA_RENDER_TARGET_SCREEN)
				vulkan_camera_set_depth_render_target_screen(camera, binding_type);
			else
				vulkan_camera_set_depth_render_target_texture(camera, binding_type, texture);

			break;
		}
		default:
		{
			DEBUG_LOG_FETAL_ERROR("Unexpected vulkan_camera_render_target_type_t: %u", target_type);
			break;
		}
	}

	debug_log_info("Render Target Status: %s", vulkan_camera_render_target_status_str(camera->render_target_status));
}

RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene, u64 queue_mask)
{
	// vulkan_render_pass_begin(camera->swapchain_depth_clear_pass, VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN, camera);
	// vulkan_render_pass_end(camera->swapchain_depth_clear_pass);
	// if(scene != NULL)
	// 	vulkan_render_scene_render(scene, queue_mask);
}

RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* target)
{

}

static bool render_pass_handle_comparer(void* handle, void* render_pass)
{
	return DREF_TO(vulkan_render_pass_handle_t, handle) == CAST_TO(vulkan_camera_render_pass_t*, render_pass)->handle;
}

#ifdef GLOBAL_DEBUG
static void dump_registered_render_pass_handles(vulkan_camera_t* camera)
{
	printf("[Log] Camera: Registered render pass handle list: ");
	if(buf_get_element_count(&camera->render_passes) > 0)
		buf_traverse_elements(&camera->render_passes, 0, buf_get_element_count(&camera->render_passes) - 1, buf_u64_print, NULL);
	puts("");
}
#else
#	define dump_registered_render_pass_handles(...)
#endif /* GLOBAL_DEBUG */

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool is_depth_render_only(vulkan_camera_t* camera)
{
	return (camera->render_target_status == VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH)
		|| (camera->render_target_status == VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_EXTERNAL_DEPTH);
}

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_render_pass_handle_t handle)
{
	if(is_depth_render_only(camera))
	{
		if(handle == camera->depth_clear_pass->handle)
			return &camera->depth_clear_framebuffers[camera->current_shot_index];
		else
		{
			_debug_assert__(handle == camera->depth_write_pass->handle);
			return &camera->depth_write_framebuffers[camera->current_shot_index];
		}
	}
	
	AUTO index = buf_find_index_of(&camera->render_passes, &handle, render_pass_handle_comparer);
	debug_if(index == BUF_INVALID_INDEX)
	{
		dump_registered_render_pass_handles(camera);
		_debug_assert__(index != BUF_INVALID_INDEX);
	}
	AUTO pass = buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, index);
	u32 framebuffer_count = buf_get_element_count(&pass->framebuffers);
	return buf_get_ptr_at_typeof(&pass->framebuffers, vulkan_framebuffer_t, camera->current_shot_index * (framebuffer_count / camera->max_shot_count));
}

RENDERER_API vulkan_render_pass_descriptor_sets_t* vulkan_camera_get_descriptor_sets(vulkan_camera_t* camera, vulkan_render_pass_handle_t prev_handle, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_handle_pair_t pair = { prev_handle, handle };
	return CAST_TO(vulkan_render_pass_descriptor_sets_t*, dictionary_get_value_ptr(&camera->render_pass_descriptor_sets, &pair));
}

RENDERER_API vulkan_camera_render_pass_t* vulkan_camera_get_camera_render_pass_from_pass_handle(vulkan_camera_t* camera, vulkan_render_pass_handle_t handle)
{
	buf_ucount_t index = buf_find_index_of(&camera->render_passes, &handle, render_pass_handle_comparer);
	return (index == BUF_INVALID_INDEX) ? NULL : buf_get_ptr_at_typeof(&camera->render_passes, vulkan_camera_render_pass_t, index);
}

RENDERER_API void vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* render_pass, vulkan_render_pass_handle_t prev_pass_handle, vulkan_framebuffer_attachments_layout_description_t* framebuffer_layout_description)
{
	_debug_assert__(render_pass->handle != VULKAN_RENDER_PASS_HANDLE_INVALID);

	/* if descriptor sets for this pair of render pass handles have already been created then return */
	vulkan_render_pass_handle_pair_t handle_pair = { prev_pass_handle, render_pass->handle };
	if(dictionary_contains(&camera->render_pass_descriptor_sets, &handle_pair))
		return;
	
	AUTO renderer = camera->renderer;

	vulkan_render_pass_descriptor_sets_t descriptor_sets = { };

	/* create render set */
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &render_pass->render_set_layout
	};
	vulkan_descriptor_set_create_no_alloc_ext(camera->renderer, &set_create_info, &descriptor_sets.render_set);

	/* create sub render sets for each subpass */
	descriptor_sets.sub_render_set_count = render_pass->subpass_count;
	descriptor_sets.sub_render_sets = memory_allocator_alloc_obj_array(camera->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_ARRAY, vulkan_descriptor_set_t, render_pass->subpass_count);
	for(u32 i = 0; i < render_pass->subpass_count; i++)
	{
		vulkan_descriptor_set_create_info_t set_create_info = 
		{
			.vo_pool = renderer->vo_descriptor_pool,
			.layout = &render_pass->sub_render_set_layouts[i]
		};
		vulkan_descriptor_set_create_no_alloc_ext(renderer, &set_create_info, &descriptor_sets.sub_render_sets[i]);
	}

	dictionary_add(&camera->render_pass_descriptor_sets, &handle_pair, &descriptor_sets);

	dump_registered_render_pass_handles(camera);

	/* make sure none of the render pass get registered more than once */
	vulkan_camera_render_pass_t* pass;
	if((pass = vulkan_camera_get_camera_render_pass_from_pass_handle(camera, render_pass->handle)) != NULL) 
	{
		/* write the descriptors */
		write_render_pass_descriptors(camera, &descriptor_sets, vulkan_camera_get_camera_render_pass_from_pass_handle(camera, prev_pass_handle), pass);
		return;
	}

	buf_push_pseudo(&camera->render_passes, 1);

	pass = CAST_TO(vulkan_camera_render_pass_t*, buf_peek_ptr(&camera->render_passes));
	pass->handle = render_pass->handle;

	/* setup attachments for this render pass */
	pass->supplementary_attachment_bucket_count = framebuffer_layout_description->supplementary_attachment_bucket_count;
	pass->supplementary_attachment_bucket_depth = framebuffer_layout_description->supplementary_attachment_bucket_depth;
	pass->supplementary_attachment_count = framebuffer_layout_description->supplementary_attachment_count;
	_debug_assert__((pass->supplementary_attachment_bucket_count * pass->supplementary_attachment_bucket_depth) == pass->supplementary_attachment_count);

	pass->vo_supplementary_attachments = (pass->supplementary_attachment_count > 0) ? memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, pass->supplementary_attachment_count) : NULL;
	if(pass->vo_supplementary_attachments != NULL)
		memcopyv(pass->vo_supplementary_attachments, framebuffer_layout_description->vo_supplementary_attachments, VkImageView, pass->supplementary_attachment_count);
	
	pass->depth_attachment_index = U32_MAX;
	pass->swapchain_attachment_index = (pass->supplementary_attachment_count > 0) ? 0 : U32_MAX;

	/* allocate attachment objects needed by the render passes */
	pass->allocated_attachment_count = CAST_TO(s32, framebuffer_layout_description->attachment_description_count) - CAST_TO(s32, pass->supplementary_attachment_bucket_count);
	pass->allocated_attachments = (pass->allocated_attachment_count == 0) ? NULL :  memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_ARRAY, vulkan_attachment_t*, pass->allocated_attachment_count);
	
	if(pass->allocated_attachments != NULL)
		memzerov(pass->allocated_attachments, vulkan_attachment_t*, pass->allocated_attachment_count);

	for(u32 i = 0; i < pass->allocated_attachment_count; i++)
	{
		vulkan_attachment_create_info_t attachment_create_info = 
		{
			.format = framebuffer_layout_description->attachment_descriptions[i + pass->supplementary_attachment_bucket_count].format,
			.width = camera->render_target_size.width,
			.height = camera->render_target_size.height,
			.next_pass_usage = framebuffer_layout_description->attachment_usages[i + pass->supplementary_attachment_bucket_count]
		};

		switch(attachment_create_info.format)
		{
			/* if this attachment is a depth attachment (format = VK_FORMAT_D32_SFLOAT) */
			case VK_FORMAT_D32_SFLOAT:
			{
				attachment_create_info.type = VULKAN_ATTACHMENT_TYPE_DEPTH;
				pass->depth_attachment_index = i + ((pass->swapchain_attachment_index != U32_MAX) ? 1 : 0);
				if(attachment_create_info.next_pass_usage != VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED)
				{
					bool is_current_depth_attachment_null_or_default = !is_depth_render_target(camera);

					/* if depth attachment isn't created then create it (but just once) */
					if(camera->default_depth_attachment == NULL)
					{
						camera->default_depth_attachment = vulkan_attachment_create(renderer, &attachment_create_info);

						/* it might be possible that there is no registered pass with this camera but still the user calls set_render_target
						 * function and after that he registers the render passes.
						 * so, we need to check if a depth render target has already been set
						 * if not, then set the current depth attachment to the default depth attachment we just created */
						if(is_current_depth_attachment_null_or_default)
							camera->current_depth_attachment = camera->default_depth_attachment;
					}
					/* no allocation has been made */
					pass->allocated_attachments[i] = NULL;
					break;
				}
				/* otherwise create separate depth attachment */
				pass->allocated_attachments[i] = vulkan_attachment_create(renderer, &attachment_create_info);
				break;
			}

			/* otherwise proceed as usual */
			default: 
			{
				attachment_create_info.type = VULKAN_ATTACHMENT_TYPE_COLOR;
				pass->allocated_attachments[i] = vulkan_attachment_create(renderer, &attachment_create_info);
				break;
			}
		}
	}

	if((!camera->is_color_supported) && (pass->swapchain_attachment_index != U32_MAX))
		camera->is_color_supported = true;
	if((!camera->is_depth_supported) && (pass->depth_attachment_index != U32_MAX))
		camera->is_depth_supported = true;

	if((camera->color_render_target == NULL) && (camera->depth_render_target == NULL))
	{
		if(camera->is_color_supported && camera->is_depth_supported)
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_DUPLEX_SWAPCHAIN_DEPTH;
		else if(camera->is_color_supported && !camera->is_depth_supported)
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_SWAPCHAIN;
		else if(camera->is_depth_supported && !camera->is_color_supported)
			camera->render_target_status = VULKAN_CAMERA_RENDER_TARGET_STATUS_SIMPLEX_DEPTH;
	}

	/* allocate vulkan image views to be attached with framebuffer objects for this render pass */
	pass->image_view_count = framebuffer_layout_description->attachment_description_count;
	_debug_assert__(pass->image_view_count > 0);
	pass->vo_image_views = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, framebuffer_layout_description->attachment_description_count);
	memzerov(pass->vo_image_views, VkImageView, pass->image_view_count);

	/* setup framebuffers for this render pass */
	pass->framebuffer_count_in_one_face = render_pass->required_framebuffer_count;
	pass->framebuffers = memory_allocator_buf_new(camera->renderer->allocator, vulkan_framebuffer_t);
	pass->valid_framebuffer_count = 0;

	/* pass->framebuffers would automatically resize inside this function based on the value of max_shot_count */
	create_or_recreate_framebuffers_for_camera_pass(camera, pass);

	write_render_pass_descriptors(camera, &descriptor_sets, vulkan_camera_get_camera_render_pass_from_pass_handle(camera, prev_pass_handle), pass);

	debug_log_info("Camera: Render pass (handle: %llu) registered success, details { prev(handle: %llu) }", render_pass->handle, prev_pass_handle);
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
	_debug_assert__(camera->max_shot_count == 6);
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
	recalculate_transform(camera);
}

RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t rotation)
{
	camera->rotation = rotation;
	recalculate_transform(camera);
}

RENDERER_API void vulkan_camera_set_clip_planes(vulkan_camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	camera->near_clip_plane = near_clip_plane;
	camera->far_clip_plane = far_clip_plane;
	recalculate_projection_matrices(camera);
}

RENDERER_API void vulkan_camera_set_field_of_view(vulkan_camera_t* camera, float fov)
{
	camera->field_of_view = fov;
	recalculate_projection_matrices(camera);
}

RENDERER_API void vulkan_camera_set_render_area(vulkan_camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height)
{
	/* render area is not absolute (size defined by the user) */
	camera->is_render_area_relative = false;
	camera->is_render_area_full = false;
	camera->render_area.extent.width = width;
	camera->render_area.extent.height = height;
	camera->render_area.offset.x = offset_x;
	camera->render_area.offset.y = offset_y;
	_debug_assert__((camera->render_area.extent.width + camera->render_area.offset.x) <= camera->render_target_size.width);
	_debug_assert__((camera->render_area.extent.height + camera->render_area.offset.y) <= camera->render_target_size.height);
	recalculate_projection_matrices(camera);
}

/* calculates the absolute render area from the relative render area */
static void recalculate_render_area_from_relative(vulkan_camera_t* camera)
{
	_debug_assert__(camera->is_render_area_relative == true);
	float inverse_100 = 1 / (float)100;
	camera->render_area.extent.width = CAST_TO(u32, camera->render_target_size.width * camera->render_area_relative.extent.width * inverse_100);
	camera->render_area.extent.height = CAST_TO(u32, camera->render_target_size.height * camera->render_area_relative.extent.height * inverse_100);
	camera->render_area.offset.x = CAST_TO(u32, camera->render_target_size.width * camera->render_area_relative.offset.x * inverse_100);
	camera->render_area.offset.y = CAST_TO(u32, camera->render_target_size.height * camera->render_area_relative.offset.y * inverse_100);
	_debug_assert__((camera->render_area.extent.width + camera->render_area.offset.x) <= camera->render_target_size.width);
	_debug_assert__((camera->render_area.extent.height + camera->render_area.offset.y) <= camera->render_target_size.height);
	recalculate_projection_matrices(camera);
}

RENDERER_API void vulkan_camera_set_render_area_relative(vulkan_camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height)
{
	/* render is now relative (in general, not the size of the render window) */
	camera->is_render_area_relative = true;
	camera->is_render_area_full = false;
	camera->render_area_relative.extent.width = width;
	camera->render_area_relative.extent.height = height;
	camera->render_area_relative.offset.x = offset_x;
	camera->render_area_relative.offset.y = offset_y;
	recalculate_render_area_from_relative(camera);
}

RENDERER_API void vulkan_camera_set_render_area_default(vulkan_camera_t* camera)
{
	camera->is_render_area_relative = false;
	camera->is_render_area_full = true;
	camera->render_area.extent.width = camera->render_target_size.width;
	camera->render_area.extent.height = camera->render_target_size.height;
	camera->render_area.offset.x = 0;
	camera->render_area.offset.y = 0;
	/* calculate the projection and screen projection matrices and upload that to the uniform buffer (cameraInfo.projection and cameraInfo.screen) in the GPU */
	recalculate_projection_matrices(camera);
}
