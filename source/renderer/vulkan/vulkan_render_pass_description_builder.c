/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_description_builder.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_render_pass_description_builder.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create(memory_allocator_t* allocator)
{
	vulkan_render_pass_description_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_DESCRIPTION_BUILDER, vulkan_render_pass_description_builder_t);
	memzero(builder, vulkan_render_pass_description_builder_t);
	builder->allocator = allocator;
	builder->description_array = memory_allocator_buf_new(allocator, vulkan_render_pass_description_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_render_pass_description_t* descriptions, u32 description_count)
{
	NOT_IMPLEMENTED_FUNCTION();
	vulkan_render_pass_description_builder_t* builder = vulkan_render_pass_description_builder_create(allocator);
	// vulkan_render_pass_description_builder_add(builder, description_count);
	// for(u32 i = 0; i < description_count; i++)
	// {
		// vulkan_render_pass_description_builder_bind(builder, i);
	// }
	return builder;
}

RENDERER_API void vulkan_render_pass_description_builder_destroy(vulkan_render_pass_description_builder_t* builder)
{
	u32 count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO description = buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, i);
		vulkan_render_pass_description_destroy_allocations(builder->allocator, description);
	}
	buf_free(&builder->description_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_render_pass_description_builder_add(vulkan_render_pass_description_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->description_array, count);
}

RENDERER_API void vulkan_render_pass_description_builder_bind(vulkan_render_pass_description_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	builder->bind_index = index;
}


RENDERER_API vulkan_render_pass_description_t* vulkan_render_pass_description_builder_get(vulkan_render_pass_description_builder_t* builder)
{
	if(buf_get_element_count(&builder->description_array) == 0)
		return NULL;
	vulkan_render_pass_description_builder_resolve_dependencies(builder);
	return CAST_TO(vulkan_render_pass_description_t*, buf_get_ptr(&builder->description_array));
}

RENDERER_API u32 vulkan_render_pass_description_builder_get_count(vulkan_render_pass_description_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->description_array));
}


static bool u32_list_contains(const u32* const list, const u32 count, const u32 value)
{
	for(u32 i = 0; i < count; i++)
		if(list[i] == value)
			return true;
	return false;
}

#define is_next_subpass_reads_any_color(pass, prev_pass) is_next_subpass_reads_any((pass), (prev_pass)->color_attachments, (prev_pass)->color_attachment_count)
#define is_next_subpass_reads_depth(pass, prev_pass) is_next_subpass_reads_any((pass), &(prev_pass)->depth_stencil_attachment, ((prev_pass)->depth_stencil_attachment == U32_MAX) ? 0 : 1)
#define is_next_subpass_writes_any_color(pass, prev_pass) is_next_subpass_writes_any((pass), (prev_pass)->color_attachments, (prev_pass)->color_attachment_count)
#define is_next_subpass_writes_depth(pass, prev_pass) is_next_subpass_writes_any((pass), &(prev_pass)->depth_stencil_attachment, ((prev_pass)->depth_stencil_attachment == U32_MAX) ? 0 : 1)

static bool is_next_subpass_reads_any(const vulkan_subpass_description_t* const pass, const u32* const attachments, u32 count)
{
	if(attachments == NULL) return false;
	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->input_attachments[i]))
			return true;
	return false;
}

static bool is_next_subpass_writes_any(const vulkan_subpass_description_t* const pass, const u32* const attachments, u32 count)
{
	for(u32 i = 0; i < pass->color_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->color_attachments[i]))
			return true;
	if(pass->depth_stencil_attachment != U32_MAX)
		if(u32_list_contains(attachments, count, pass->depth_stencil_attachment))
			return true;
	return false;
}

UNUSED_FUNCTION static bool is_renderpass_reads_any(const vulkan_render_pass_description_t* const pass, const u32* const attachments, u32 count)
{
	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->input_attachments[i]))
			return true;
	return false;
}

UNUSED_FUNCTION static bool is_renderpass_writes_any(const vulkan_render_pass_description_t* const pass, const u32* const attachments, u32 count)
{
	/* currently not supported */
	return false;
}

