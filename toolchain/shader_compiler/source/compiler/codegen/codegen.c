#include <shader_compiler/compiler/codegen/codegen.h>
#include <shader_compiler/compiler/codegen/gfx_pipeline.h>
#include <shader_compiler/compiler/codegen/glsl.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/compiler/grammar.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>

static v3d_generic_node_t* find_child_node_if_name(v3d_generic_node_t* node, const char* name, const char* const start)
{
	for(u32 i = 0; i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		_ASSERT(child->qualifier_count >= 1);
		u32_pair_t pair = child->qualifiers[child->qualifier_count - 1];
		if(safe_strncmp(pair.start + start, name, U32_PAIR_DIFF(pair)) == 0)
			return child;
	}
	return NULL;
}

static u32 count_child_node_if_name(v3d_generic_node_t* node, const char* name, const char* const start)
{
	u32 count = 0;
	for(u32 i = 0; i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		_ASSERT(child->qualifier_count >= 1);
		u32_pair_t pair = child->qualifiers[child->qualifier_count - 1];
		if(safe_strncmp(pair.start + start, name, U32_PAIR_DIFF(pair)) == 0)
			count++;
	}
	return count;
}

static bool foreach_child_node_if_name(v3d_generic_node_t* node, const char* name, codegen_buffer_t* writer, void (*visitor)(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 count, void* user_data), void* user_data, compiler_ctx_t* ctx)
{
	u32 count = 0;
	for(u32 i = 0; i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		_ASSERT(child->qualifier_count >= 1);
		u32_pair_t pair = child->qualifiers[child->qualifier_count - 1];
		if(safe_strncmp(pair.start + ctx->src, name, U32_PAIR_DIFF(pair)) == 0)
		{
			count++;
			visitor(child, ctx, writer, count, user_data);
		}
	}
	return count != 0;
}


typedef enum attachment_type_t
{
	ATTACHMENT_TYPE_COLOR = 0UL,
	ATTACHMENT_TYPE_DEPTH = 2UL,
	ATTACHMENT_TYPE_STENCIL = 4UL
} attachment_type_t;

typedef struct color_location_t
{ 
	u32 attachment_index;
	u32 location;
} color_location_t;

/* descriptor bind information */
typedef struct bind_info_t
{
	u32 attachment_index;
	u32 set_number;
	u32 binding_number;
} bind_info_t;

typedef struct subpass_analysis_t
{
	/* list of color writes */
	u32* color_writes;
	union
	{
		u32 color_write_count;
		u32 color_location_count;
	};
	/* write location mapping of the above color writes */
	color_location_t* color_locations;
	/* list of color reades */
	u32* color_reads;
	union 
	{
		u32 color_read_count;
		u32 color_bind_count;
	};
	bind_info_t* color_binds;
	/* depth attachment index for read */
	u32 depth_read;
	/* descriptor bind info for the depth attachment */
	bind_info_t depth_bind;
	/* depth attachment index for write, otherwise U32_MAX */
	u32 depth_write;
} subpass_analysis_t;

typedef struct render_pass_analysis_t
{
	/* list of unique attachments to be created/satified for this renderpass after loading the shader */
	attachment_type_t* attachments;
	u32 attachment_count;
	/* index of the depth attachment in the above list of attachments */
	u32 depth_attachment_index;
	/* list of subpass analyses */
	subpass_analysis_t* subpasses;
	u32 subpass_count;

	/* list of indices referencing attachments in the previous render pass */
	u32* color_reads;
	union
	{
		u32 color_read_count;
		u32 color_bind_count;
	};
	/* descriptor bind info for each color read attachment (input attachment) */
	bind_info_t* color_binds;
	/* index of the depth attachment in the previous render pass */
	u32 depth_read;
	/* descriptor bind info for the depth attachment in the previous render pass */
	bind_info_t depth_bind;
} render_pass_analysis_t;

typedef struct render_pass_user_data_t
{
	u32 render_pass_count;
	render_pass_analysis_t* prev_analysis;
} render_pass_user_data_t;

static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 count, void* user_data);
static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 count, void* user_data);
static void codegen_subpass(v3d_generic_node_t* node, subpass_analysis_t* analysis, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 rpindex, u32 spindex);
static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 count, void* user_data);
static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 count, void* user_data);