static bool is_renderpass_reads_depth(const vulkan_render_pass_description_t* const pass, const vulkan_render_pass_description_t* const prev_pass)
{
	u32 depth_index;
	if((prev_pass->attachment_count > 0) && (prev_pass->attachments[prev_pass->attachment_count - 1] == VULKAN_ATTACHMENT_TYPE_DEPTH))
		depth_index = prev_pass->attachment_count - 1;
	else
	{
		log_wrn("No depth attachment found in the previou pass\n");
		return false;
	}

	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(pass->input_attachments[i] == depth_index)
				return true;

	return false;
}

static void try_add_dependency(BUFFER* dependencies, const VkSubpassDependency* const in_dependency)
{
	u32 count = buf_get_element_count(dependencies);
	VkSubpassDependency* best_match = NULL;
	for(u32 i = 0; i < count; i++)
	{
		VkSubpassDependency* dependency = buf_get_ptr_at_typeof(dependencies, VkSubpassDependency, i);
		if((dependency->srcSubpass == in_dependency->srcSubpass) && (dependency->dstSubpass == in_dependency->dstSubpass))
		{
			best_match = dependency;
			if(((dependency->srcStageMask & in_dependency->srcStageMask) == in_dependency->srcStageMask)
				&& ((dependency->dstStageMask & in_dependency->dstStageMask) == in_dependency->dstStageMask)
				&& ((dependency->srcAccessMask & in_dependency->srcAccessMask) == in_dependency->srcAccessMask)
				&& ((dependency->dstAccessMask & in_dependency->dstAccessMask) == in_dependency->dstAccessMask))
				return;
		}
	}

	if(best_match == NULL)
		buf_push(dependencies, (VkSubpassDependency*)in_dependency);
	else
	{
		best_match->srcStageMask |= in_dependency->srcStageMask;
		best_match->dstStageMask |= in_dependency->dstStageMask;
		best_match->srcAccessMask |= in_dependency->srcAccessMask;
		best_match->dstAccessMask |= in_dependency->dstAccessMask;
	}
}

static u32 get_most_recent_subpass_index_writing_to_depth(const vulkan_render_pass_description_t* pass)
{
	u32 index = U32_MAX;
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		vulkan_subpass_description_t* subpass = &pass->subpass_descriptions[i];
		if(subpass->depth_stencil_attachment != U32_MAX)
			index = i;
	}
	_debug_assert__(index != U32_MAX);
	return index;
}

static VkSubpassDependency* create_and_resolve_subpass_dependencies(memory_allocator_t* allocator, const vulkan_render_pass_description_t* pass, const vulkan_render_pass_description_t* next_pass, u32 OUT dependency_count)
{
	BUFFER dependencies = memory_allocator_buf_new(allocator, VkSubpassDependency);
	
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		vulkan_subpass_description_t* subpass = &pass->subpass_descriptions[i];
		vulkan_subpass_description_t* next_subpass = (i < (pass->subpass_count - 1)) ? (subpass + 1) : NULL;

		if(subpass->depth_stencil_attachment != U32_MAX)
		{
			/* 	let the image load operation (clear) happen first
				NOTE: VK_ATTACHMENT_LOAD_OP_CLEAR is a write operation 
			 */
			VkSubpassDependency dependency = 
			{ 
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			};

			try_add_dependency(&dependencies, &dependency);

			/* 	let the image layout transition happen first
				NOTE: VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL is a write operation) 
			 */
			dependency = (VkSubpassDependency)
			{
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.dstAccessMask = 0
			};

			try_add_dependency(&dependencies, &dependency);
		}

		if(subpass->color_attachment_count > 0)
		{
			/* 	let the image layout transition happen first
				NOTE: VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL is a write operation 
			*/
			VkSubpassDependency dependency = 
			{
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
			};

			try_add_dependency(&dependencies, &dependency);
		}

		if(next_subpass != NULL)
		{
			if(is_next_subpass_writes_any_color(next_subpass, subpass))
			{
				/* let the write from this subpass to complete then allow write in the next pass */
				VkSubpassDependency dependency =
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}

			if(is_next_subpass_reads_any_color(next_subpass, subpass))
			{
				/* let color write complete and then allow read in the next pass */
				VkSubpassDependency dependency =
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}

			/* 	if the next subpass writes to the same depth stencil attachment this subpass writes to, 
				add another subpass dependency to let this subpass write first and then the next subpass
			 */
			if(is_next_subpass_writes_depth(next_subpass, subpass))
			{
				VkSubpassDependency dependency = 
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
					.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}
		}
	}

	if((next_pass != NULL) && is_renderpass_reads_depth(next_pass, pass))
	{
		/* let the write complete and then allow read in the next pass */
		VkSubpassDependency dependency = 
		{
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			.srcSubpass = get_most_recent_subpass_index_writing_to_depth(pass),
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
		};

		try_add_dependency(&dependencies, &dependency);
	}
	
	OUT dependency_count = CAST_TO(u32, buf_get_element_count(&dependencies));
	return CAST_TO(VkSubpassDependency*, buf_get_ptr(&dependencies));

}