static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	/* as per the v3d binary spec, the property descriptions come first */
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_PROPERTIES], writer, codegen_descriptions, NULL, ctx))
	{
		/* if there is no properties block then just write the number of material properties to be zero */
		binary_writer_u32(writer->main, 0);
	}
	/* the layout descriptions come after the property descriptions */
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_LAYOUT], writer, codegen_descriptions, NULL, ctx))
	{
		/* if there is no layout block then just write the number of layout descriptions to be zero */
		binary_writer_u32(writer->main, 0);
	}

	u32 render_pass_count = count_child_node_if_name(node, keywords[KEYWORD_RENDERPASS], ctx->src);
	binary_writer_u32(writer->main, render_pass_count);
	
	render_pass_user_data_t data = { render_pass_count, NULL };
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_RENDERPASS], writer, codegen_renderpass, &data, ctx))
	{
		/* if there is no RenderPass block then just write the number of RenderPasses to be zero */
		binary_writer_u32(writer->main, 0);
	}
}

static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 iteration, void* user_data)
{
	write_layout(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer, true);
}

/* sp.loctoat */
static u32 subpass_location_to_attachment_index(subpass_analysis_t* subpass, u32 location)
{
	for(u32 i = 0; i < subpass->color_write_count; i++)
		if(subpass->color_locations[i].location == location)
			return subpass->color_locations[i].attachment_index;
	debug_log_warning("[Codegen] There is no color location found with location %lu, returning U32_MAX", location);
	return U32_MAX;
}

static u32_pair_t node_name(v3d_generic_node_t* node)
{
	if(node->qualifier_count == 0)
		return U32_PAIR_INVALID;
	return node->qualifiers[node->qualifier_count - 1];
}

static u32 strntoul(const char* str, u32 len)
{
	char str1[len + 1];
	memcpy(str1, str, len);
	str1[len] = 0;
	return strtoul(str1, NULL, 0);
}

static u32 parse_set(const char* str, u32 len)
{
	if(safe_strncmp(str, "CAMERA_SET", len) == 0) return 0;
	else if(safe_strncmp(str, "GLOBAL_SET", len) == 0) return 1;
	else if(safe_strncmp(str, "RENDER_SET", len) == 0) return 2;
	else if(safe_strncmp(str, "SUB_RENDER_SET", len) == 0) return 3;
	else if(safe_strncmp(str, "MATERIAL_SET", len) == 0) return 4;
	else if(safe_strncmp(str, "OBJECT_SET", len) == 0) return 5;
	
	DEBUG_LOG_ERROR("[Codegen] Unrecognized set \"%.*s\"", len, str);

	return U32_MAX;
}

static s32 signed_sub(u32 op1, u32 op2)
{
	return (CAST_TO(s32, op1) - CAST_TO(s32, op2));
}

static u32 parse_binding(const char* str, u32 len)
{
	if(safe_strncmp(str, "CAMERA_PROPERTIES_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "CAMERA_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "LIGHT_BINDING", len) == 0) return 1;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING0", len) == 0) return 0;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING1", len) == 0) return 1;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING2", len) == 0) return 2;
	else if(safe_strncmp(str, "MATERIAL_PROPERTIES_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "TEXTURE_BINDING0", len) == 0) return 1;
	else if(safe_strncmp(str, "TEXTURE_BINDING1", len) == 0) return 2;
	else if(safe_strncmp(str, "TEXTURE_BINDING2", len) == 0) return 3;
	else if(safe_strncmp(str, "TEXTURE_BINDING3", len) == 0) return 4;
	else if(safe_strncmp(str, "TEXTURE_BINDING4", len) == 0) return 5;
	else if(safe_strncmp(str, "TEXTURE_BINDING5", len) == 0) return 6;
	else if(safe_strncmp(str, "TEXTURE_BINDING6", len) == 0) return 7;
	else if(safe_strncmp(str, "TEXTURE_BINDING7", len) == 0) return 8;
	else if(safe_strncmp(str, "TEXTURE_BINDING8", len) == 0) return 9;
	else if(safe_strncmp(str, "TRANSFORM_BINDING", len) == 0) return 0;

	DEBUG_LOG_ERROR("[Codegen] Unrecognized binding \"%.*s\"", len, str);

	return U32_MAX;
}

static void run_sub_pass_analysis(v3d_generic_node_t* subpass, compiler_ctx_t* ctx, render_pass_analysis_t* renderpass_analysis, subpass_analysis_t* prev_analysis, BUFFER* attachments, subpass_analysis_t OUT analysis)
{
	debug_log_info("[Subpass analysis] begin");
	/* intially the depth write must be invalid */
	analysis->depth_write = U32_MAX;
	/* intially the depth read must be invalid */
	analysis->depth_read = U32_MAX;

	/* because we are working with legacy compiler code, we need to find the unparsed gfx pipeline node to parse it to get more detail about it */
	v3d_generic_node_t* unparsed_gfx_pipeline = find_child_node_if_name(subpass, keywords[KEYWORD_GFXPIPELINE], ctx->src);
	_ASSERT(unparsed_gfx_pipeline != NULL);

	/* parse the unparsed gfx pipeline to get more detail about the pipeline configuration */
	u32_pair_t unparsed = unparsed_gfx_pipeline->unparsed;
	AUTO result = ppsr_v3d_generic_parse(unparsed.start + ctx->src, U32_PAIR_DIFF(unparsed));
	_ASSERT(result.result == PPSR_SUCCESS);

	v3d_generic_node_t* gfx_pipeline = result.root;
	_assert(gfx_pipeline != NULL);

	/* if there is no depth attachment in the render pass and the pipeline has some information about depth or stencil then 
		this render pass must have a depth attachment.

		NOTE: we are determining the need of a depth attachment ahead as it is needed when we start analysing the attributes on this subpass
	 */
	if(find_child_node_if_name(gfx_pipeline, "depthStencil", ctx->src + unparsed.start) != NULL)
	{
		if(renderpass_analysis->depth_attachment_index == U32_MAX)
		{
			renderpass_analysis->depth_attachment_index = buf_get_element_count(attachments);
			/* at0 = @attachment_new depth */
			buf_push_auto(attachments, ATTACHMENT_TYPE_DEPTH);
			debug_log_info("[Subpass analysis] creating depth attachment, attachment index: %lu", renderpass_analysis->depth_attachment_index);
		}
		analysis->depth_write = renderpass_analysis->depth_attachment_index;
		debug_log_info("[Subpass analysis] depth write, attachment index: %lu", analysis->depth_write);
	}

	/* list of color reads, i.e [Read(color = ...)] attribute on a subpass */
	BUFFER color_reads = buf_new(u32);
	/* list of binding infos, i.e [Read(color = ..., set = <set_number>, binding = <binding_number>)] attribute on a subpass */
	BUFFER color_binds = buf_new(bind_info_t);
	/* list of color writes, i.e. [Write(color = ...)] attribute on a subpass */
	BUFFER color_writes = buf_new(u32);
	/* list of color locations, i.e. [Write(color = ..., location = ...)] attribute on a subpass */
	BUFFER color_locations = buf_new(color_location_t);

	/* now analyse the attributes on the subpass */

	/* for each attribute on this subpass */
	s32 color_input_count = 0;
	for(u32 j = 0; j < subpass->attribute_count; j++)
	{
		v3d_generic_attribute_t* attrib = &subpass->attributes[j];

		/* if this is a [Read(...)] attribute */
		if(safe_strncmp(attrib->name.start + ctx->src, attribute_keywords[ATTRIBUTE_READ], U32_PAIR_DIFF(attrib->name)) == 0)
		{
			/* the [Read(...)] attribute requires three arguments as of now */
			_ASSERT(attrib->argument_count == 3);

			/* if the first argument is given as "depth" */
			if(safe_strncmp(attrib->arguments[0].start + ctx->src, "depth", U32_PAIR_DIFF(attrib->arguments[0])) == 0)
			{
				analysis->depth_read = renderpass_analysis->depth_attachment_index;
				analysis->depth_bind.attachment_index = analysis->depth_read;
				analysis->depth_bind.set_number = parse_set(attrib->arguments[1].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[1]));
				analysis->depth_bind.binding_number = parse_binding(attrib->arguments[2].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[2]));
				debug_log_info("[Subpass analysis] depth read, attachment index: %lu, set_number: %lu, binding_number: %lu", analysis->depth_bind.set_number, analysis->depth_bind.binding_number);
			}
			else
			{
				/* color = location */
				_ASSERT(attrib->parameter_count >= 1);
				_ASSERT(!U32_PAIR_IS_INVALID(attrib->parameters[0]));
				
				/* if the first argument is given as a digit with the parameter "color" */
				if(safe_strncmp(attrib->parameters[0].start + ctx->src, "color", U32_PAIR_DIFF(attrib->parameters[0])) == 0)
				{
					/* the [Read(...)], and [Write(...)] can only be applied on subsequent passes, not on the very first pass */
					_ASSERT(prev_analysis != NULL);

					/*	since we are referencing the attachments by the locations in the previous subpass, we must get the corresponding
						attachment reference from the previous subpass.
						for example, [Read(color = 2, ...)] doesn't actually references the attachment number 2 in the list of global/renderpass attachments,
						it refers to the attachment the previous subpass is writing with the location 2 to it.

						var0 = sb0.loctoat(1)
						@attachment_input var0
					 */
					u32 location = strntoul(attrib->arguments[0].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[0]));
					u32 attachment_index = subpass_location_to_attachment_index(prev_analysis, location);
					buf_push(&color_reads, &attachment_index);
					/* 	add the descriptor set and binding information for this attachment
					 	@attachment_bind var0 sbrd_set inat_id0 
					 */
					bind_info_t bind = 
					{
						.attachment_index = attachment_index,
						.set_number = parse_set(attrib->arguments[1].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[1])),
						.binding_number = parse_binding(attrib->arguments[2].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[2]))
					};
					buf_push(&color_binds, &bind);
					debug_log_info("[Subpass analysis] color read, attachment index: %lu (prev.location = %lu), set_number: %lu (%.*s), binding_number: %lu (%.*s)", 
									attachment_index, location, bind.set_number,
									U32_PAIR_DIFF(attrib->arguments[1]), attrib->arguments[1].start + ctx->src,
									bind.binding_number,
									U32_PAIR_DIFF(attrib->arguments[2]), attrib->arguments[2].start + ctx->src);
				}
			}
		}
		/* if this is a [Write(...)] attribute */
		else if(safe_strncmp(attrib->name.start + ctx->src, attribute_keywords[ATTRIBUTE_WRITE], U32_PAIR_DIFF(attrib->name)) == 0)
		{
			/* the [Write(...)] attribute requires two arguments */
			_ASSERT(attrib->argument_count == 2);
			_ASSERT(!U32_PAIR_IS_INVALID(attrib->parameters[0]));

			/* if the first argument is a digit with parameter "color" */
			if(safe_strncmp(attrib->parameters[0].start + ctx->src, "color", U32_PAIR_DIFF(attrib->parameters[0])) == 0)
			{
				_ASSERT(prev_analysis != NULL);

				/*	parse the location and then translate it to the attachment reference.

					var0 = sb0.loctoat(1)
					@attachment_color var0
					@attachment_loc var0 1
				 */
				u32 location = strntoul(attrib->arguments[0].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[0]));
				u32 attachment_index = subpass_location_to_attachment_index(prev_analysis, location);
				buf_push(&color_writes, &attachment_index);

				/* add the color write location, i.e layout(location = <location>) */
				color_location_t _location = 
				{
					.attachment_index = attachment_index,
					.location = strntoul(attrib->arguments[1].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[1]))
				};

				buf_push(&color_locations, &_location);
				debug_log_info("[Subpass analysis] color write, attachment index: %lu (this.location = %lu) (prev.location = %lu)", attachment_index, _location.location, location);
			}
			else DEBUG_LOG_ERROR("[Codegen] Unknown Error");
		}
	}

	analysis->color_reads = buf_get_ptr(&color_reads);
	analysis->color_binds = buf_get_ptr(&color_binds);
	analysis->color_read_count = buf_get_element_count(&color_reads);
	analysis->color_write_count = buf_get_element_count(&color_writes);

	/* now analyse the details of the gfx pipeline */
	for(u32 j = 0; j < gfx_pipeline->child_count; j++)
	{
		v3d_generic_node_t* node = gfx_pipeline->childs[j];
		u32_pair_t pair = node_name(node);

		/* if the block is a colorBlend block */
		if(safe_strncmp(pair.start + ctx->src + unparsed.start, "colorBlend", U32_PAIR_DIFF(pair)) == 0)
		{
			/* 	we are iterating through all the "attachment" nodes because each
				attachment nodes indicates a color write to an attachment. 
				for example, if there are 5 attachment blocks that means there would be
				5 layout(location = ...) in the glsl shader.
			 */
			u32 color_hints = 0;
			for(u32 k = 0; k < node->child_count; k++)
			{
				u32_pair_t pair = node_name(node->childs[k]);
				if(safe_strncmp(pair.start + ctx->src + unparsed.start, "attachment", U32_PAIR_DIFF(pair)) == 0)
				{
					color_hints++;

					/* if the number of attachment blocks exceeds the propagated attachments (from the previous subpass),
						then create new attachments and add write locations for them in this subpass.
					 */
					if(color_hints > analysis->color_write_count)
					{
						/* @attachment_color at1 */
						u32 attachment_index = buf_get_element_count(attachments);
						/* at1 = @attachment_new color */
						buf_push_auto(attachments, ATTACHMENT_TYPE_COLOR);
						buf_push(&color_writes, &attachment_index);
						/* @attachment_loc at1 2 */
						color_location_t location = 
						{
							.attachment_index = attachment_index,
							.location = color_hints - 1
						};
						buf_push(&color_locations, &location);
						debug_log_info("[Subpass analysis] creating color attachment, attachment index: %lu", attachment_index);
						debug_log_info("[Subpass analysis] color write, attachment index: %lu (this.location = %lu)", attachment_index, location.location);
					}
				}
			}
			break;
		}
	}

	analysis->color_locations = buf_get_ptr(&color_locations);
	analysis->color_writes = buf_get_ptr(&color_writes);
	analysis->color_write_count = buf_get_element_count(&color_writes);
	debug_log_info("[Subpass analysis] end");
}