/*	merges the subpass depedencies which targets the same subpass pair,
	and removes duplicate subpass dependencies.
 */
static VkSubpassDependency* optimize_subpass_dependencies(memory_allocator_t* allocator, VkSubpassDependency* dependencies, u32 OUT dependency_count)
{
	BUFFER new_dependencies = memory_allocator_buf_new(allocator, VkSubpassDependency);

	u32 count = *dependency_count;
	for(u32 i = 0; i < count; i++)
		try_add_dependency(&new_dependencies, &dependencies[i]);

	OUT dependency_count = count = buf_get_element_count(&new_dependencies);

	return (count == 0) ? NULL : buf_get_ptr(&new_dependencies);
}

RENDERER_API void vulkan_render_pass_description_builder_resolve_dependencies(vulkan_render_pass_description_builder_t* builder)
{
	if(builder->is_dependencies_resolved)
		return;
	u32 count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < count; i++)
	{
		vulkan_render_pass_description_t* pass = buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, i);
		if(pass->subpass_dependency_count > 0)
		{
			/* TODO: if subpass dependency already provided by the user, we may try optimizing them here. */
			continue;
		}
		vulkan_render_pass_description_t* next_pass = (i == (count - 1U)) ? NULL : buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, i + 1);
		/* TODO: we need to move some code from vulkan_render_pass_description.c  to vulkan_render_pass_description_builder.c
		 * especially, vulkan_render_pass_description_add_dependency(), the memory is managed by the vulkan_render_pass_description_t
		 * but it is supposed to be managed by its builder. */
		pass->subpass_dependencies = create_and_resolve_subpass_dependencies(builder->allocator, pass, next_pass, &pass->subpass_dependency_count);
	}
	builder->is_dependencies_resolved = true;
}

static vulkan_render_pass_description_t* get_description(vulkan_render_pass_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	return buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_render_pass_description_t* get_bound_description(vulkan_render_pass_description_builder_t* builder)
{
	return get_description(builder, builder->bind_index);
}


RENDERER_API void vulkan_render_pass_description_builder_begin_pass(vulkan_render_pass_description_builder_t* builder, vulkan_render_pass_type_t type)
{
	builder->is_dependencies_resolved = false;
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_begin(builder->allocator, description, type);
}

RENDERER_API void vulkan_render_pass_description_builder_add_input(vulkan_render_pass_description_builder_t* builder, glsl_type_t type, u32 index, u32 binding)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_input(builder->allocator, description, type, index, binding);
}

RENDERER_API void vulkan_render_pass_description_builder_add_attachment(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_type_t type)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_attachment(description, type);
}


RENDERER_API void vulkan_render_pass_description_builder_begin_subpass(vulkan_render_pass_description_builder_t* builder, u32 pipeline_index)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_begin_subpass(builder->allocator, description, pipeline_index);
}

RENDERER_API void vulkan_render_pass_description_builder_add_attachment_reference(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_attachment_reference(builder->allocator, description, type, reference, binding);
}

RENDERER_API void vulkan_render_pass_description_builder_end_subpass(vulkan_render_pass_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_end_subpass(builder->allocator, description);
}


RENDERER_API void vulkan_render_pass_description_builder_add_subpass_dependency(vulkan_render_pass_description_builder_t* builder, VkSubpassDependency* dependency)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_subpass_dependency(description, dependency);
}

RENDERER_API void vulkan_render_pass_description_builder_end_pass(vulkan_render_pass_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_end(builder->allocator, description);
}