static void reorder_attachments_phase1(render_pass_analysis_t* renderpass)
{
	if(renderpass->depth_attachment_index == U32_MAX) return;

	debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] begin");

	u32 prev_index = renderpass->depth_attachment_index;
	u32 depth_attachment = renderpass->attachments[renderpass->depth_attachment_index];
	for(u32 i = prev_index; i < (renderpass->attachment_count - 1); i++)
		renderpass->attachments[i] = renderpass->attachments[i + 1];
	renderpass->attachments[renderpass->attachment_count - 1] = depth_attachment;
	renderpass->depth_attachment_index = renderpass->attachment_count - 1;

	debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] previous depth attachment index: %lu, now: %lu", prev_index, renderpass->depth_attachment_index);

	for(u32 i = 0; i < renderpass->subpass_count; i++)
	{
		subpass_analysis_t* subpass = &renderpass->subpasses[i];
		if(subpass->depth_write != U32_MAX)
		{
			DEBUG_BLOCK( u32 depth_write = subpass->depth_write; )
			subpass->depth_write = renderpass->depth_attachment_index;
			debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] subpass depth write: %lu, now: %lu", depth_write, subpass->depth_write);
		}
		if(subpass->depth_read != U32_MAX)
		{
			DEBUG_BLOCK( u32 depth_read = subpass->depth_read; )
			subpass->depth_read = renderpass->depth_attachment_index;
			subpass->depth_bind.attachment_index = renderpass->depth_attachment_index;
			debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] subpass depth read: %lu, now: %lu", depth_read, subpass->depth_read);
		}

		for(u32 j = 0; j < subpass->color_read_count; j++)
		{
			if(subpass->color_reads[j] > prev_index)
			{
				DEBUG_BLOCK( u32 color_read = subpass->color_reads[j]; )
				subpass->color_reads[j]--;
				subpass->color_binds[j].attachment_index--;
				debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] subpass color read: %lu, now: %lu", color_read, subpass->color_reads[j]);
			}
		}

		for(u32 j = 0; j < subpass->color_write_count; j++)
		{
			if(subpass->color_writes[j] > prev_index)
			{
				DEBUG_BLOCK( u32 color_write = subpass->color_writes[j]; )
				subpass->color_writes[j]--;
				subpass->color_locations[j].attachment_index--;
				debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] subpass color write: %lu, now: %lu", color_write, subpass->color_writes[j]);
			}
		}
	}

	debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase1] end");
}

static void reorder_attachments_phase2(render_pass_analysis_t* renderpass)
{
	_ASSERT(renderpass->attachment_count >= 1);
	
	debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase2] begin");

	/* index of the color attachment which qualifies for presentation */
	u32 present_index = (renderpass->depth_attachment_index == U32_MAX) ? 
						(renderpass->attachment_count - 1) : 
						((renderpass->attachment_count > 1) ? (renderpass->attachment_count - 2) : U32_MAX);
	if(present_index == U32_MAX)
		return;

	u32 color_attachment = renderpass->attachments[present_index];
	for(u32 i = present_index; i > 0; i--)
		renderpass->attachments[i] = renderpass->attachments[i - 1];

	for(u32 i = 0; i < renderpass->subpass_count; i++)
	{
		subpass_analysis_t* subpass = &renderpass->subpasses[i];
		for(u32 j = 0;j < subpass->color_read_count; j++)
		{
			if(subpass->color_reads[j] < present_index)
			{
				DEBUG_BLOCK(u32 color_read = subpass->color_reads[j]; )
				subpass->color_reads[j]++;
				subpass->color_binds[j].attachment_index++;
				debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase2] subpass color read: %lu, now: %lu", color_read, subpass->color_reads[j]);
			}
			else if(subpass->color_reads[j] == present_index)
			{
				DEBUG_LOG_WARNING("[Renderpass analysis] [Attachment reorder] [Phase2] Swapchain attachment (the last one) is being read in subpass %lu", i);
				subpass->color_reads[j] = 0;
				subpass->color_binds[j].attachment_index = 0;
			}
		}
		for(u32 j = 0; j < subpass->color_write_count; j++)
		{
			if(subpass->color_writes[j] < present_index)
			{
				DEBUG_BLOCK(u32 color_write = subpass->color_writes[j]; )
				subpass->color_writes[j]++;
				subpass->color_locations[j].attachment_index++;
				debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase2] subpass color write: %lu, now: %lu", color_write, subpass->color_writes[j]);
			}
			else if(subpass->color_writes[j] == present_index)
			{
				subpass->color_writes[j] = 0;
				subpass->color_locations[j].attachment_index = 0;
			}
		}
	}
	debug_log_info("[Renderpass analysis] [Attachment reorder] [Phase2] end");
}

static void run_render_pass_analysis(v3d_generic_node_t* renderpass, compiler_ctx_t* ctx, render_pass_analysis_t* prev_analysis, render_pass_analysis_t OUT analysis)
{
	debug_log_info("[Renderpass analysis] begin");
	/* intitially the depth attachment index must be invalid */
	analysis->depth_attachment_index = U32_MAX;

	/* list of analysises of the subpasses in this renderpass */
	BUFFER subpasses = buf_new(subpass_analysis_t);

	/* list of unique attachments that has to be created after loading the shader  */
	BUFFER attachments = buf_new(attachment_type_t);
	
	/* stores the index of the previous subpass been analysed */
	u32 prev_index = U32_MAX;
	
	/* for each subpass node */
	for(u32 i = 0; i < renderpass->child_count; i++)
	{
		v3d_generic_node_t* subpass = renderpass->childs[i];

		/* get the pointer to the previous subpass using its indice in the list of subpasses
			NOTE: we are doing it with an index not with pointer because whenever the internal buffer resizes
				the pointer gets invalidated.
		 */
		subpass_analysis_t* prev_sub_analysis = (prev_index == U32_MAX) ? NULL : buf_get_ptr_at_typeof(&subpasses, subpass_analysis_t, prev_index);

		subpass_analysis_t sub_analysis = { };
		run_sub_pass_analysis(subpass, ctx, analysis, prev_sub_analysis, &attachments, &sub_analysis);

		buf_push(&subpasses, &sub_analysis);

		prev_index = i;
	}

	analysis->subpasses = buf_get_ptr(&subpasses);
	analysis->subpass_count = buf_get_element_count(&subpasses);
	analysis->attachments = buf_get_ptr(&attachments);
	analysis->attachment_count = buf_get_element_count(&attachments);

	/* initial depth attachment index must be invalid */
	analysis->depth_read = U32_MAX;
	analysis->depth_bind.attachment_index = U32_MAX;

	for(u32 i = 0; i < renderpass->attribute_count; i++)
	{
		v3d_generic_attribute_t* attrib = &renderpass->attributes[i];

		/* if this is a [Read(...)] attribute */
		if(safe_strncmp(attrib->name.start + ctx->src, attribute_keywords[ATTRIBUTE_READ], U32_PAIR_DIFF(attrib->name)) == 0)
		{
			/* the [Read(...)] attribute requires three arguments as of now */
			_ASSERT(attrib->argument_count == 3);

			/* if the first argument is given as "depth" */
			if(safe_strncmp(attrib->arguments[0].start + ctx->src, "depth", U32_PAIR_DIFF(attrib->arguments[0])) == 0)
			{
				_ASSERT(prev_analysis != NULL);
				if(prev_analysis->depth_attachment_index == U32_MAX)
					DEBUG_LOG_WARNING("[Codegen] The previous render pass doesn't have any depth attachment but you are trying read that in the next render pass");
				analysis->depth_read = prev_analysis->depth_attachment_index;
				analysis->depth_bind.attachment_index = analysis->depth_read;
				analysis->depth_bind.set_number = parse_set(attrib->arguments[1].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[1]));
				analysis->depth_bind.binding_number = parse_binding(attrib->arguments[2].start + ctx->src, U32_PAIR_DIFF(attrib->arguments[2]));
				debug_log_info("[Renderpass analysis] depth read, prev.attachment index: %lu, set number: %lu (%.*s), binding number: %lu (%.*s)",
								analysis->depth_read, analysis->depth_bind.set_number, 
								U32_PAIR_DIFF(attrib->arguments[1]), attrib->arguments[1].start + ctx->src,
								analysis->depth_bind.binding_number,
								U32_PAIR_DIFF(attrib->arguments[2]), attrib->arguments[2].start + ctx->src);
			}
			else
			{
				/* do nothing for now */
			}
		}
		/* if this is a [Write(...)] attribute */
		else if(safe_strncmp(attrib->name.start + ctx->src, attribute_keywords[ATTRIBUTE_WRITE], U32_PAIR_DIFF(attrib->name)) == 0)
		{
			/* do nothing for now */
		}
	}

	reorder_attachments_phase1(analysis);
	reorder_attachments_phase2(analysis);

	debug_log_info("[Renderpass analysis] end");
}

typedef enum render_pass_type_t
{
	/* the render pass only contains a single framebuffer, though not presentable in double or tripple buffering */
	RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER,
	/* the render pass contains multiple identical framebuffers, it is presentable in double and tripple buffering */
	RENDER_PASS_TYPE_SWAPCHAIN_TARGET
} render_pass_type_t;

static void write_layout2(compiler_ctx_t* ctx, codegen_buffer_t* writer, const char* format, ...)
{
	buf_clear(&ctx->string_buffer, NULL);
	va_list args;
	va_start(args, format);
 	buf_vprintf(&ctx->string_buffer, NULL, format, args);
 	va_end(args);
	write_layout(buf_get_ptr(&ctx->string_buffer), buf_peek_ptr(&ctx->string_buffer), writer, false);
}

static void codegen_render_set_binding_descriptions(render_pass_analysis_t* analysis, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 index)
{
	/* write the number of input descriptions */
	binary_writer_u16(writer->main, analysis->color_bind_count + ((analysis->depth_read == U32_MAX) ? 0 : 1));

	/* write the color input descriptions */
	for(u32 i = 0; i < analysis->color_bind_count; i++)
	{
		write_layout2(ctx, writer, 
					"fragment [%lu, %lu] uniform sampler2D rp%lucolor%lu;",
					analysis->color_binds[i].set_number,
					analysis->color_binds[i].binding_number,
					index,
					CAST_TO(u32, i));
	}

	/* write the depth input descriptions */
	if(analysis->depth_read != U32_MAX)
		write_layout2(ctx, writer, 
					"fragment [%lu, %lu] uniform sampler2D rp%ludepth;",
					analysis->depth_bind.set_number,
					analysis->depth_bind.binding_number,
					index);
}

static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 iteration, void* user_data)
{
	render_pass_user_data_t* data = CAST_TO(render_pass_user_data_t*, user_data);
	render_pass_analysis_t* analysis = new(render_pass_analysis_t);
	run_render_pass_analysis(node, ctx, data->prev_analysis, analysis);
	data->prev_analysis = analysis;

	_ASSERT(analysis->attachment_count != 0);

	/* write the type of the render pass, if this is the last pass then it must be type of SWAPCHAIN_TARGET */
	binary_writer_u32(writer->main, (iteration == data->render_pass_count) ? RENDER_PASS_TYPE_SWAPCHAIN_TARGET : RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);

	/* write the list of input attachments */
	u32 input_count = analysis->color_read_count + ((analysis->depth_read == U32_MAX) ? 0 : 1);
	binary_writer_u32(writer->main, input_count);
	binary_writer_write(writer->main, (void*)analysis->color_reads, analysis->color_read_count * sizeof(u32));
	binary_writer_write(writer->main, &analysis->depth_read, (input_count - analysis->color_read_count) * sizeof(u32));

	/* write the list of attachments */
	binary_writer_u32(writer->main, analysis->attachment_count);
	binary_writer_write(writer->main, (void*)analysis->attachments, analysis->attachment_count * sizeof(u32));

	/* let's keep the subpass dependencies out of the picture for now */
	binary_writer_u32(writer->main, 0);

	/* write the render set binding descriptions */
	codegen_render_set_binding_descriptions(analysis, ctx, writer, iteration);

	/* write the list of subpass descriptions */
	binary_writer_u32(writer->main, analysis->subpass_count);

	for(u32 i = 0; i < analysis->subpass_count; i++)
		codegen_subpass(node->childs[i], &analysis->subpasses[i], ctx, writer, iteration, i);
}

static void codegen_sub_render_set_binding_descriptions(subpass_analysis_t* analysis, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 rpindex, u32 spindex)
{
	/* write the number of input descriptions */
	binary_writer_u16(writer->main, analysis->color_bind_count + ((analysis->depth_read == U32_MAX) ? 0 : 1));

	/* write the color input descriptions */
	for(u32 i = 0; i < analysis->color_bind_count; i++)
	{
		write_layout2(ctx, writer, 
					"fragment [%lu, %lu] uniform sampler2D sb%lurp%lucolor%lu;",
					analysis->color_binds[i].set_number,
					analysis->color_binds[i].binding_number,
					rpindex,
					spindex,
					CAST_TO(u32, i));
	}

	/* write the depth input descriptions */
	if(analysis->depth_read != U32_MAX)
		write_layout2(ctx, writer, 
					"fragment [%lu, %lu] uniform sampler2D sb%lurp%ludepth;",
					analysis->depth_bind.set_number,
					analysis->depth_bind.binding_number,
					rpindex,
					spindex);
}

static void codegen_subpass(v3d_generic_node_t* node, subpass_analysis_t* analysis, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 rpindex, u32 spindex)
{
	/* write input attachment list */
	u32 input_count = analysis->color_read_count + ((analysis->depth_read != U32_MAX) ? 1 : 0);
	binary_writer_u32(writer->main, input_count);
	binary_writer_write(writer->main, (void*)analysis->color_reads, analysis->color_read_count * sizeof(u32));
	binary_writer_write(writer->main, &analysis->depth_read, (input_count - analysis->color_read_count) * sizeof(u32));

	/* write color attachment list */
	binary_writer_u32(writer->main, analysis->color_write_count);
	binary_writer_write(writer->main, (void*)analysis->color_writes, analysis->color_write_count * sizeof(u32));

	/* let's keep the preserve attachment list zero for now */
	binary_writer_u32(writer->main, 0);

	/* write the depth attachment index */
	binary_writer_u32(writer->main, analysis->depth_write);

	/* write the pipeline offset */
	ctx->current_pipeline_index++;
	_ASSERT((MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index) < MARK_ID_PIPELINE_OFFSET_MAX);
	binary_writer_u32_mark(writer->main, MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index);
	binary_writer_u32_set(writer->main, MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index, binary_writer_pos(writer->data));

	/* WRITE PIPELINE DESCRIPTION INTO THE DATA SECTION */

	/* write the vulkan_graphics_pipeline_description_t object into the data section */
	codegen_pipeline(find_child_node_if_name(node, keywords[KEYWORD_GFXPIPELINE], ctx->src), ctx, writer, 0, NULL);

	/* write the spir-v binaries into the data section */
	codegen_glsl(find_child_node_if_name(node, keywords[KEYWORD_GLSL], ctx->src), ctx, writer, 0, NULL);

	/* write the sub render set binding descriptions */
	codegen_sub_render_set_binding_descriptions(analysis, ctx, writer, rpindex, spindex);
}

static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 iteration, void* user_data)
{
	_ASSERT(node != NULL);
	write_gfx_pipeline(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer);
}

static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer, u32 iteration, void* user_data)
{
	_ASSERT(node != NULL);
	write_glsl(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer, ctx);
}

SC_API void codegen(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	codegen_shader(node, ctx, writer);
}
