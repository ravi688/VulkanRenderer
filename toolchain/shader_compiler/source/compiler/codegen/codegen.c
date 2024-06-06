/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: codegen.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/codegen/codegen.h>
#include <shader_compiler/compiler/codegen/gfx_pipeline.h>
#include <shader_compiler/compiler/codegen/glsl.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/compiler/grammar.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <glslcommon/glsl_types.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>

#include <shader_compiler/sb.h>

/* 	finds a child node in the parent node 'node' if matches name 'name'.
	node: parent node
	name: ptr to the name string to be compared with (must be null terminated)
	start: ptr to the source string
	returns a valid ptr if a match is found, otherwise NULL
 */
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

/*	returns the number of child node found with matching name 'name', zero otherwise.
	node: parent node
	name: ptr to the name string to be compared with (must be null terminated)
	start: ptr to the source string
	returns an unsigned 32-bit integer (number of nodes found) otherwise zero.
 */
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

/*	visits each child node matching name 'name'
	node: parent node
	name: ptr to the name string to be compared with (must be null terminated)
	writer: ptr the codegen_buffer_t* object (it might be needed to the visitor)
	visitor: ptr to the visitor function
		node: ptr to the node for the current invocation
		ctx: ptr to the compiler context object
		writer: ptr to the codegne_buffer_t* object
		count: zero based invocation count (0 means first invocation)
		user_data: ptr to the user data object
	user_data: ptr to the user data needed by the visitor
	ctx: ptr to the compiler context object	
	returns true if the number of visits greater than 0 otherwise false.
 */
static bool foreach_child_node_if_name(v3d_generic_node_t* node, const char* name, void (*visitor)(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data), void* user_data, compiler_ctx_t* ctx)
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
			visitor(child, ctx, count, user_data);
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

static void subpass_analysis_destroy(com_allocation_callbacks_t* callbacks, subpass_analysis_t* analysis)
{
	if(analysis->color_write_count > 0)
	{
		com_deallocate(callbacks, analysis->color_writes);
		com_deallocate(callbacks, analysis->color_locations);
	}
	if(analysis->color_read_count > 0)
	{
		com_deallocate(callbacks, analysis->color_reads);
		com_deallocate(callbacks, analysis->color_binds);
	}
}

static void render_pass_analysis_destroy(com_allocation_callbacks_t* callbacks, render_pass_analysis_t* analysis)
{
	if(analysis->attachment_count > 0)
		com_deallocate(callbacks, analysis->attachments);
	for(u32 i = 0; i < analysis->subpass_count; i++)
		subpass_analysis_destroy(callbacks, &analysis->subpasses[i]);
	if(analysis->subpass_count > 0)
		com_deallocate(callbacks, analysis->subpasses);
	if(analysis->color_read_count > 0)
	{
		com_deallocate(callbacks, analysis->color_reads);
		com_deallocate(callbacks, analysis->color_binds);
	}
}

typedef struct render_pass_user_data_t
{
	u32 render_pass_count;
	render_pass_analysis_t* prev_analysis;
} render_pass_user_data_t;

static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx);
static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data);
static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data);
static void codegen_subpass(v3d_generic_node_t* node, subpass_analysis_t* analysis, compiler_ctx_t* ctx, u32 rpindex, u32 spindex);
static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data);
static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data);

static void codegen_vertex_buffer_layout(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data);
static void codegen_buffer_layouts_and_samplers(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data);

static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx)
{
	/* as per the v3d binary spec, the property descriptions come first */
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_PROPERTIES], (ctx->sl_version == SL_VERSION_2023) ? codegen_buffer_layouts_and_samplers : codegen_descriptions, NULL, ctx))
	{
		/* if there is no properties block then just write the number of material properties to be zero */
		binary_writer_u16(ctx->codegen_buffer->main, 0);
	}
	/* the layout descriptions come after the property descriptions */
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_LAYOUT], (ctx->sl_version == SL_VERSION_2023) ? codegen_vertex_buffer_layout : codegen_descriptions, NULL, ctx))
	{
		/* if there is no layout block then just write the number of layout descriptions to be zero */
		binary_writer_u16(ctx->codegen_buffer->main, 0);
	}

	u32 render_pass_count = count_child_node_if_name(node, keywords[KEYWORD_RENDERPASS], ctx->src);
	binary_writer_u32(ctx->codegen_buffer->main, render_pass_count);
	
	render_pass_user_data_t data = { render_pass_count, NULL };
	if(!foreach_child_node_if_name(node, keywords[KEYWORD_RENDERPASS], codegen_renderpass, &data, ctx))
	{
		/* if there is no RenderPass block then just write the number of RenderPasses to be zero */
		binary_writer_u32(ctx->codegen_buffer->main, 0);
	}
}

const char* u32_pair_get_str(compiler_ctx_t* ctx, u32_pair_t pair)
{
	if(U32_PAIR_IS_INVALID(pair))
		return "";
	else
		return ctx->src + pair.start;
}

static u32 try_parse_u32_pair_str_to_u32(compiler_ctx_t* ctx, u32_pair_t pair)
{
	return try_parse_to_u32(u32_pair_get_str(ctx, pair));
}


#define __ATTRIBUTE_HANDLER_PARAMS__ compiler_ctx_t* ctx, v3d_generic_attribute_t* attribute, v3d_generic_node_t* node, void* user_data
typedef void (*attribute_callback_handler_t)(__ATTRIBUTE_HANDLER_PARAMS__);

static const char* g_vertex_attribute_bind_names[] = 
{
	"position",
	"normal",
	"tangent",
	"texcoord",
	"color"
};

static const char* g_mesh_layouts[] = 
{
	"sge_optimal",
	"sge_separate",
	"sge_interleaved"
};

static u32 g_mesh_layout_emit_values[][SIZEOF_ARRAY(g_vertex_attribute_bind_names)][2] = 
{
	/* sge_optimal */
	{
		/* position */
		{ SGE_POSITION_BINDING_OPTIMAL, SGE_POSITION_LOCATION_OPTIMAL }, 
		/* normal */
		{ SGE_NORMAL_BINDING_OPTIMAL, SGE_NORMAL_LOCATION_OPTIMAL },
		/* tangent */
		{ SGE_TANGENT_BINDING_OPTIMAL, SGE_TANGENT_LOCATION_OPTIMAL },
		/* texcoord */
		{ SGE_TEXCOORD_BINDING_OPTIMAL, SGE_TEXCOORD_LOCATION_OPTIMAL },
		/* color */
		{ SGE_COLOR_BINDING_OPTIMAL, SGE_COLOR_LOCATION_OPTIMAL }
	},
	/* sge_separate */
	{
		/* position */
		{ SGE_POSITION_BINDING_SEPARATE, SGE_POSITION_LOCATION_SEPARATE }, 
		/* normal */
		{ SGE_NORMAL_BINDING_SEPARATE, SGE_NORMAL_LOCATION_SEPARATE },
		/* tangent */
		{ SGE_TANGENT_BINDING_SEPARATE, SGE_TANGENT_LOCATION_SEPARATE },
		/* texcoord */
		{ SGE_TEXCOORD_BINDING_SEPARATE, SGE_TEXCOORD_LOCATION_SEPARATE },
		/* color */
		{ SGE_COLOR_BINDING_SEPARATE, SGE_COLOR_LOCATION_SEPARATE }
	},
	/* sge_interleaved */
	{
		/* position */
		{ SGE_POSITION_BINDING_INTERLEAVED, SGE_POSITION_LOCATION_INTERLEAVED }, 
		/* normal */
		{ SGE_NORMAL_BINDING_INTERLEAVED, SGE_NORMAL_LOCATION_INTERLEAVED },
		/* tangent */
		{ SGE_TANGENT_BINDING_INTERLEAVED, SGE_TANGENT_LOCATION_INTERLEAVED },
		/* texcoord */
		{ SGE_TEXCOORD_BINDING_INTERLEAVED, SGE_TEXCOORD_LOCATION_INTERLEAVED },
		/* color */
		{ SGE_COLOR_BINDING_INTERLEAVED, SGE_COLOR_LOCATION_INTERLEAVED }
	}
};

static int safe_stru32pncmp(compiler_ctx_t* ctx, u32_pair_t pair, const char* str)
{
	return safe_strncmp(u32_pair_get_str(ctx, pair), str, U32_PAIR_DIFF(pair));
}

static u32 strs_find_u32_pair(compiler_ctx_t* ctx, const char* const* strs, u32 str_count, u32_pair_t pair)
{
	for(u32 i = 0; i < str_count; i++)
		if(safe_stru32pncmp(ctx, pair, strs[i]) == 0)
			return i;
	return U32_MAX;
}

static u32_pair_t u32_pairs_find_any_str(compiler_ctx_t* ctx, u32_pair_t* pairs, u32 pair_count, const char* const* strs, u32 str_count)
{
	for(u32 i = 0; i < pair_count; i++)
	{
		u32 index = strs_find_u32_pair(ctx, strs, str_count, pairs[i]);
		if(index != U32_MAX)
			return (u32_pair_t) { i, index };
	}
	return (u32_pair_t) { U32_MAX, U32_MAX };
}

static u32 find_mesh_layout_index(compiler_ctx_t* ctx, v3d_generic_attribute_t* attributes, u32 attribute_count)
{
	for(u32 i = 0; i < attribute_count; i++)
	{
		AUTO attribute = &attributes[i];
		if(safe_stru32pncmp(ctx, attribute->name, "MeshLayout") == 0)
		{
			if(attribute->argument_count == 1)
			{
				u32 index = strs_find_u32_pair(ctx, g_mesh_layouts, SIZEOF_ARRAY(g_mesh_layouts), attribute->arguments[0]);
				if(index != U32_MAX)
					return index;
				else DEBUG_LOG_FETAL_ERROR("MeshLayout() has been passed an incorrect argument \"%.*s\"", U32_PAIR_DIFF(attribute->arguments[0]), u32_pair_get_str(ctx, attribute->arguments[0]));
			}
			else DEBUG_LOG_FETAL_ERROR("MeshLayout() has been passed incorrect number of arguments, it doesn't accept %" PRIu32 " arguments", attribute->argument_count);
		}
	}
	return U32_MAX;
}

/* [Attribute(...)] handler*/
static void attribute_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__)
{
	/* data[0] is the vertex buffer bind index, data[1] is the vertex attribute index (or so called vertex location) */
	u32 data[2] = { };
	/* parameter index occupation map */
	bool map[2] = { false, false };
	if(attribute->parameter_count == 2)
	{
		for(u32 i = 0; i < attribute->parameter_count; i++)
		{
			if(safe_stru32pncmp(ctx, attribute->parameters[i], "binding") == 0)
			{
				map[i] = true;
				data[0] = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[i]);
			}
			else if(safe_stru32pncmp(ctx, attribute->parameters[i], "location") == 0)
			{
				map[i] = true;
				data[1] = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[i]);
			}
			else
			{
				if(!map[i])
					DEBUG_LOG_FETAL_ERROR("Attribute() is incorrectly called, argument \"%.*s\" can't find its place", U32_PAIR_DIFF(attribute->arguments[i]), u32_pair_get_str(ctx, attribute->arguments[i]));
				data[i] = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[i]);
			}
		}
	}
	else if(attribute->parameter_count == 1)
	{
		/* find index in { "position", "normal", etc. }, i.e. g_vertex_attribute_bind_names */
		u32 bind_name_index = strs_find_u32_pair(ctx, g_vertex_attribute_bind_names, SIZEOF_ARRAY(g_vertex_attribute_bind_names), attribute->arguments[0]);
		if(bind_name_index != U32_MAX)
		{
			u32 mesh_layout_index = find_mesh_layout_index(ctx, node->attributes, node->attribute_count);
			if(mesh_layout_index != U32_MAX)
			{
				data[0] = g_mesh_layout_emit_values[mesh_layout_index][bind_name_index][0];
				data[1] = g_mesh_layout_emit_values[mesh_layout_index][bind_name_index][1];
			}
		}
		else DEBUG_LOG_FETAL_ERROR("Attribute() is passed incorrect argument \"%.%s\", no such vertex attribute bind name exists", u32_pair_get_str(ctx, attribute->arguments[0]), U32_PAIR_DIFF(attribute->arguments[0]));
	}
	else DEBUG_LOG_FETAL_ERROR("Attribute() is passed incorrect number of arguments, it doesn't accept %" PRIu32 " arguments", attribute->parameter_count);

	sb_emitter_emit_vertex_bind_and_location(ctx->emitter, data[0], data[1]);
}

static const char* g_vertex_input_rates[] = 
{
	"per_vertex",
	"per_instance"
};

static vertex_input_rate_t g_vertex_input_rate_emit_values[] = 
{
	VERTEX_INPUT_RATE_VERTEX,
	VERTEX_INPUT_RATE_INSTANCE
};

/* [Rate(...)] attribute handler */
static void rate_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__)
{
	if(attribute->argument_count == 1)
	{
		u32 index = strs_find_u32_pair(ctx, g_vertex_input_rates, SIZEOF_ARRAY(g_vertex_input_rates), attribute->arguments[0]);
		if(index != U32_MAX)
			sb_emitter_emit_vertex_input_rate(ctx->emitter, g_vertex_input_rate_emit_values[index]);
		else DEBUG_LOG_FETAL_ERROR("Rate() is passed incorrect argument \"%.*s\", no such input rate exists", U32_PAIR_DIFF(attribute->arguments[0]), u32_pair_get_str(ctx, attribute->arguments[0]));
	}
	else DEBUG_LOG_FETAL_ERROR("Rate() is passed incorrect number of arguments, it doesn't accept %" PRIu32 " arguments", attribute->parameter_count);
}

/* [MeshLayout(...)] attribute handler */
static void mesh_layout_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__) { }

static const char* g_vertex_attribute_names[] =
{
	"Rate",
	"MeshLayout",
	"Attribute"
};

static attribute_callback_handler_t g_vertex_attribute_handlers[] = 
{
	rate_attribute_handler,
	mesh_layout_attribute_handler,
	attribute_attribute_handler
};

#define NON_OPAQUE_TYPE_STRS	\
	"vec2", \
	"vec3", \
	"vec4", \
	"ivec2", \
	"ivec3", \
	"ivec4", \
	"uvec2", \
	"uvec3", \
	"uvec4", \
	"dvec2", \
	"dvec3", \
	"dvec4", \
	"float", \
	"double", \
	"int", \
	"uint", \
	"mat2", \
	"mat3", \
	"mat4", \
	"dmat2", \
	"dmat3", \
	"dmat4"

static const char* g_vertex_attribute_types[] = 
{
	NON_OPAQUE_TYPE_STRS
};

#define NON_OPAQUE_TYPE_EMIT_VALUES \
	GLSL_TYPE_VEC2, \
	GLSL_TYPE_VEC3, \
	GLSL_TYPE_VEC4, \
	GLSL_TYPE_IVEC2, \
	GLSL_TYPE_IVEC3, \
	GLSL_TYPE_IVEC4, \
	GLSL_TYPE_UVEC2, \
	GLSL_TYPE_UVEC3, \
	GLSL_TYPE_UVEC4, \
	GLSL_TYPE_DVEC2, \
	GLSL_TYPE_DVEC3, \
	GLSL_TYPE_DVEC4, \
	GLSL_TYPE_FLOAT, \
	GLSL_TYPE_DOUBLE, \
	GLSL_TYPE_INT, \
	GLSL_TYPE_UINT, \
	GLSL_TYPE_MAT2, \
	GLSL_TYPE_MAT3, \
	GLSL_TYPE_MAT4, \
	GLSL_TYPE_DMAT2, \
	GLSL_TYPE_DMAT3, \
	GLSL_TYPE_DMAT4

static glsl_type_t g_vertex_attribute_type_emit_values[] = 
{
	NON_OPAQUE_TYPE_EMIT_VALUES
};

static u32 node_call_attribute_handlers(v3d_generic_node_t* node, compiler_ctx_t* ctx, const char* const* strs, u32 str_count, attribute_callback_handler_t* const handlers, void* user_data, const u32* const requirements, u32 requirement_count)
{
	bool is_found_mask[str_count];

	/* initialy no attribute is found */
	for(u32 i = 0; i < str_count; i++)
		is_found_mask[i] = false;

	/* iterate through each attribute applied to this vertex attribute definition */
	for(u32 j = 0; j < node->attribute_count; j++)
	{
		AUTO attribute = &node->attributes[j];
		u32 index = strs_find_u32_pair(ctx, strs, str_count, attribute->name);
		if(index != U32_MAX)
		{
			/* if found then invoke the handler at the corresponding index */
			is_found_mask[index] = true;
			AUTO fnptr = handlers[index];
			fnptr(ctx, attribute, node, user_data);
		}
	}

	/* check if all the required attribute has been found */
	for(u32 i = 0; i < requirement_count; i++)
		if(!is_found_mask[requirements[i]])
			/* requirement at i isn't satified */
			return i;

	return U32_MAX;
}

static u32_pair_t node_get_last_qualifier(v3d_generic_node_t* node)
{
	if(node->qualifier_count > 0)
		return node->qualifiers[node->qualifier_count - 1];
	return (u32_pair_t) { 0u, 0u };
}

static void write_vertex_buffer_layout(v3d_generic_node_t** nodes, u32 node_count, compiler_ctx_t* ctx, void* user_data)
{
	sb_emitter_open_vertex_attribute_array(ctx->emitter);
	/*
		** NODE 0 **
		[Rate(per_vertex)]
		[MeshLayout(sge_optimal)]
		[Attribute(position)]
		// [Attribute(binding=1, location=1)]
		vec3 position;
		
		** NODE 1 **
		[Rate(per_vertex)] 
		[MeshLayout(sge_optional)]
		[Attribute(normal)]
		// [Attribute(binding=2, location=2)]
		vec3 normal;
		
		** NODE 2 **
		[Rate(per_vertex)]
		[MeshLayout(sge_optimal)]
		[Attribute(texcoord)]
		// [Attribute(binding=3, location=3)]
		vec2 texcoord;
	*/

	for(u32 i = 0; i < node_count; i++)
	{
		AUTO node = nodes[i];

		/* index in { "vec2", "vec3", "vec4", ... } -- vertex attribute types */
		u32_pair_t pair = u32_pairs_find_any_str(ctx, node->qualifiers, node->qualifier_count, g_vertex_attribute_types, SIZEOF_ARRAY(g_vertex_attribute_types));
		if((pair.start == U32_MAX) || (pair.end == U32_MAX))
			DEBUG_LOG_FETAL_ERROR("Vertex Attribute type is incorrect");
		
		sb_emitter_open_vertex_attribute(ctx->emitter);
		sb_emitter_emit_vertex_attribute_type(ctx->emitter, g_vertex_attribute_type_emit_values[pair.end]);
		AUTO last = node_get_last_qualifier(node);
		sb_emitter_emit_vertex_attribute_name(ctx->emitter, u32_pair_get_str(ctx, last), U32_PAIR_DIFF(last));

		u32 required_attributes[] = 
		{ 
			/* Rate is required (at index 0 in g_vertex_attribute_names) */
		 	0, 
		 	/* Attribute is required (at index 2 in g_vertex_attribute_names) */
			2 
		};
		u32 result = node_call_attribute_handlers(node, ctx, g_vertex_attribute_names, SIZEOF_ARRAY(g_vertex_attribute_names), g_vertex_attribute_handlers, NULL, required_attributes, SIZEOF_ARRAY(required_attributes));
		if(result != U32_MAX)
			DEBUG_LOG_FETAL_ERROR("Attribute \"%s\" is required to correctly define a vertex attribute's format, binding and location", g_vertex_attribute_names[required_attributes[result]]);

		sb_emitter_close_vertex_attribute(ctx->emitter);
	}

	sb_emitter_close_vertex_attribute_array(ctx->emitter);
}

static void codegen_vertex_buffer_layout(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	/* if unparsed content is empty, then assume SL2023 variant of Layout block */
	if(node->unparsed.start == node->unparsed.end)
		write_vertex_buffer_layout(node->childs, node->child_count, ctx, user_data);
	/* otherwise, try with SL2022 variant of Layout block */
	else
	{
		debug_log_warning("SL2022 Layout block has been used in SL2023 version, trying SL2022 semantics...");
		codegen_descriptions(node, ctx, iteration, user_data);
	}
}

static const char* g_shader_stages[] =
{
	"vertex",
	"tessellation",
	"geometry",
	"fragment"
};

static shader_stage_bits_t g_shader_stage_emit_values[SIZEOF_ARRAY(g_shader_stages)] =
{
	SHADER_STAGE_BIT_VERTEX,
	SHADER_STAGE_BIT_TESSELLATION,
	SHADER_STAGE_BIT_GEOMETRY,
	SHADER_STAGE_BIT_FRAGMENT,
};

/* callback, index --> the matched index in strs */
static bool u32_pairs_do_if_find_any_str(compiler_ctx_t* ctx, u32_pair_t* pairs, u32 pair_count, const char* const* strs, u32 str_count, void (*callback)(u32 index, void* user_data), void* user_data)
{
	bool is_any_found = false;
	for(u32 i = 0; i < pair_count; i++)
	{
		u32 index = strs_find_u32_pair(ctx, strs, str_count, pairs[i]);
		if(index != U32_MAX)
		{
			callback(index, user_data);
			is_any_found = true;
		}
	}
	return is_any_found;
}

static void accumulate_stage_bits(u32 index, void* user_data)
{
	_com_assert(index < SIZEOF_ARRAY(g_shader_stage_emit_values));
	DREF_TO(u32, user_data) |= g_shader_stage_emit_values[index];
}

static void stage_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__)
{
	u32 stage_bits = 0;
	/* index in { "vertex", "tessellation", "geometry", ... } -- shader stages */
	bool result = u32_pairs_do_if_find_any_str(ctx, attribute->arguments, attribute->argument_count, g_shader_stages, SIZEOF_ARRAY(g_shader_stages), accumulate_stage_bits, &stage_bits);
	if(!result)
	{
		AUTO last = node_get_last_qualifier(node);
		DEBUG_LOG_FETAL_ERROR("No shader stage is specified for the shader property \"%.*s\"", u32_pair_get_str(ctx, last), U32_PAIR_DIFF(last));
	}

	sb_emitter_emit_shader_property_stage(ctx->emitter, stage_bits);
}

static u32 attribute_foreach_arg_parse_to_u32(v3d_generic_attribute_t* attribute, compiler_ctx_t* ctx, const char* const* ordered_params, u32 ordered_param_count, u32* const outputs, u32 output_count)
{
	/* parameter index occupation map */
	bool map[ordered_param_count];
	for(u32 i = 0; i < ordered_param_count; i++)
		map[i] = false;

	AUTO param_count = min(attribute->parameter_count, ordered_param_count);
	for(u32 i = 0; i < param_count; i++)
	{
		AUTO u32_param = attribute->parameters[i];
		if(u32_param.start != u32_param.end)
		{
			u32 index = strs_find_u32_pair(ctx, ordered_params, ordered_param_count, u32_param);
			if(index != U32_MAX)
			{
				map[index] = true;
				outputs[index] = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[i]);
				continue;
			}
		}
		if(map[i])
			return i;
		outputs[i] = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[i]);
	}

	return U32_MAX;
}

static const char* g_set_attribute_params[] = 
{
	"set",
	"binding"
};

static const char* g_shader_property_bind_names[] = 
{
	"material_properties",
	"texture0",
	"texture1",
	"texture2",
	"texture3",
	"texture4",
	"texture5",
	"texture6",
	"texture7",
	"texture8"
};

static u32 g_shader_property_bind_name_emit_values[][SIZEOF_ARRAY(g_set_attribute_params)] = 
{
	{ SGE_SHADER_PROPERTY_MATERIAL_PROPERTIES_SET, SGE_SHADER_PROPERTY_MATERIAL_PROPERTIES_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE0_SET, SGE_SHADER_PROPERTY_TEXTURE0_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE1_SET, SGE_SHADER_PROPERTY_TEXTURE1_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE2_SET, SGE_SHADER_PROPERTY_TEXTURE2_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE3_SET, SGE_SHADER_PROPERTY_TEXTURE3_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE4_SET, SGE_SHADER_PROPERTY_TEXTURE4_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE5_SET, SGE_SHADER_PROPERTY_TEXTURE5_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE6_SET, SGE_SHADER_PROPERTY_TEXTURE6_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE7_SET, SGE_SHADER_PROPERTY_TEXTURE7_BINDING },
	{ SGE_SHADER_PROPERTY_TEXTURE8_SET, SGE_SHADER_PROPERTY_TEXTURE8_BINDING }
};

static void set_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__)
{
	/* output[0] is the shader property's set number, output[1] is the shader property's binding number */
	u32 output[2] = { };

	if(attribute->parameter_count == 2)
	{
		u32 result = attribute_foreach_arg_parse_to_u32(attribute, ctx, g_set_attribute_params, SIZEOF_ARRAY(g_set_attribute_params), output, SIZEOF_ARRAY(output));
		if(result != U32_MAX)
			DEBUG_LOG_FETAL_ERROR("Set() is incorrectly called, argument \"%.*s\" can't find its place", U32_PAIR_DIFF(attribute->arguments[result]), u32_pair_get_str(ctx, attribute->arguments[result]));
	}
	else if(attribute->parameter_count == 1)
	{
		/* find index in { "material_properties", "texture0", etc. }, i.e. g_shader_property_bind_names */
		u32 bind_name_index = strs_find_u32_pair(ctx, g_shader_property_bind_names, SIZEOF_ARRAY(g_shader_property_bind_names), attribute->arguments[0]);
		if(bind_name_index != U32_MAX)
		{
			output[0] = g_shader_property_bind_name_emit_values[bind_name_index][0];
			output[1] = g_shader_property_bind_name_emit_values[bind_name_index][1];
		}
		else DEBUG_LOG_FETAL_ERROR("Set() is passed incorrect argument \"%.%s\", no such shader property bind name exists", u32_pair_get_str(ctx, attribute->arguments[0]), U32_PAIR_DIFF(attribute->arguments[0]));
	}
	else DEBUG_LOG_FETAL_ERROR("Set() is passed incorrect number of arguments, it doesn't accept %" PRIu32 " arguments", attribute->parameter_count);

	sb_emitter_emit_shader_property_set_and_binding(ctx->emitter, output[0], output[1]);
}

static void pushconst_attribute_handler(__ATTRIBUTE_HANDLER_PARAMS__)
{
	/* offset in the push constant buffer */
	u32 offset = 0;
	if((attribute->parameter_count == 1) || (attribute->parameter_count == 0))
	{
		if(attribute->parameter_count == 1)
			offset = try_parse_u32_pair_str_to_u32(ctx, attribute->arguments[0]);
		else /* by default, the offset will be zero */
			offset = 0;
	}
	else DEBUG_LOG_FETAL_ERROR("PushConst is passed extra number of arguments, it doesn't accept %" PRIu32 " arguments", attribute->parameter_count);

	sb_emitter_emit_shader_property_type(ctx->emitter, GLSL_TYPE_PUSH_CONSTANT);
	sb_emitter_emit_shader_property_set_push_constant_offset(ctx->emitter, offset);
}

static const char* g_shader_property_attributes[] = 
{
	"Stage",
	"Set",
	"PushConst"
};

static attribute_callback_handler_t g_shader_property_attribute_handlers[SIZEOF_ARRAY(g_shader_property_attributes)] = 
{
	stage_attribute_handler,
	set_attribute_handler,
	pushconst_attribute_handler
};

static const char* g_shader_property_storage_class_types[] = 
{
	"uniform",
	"buffer"
};

static storage_class_t g_shader_property_storage_class_emit_values[SIZEOF_ARRAY(g_shader_property_storage_class_types)] = 
{
	STORAGE_CLASS_UNIFORM,
	STORAGE_CLASS_BUFFER
};

static const char* g_shader_property_opaque_types[] = 
{
	"sampler2D",
	"sampler3D",
	"samplerCube"
};

static glsl_type_t g_shader_property_opaque_type_emit_values[SIZEOF_ARRAY(g_shader_property_opaque_types)] =
{
	GLSL_TYPE_SAMPLER_2D,
	GLSL_TYPE_SAMPLER_3D,
	GLSL_TYPE_SAMPLER_CUBE
};

static bool foreach_child_node(v3d_generic_node_t* node, void (*visitor)(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data), void* user_data, compiler_ctx_t* ctx)
{
	u32 count = 0;
	for(u32 i = 0; i < node->child_count; i++)
	{
		count++;
		visitor(node->childs[i], ctx, count, user_data);
	}
	return count != 0;
}

static const char* g_shader_property_field_types[] =
{
	NON_OPAQUE_TYPE_STRS
};

static glsl_type_t g_shader_property_field_type_emit_values[] = 
{
	NON_OPAQUE_TYPE_EMIT_VALUES
};

static void emit_shader_property_field(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 count, void* user_data)
{
	/* index in { "vec2", "vec3", "vec4", ... } -- shader property field types */
	u32_pair_t pair = u32_pairs_find_any_str(ctx, node->qualifiers, node->qualifier_count, g_shader_property_field_types, SIZEOF_ARRAY(g_shader_property_field_types));
	if((pair.start == U32_MAX) || (pair.end == U32_MAX))
		DEBUG_LOG_FETAL_ERROR("Shader Property field type is incorrect");
		
	sb_emitter_open_shader_property_field(ctx->emitter);
	sb_emitter_emit_shader_property_field_type(ctx->emitter, g_shader_property_field_type_emit_values[pair.end]);
	AUTO identifier_name = node_get_last_qualifier(node);
	sb_emitter_emit_shader_property_field_name(ctx->emitter, u32_pair_get_str(ctx, identifier_name), U32_PAIR_DIFF(identifier_name));
	if(node->indexer_count > 0)
		sb_emitter_emit_shader_property_field_array_size(ctx->emitter, try_parse_u32_pair_str_to_u32(ctx, node->indexers[0]));
	sb_emitter_close_shader_property_field(ctx->emitter);
}

static bool node_is_name_only(v3d_generic_node_t* node)
{
	if(node->qualifier_count != 1)
		return false;
	if(node->attribute_count != 0)
		return false;
	return true;
}

static void write_buffer_layouts_and_samplers(v3d_generic_node_t** nodes, u32 node_count, compiler_ctx_t* ctx, void* user_data)
{
	sb_emitter_open_shader_property_array(ctx->emitter);
	/*
		Properties
		{
			** NODE 0 **
			[Stage(fragment, vertex)]
			[Set(set = material, binding = material_properties)]
			uniform Parameters
			{
				vec4 color;
			} parameters;

			** NODE 1 **
			[Stage(fragment, vertex)]
			[Set(set = material, binding = texture0)]
			uniform sampler2D albedo; 
		}
	*/
	for(u32 i = 0; i < node_count; i++)
	{
		AUTO node = nodes[i];

		sb_emitter_open_shader_property(ctx->emitter);

		/* index in { "uniform", "buffer" } -- storage class types */
		u32_pair_t pair = u32_pairs_find_any_str(ctx, node->qualifiers, node->qualifier_count, g_shader_property_storage_class_types, SIZEOF_ARRAY(g_shader_property_storage_class_types));
		if((pair.start == U32_MAX) || (pair.end == U32_MAX))
			DEBUG_LOG_FETAL_ERROR("Storage Class is incorrect");
		AUTO storage_class = g_shader_property_storage_class_emit_values[pair.end];
		sb_emitter_emit_shader_property_storage_class(ctx->emitter, storage_class);

		if(storage_class == STORAGE_CLASS_UNIFORM)
		{
			/* index in { "sampler2D", "sampler3D", etc. } -- opaque types */
			u32_pair_t pair2 = u32_pairs_find_any_str(ctx, node->qualifiers + pair.start + 1u, node->qualifier_count - pair.start - 1u, g_shader_property_opaque_types, SIZEOF_ARRAY(g_shader_property_opaque_types));
			if((pair2.start == U32_MAX) || (pair2.end == U32_MAX))
			{
				/* if this shader property doesn't have any opaque types nor it is a uniform block */
				if(!node->is_block)
					DEBUG_LOG_FETAL_ERROR("An Opaque type is expected such as sampler2D, sampler3D etc; but no expected opaque type is found");
				else
					/* here, there are two possibilities, it is either a push constant buffer or a uniform buffer, but let's set it to uniform buffer
					 * and then later we will overwrite it as push constant if [PushConst] attribute is found */
					sb_emitter_emit_shader_property_type(ctx->emitter, GLSL_TYPE_UNIFORM_BUFFER);
			}
			else /* if this shader property is an opaque type */
			{
				if(node->is_block)
					DEBUG_LOG_FETAL_ERROR("Opaque types can't be defined as blocks");
				AUTO opaque_type = g_shader_property_opaque_type_emit_values[pair2.end];
				sb_emitter_emit_shader_property_type(ctx->emitter, opaque_type);
			}
		}
		else 
		{
			_com_assert(storage_class == STORAGE_CLASS_BUFFER);
			if(!node->is_block)
				DEBUG_LOG_FETAL_ERROR("Shader Property has been specified storage class as \"buffer\", but is not a block");
			sb_emitter_emit_shader_property_type(ctx->emitter, GLSL_TYPE_STORAGE_BUFFER);
		}

		AUTO shr_property_name = node_get_last_qualifier(node);
		if(node->is_block)
		{
			/* if no block name is specified */
			if(node->qualifier_count == 1)
				/* then only add a null character as a block name */
				sb_emitter_emit_shader_property_block_name(ctx->emitter, "", 0u);
			else
				sb_emitter_emit_shader_property_block_name(ctx->emitter, u32_pair_get_str(ctx, shr_property_name), U32_PAIR_DIFF(shr_property_name));
			/* if this block has an identifier name (the next node will represent the identifier name) */
			if(((i + 1) < node_count) && node_is_name_only(nodes[i + 1]))
			{
				shr_property_name = nodes[i + 1]->qualifiers[0];
				sb_emitter_emit_shader_property_name(ctx->emitter, u32_pair_get_str(ctx, shr_property_name), U32_PAIR_DIFF(shr_property_name));
				++i;
			}
			else
				DEBUG_LOG_FETAL_ERROR("Shader Property doesn't have any identifier name);");
		}
		/* if this shader property, which is not a block, has identifier name */
		else if(node->qualifier_count != 1)
			/* then add it */
			sb_emitter_emit_shader_property_name(ctx->emitter, u32_pair_get_str(ctx, shr_property_name), U32_PAIR_DIFF(shr_property_name));
		else /* otherwise, throw an error */
			DEBUG_LOG_FETAL_ERROR("Shader Property doesn't have any identifier name");

		if(node->child_count > 0)
		{
			_com_assert(node->is_block);
			sb_emitter_open_shader_property_field_array(ctx->emitter);
			if(!foreach_child_node(node, emit_shader_property_field, NULL, ctx))
				DEBUG_LOG_WARNING("An interface block has no elements in it");
			sb_emitter_close_shader_property_field_array(ctx->emitter);
		}

		u32 required_attributes[] = 
		{ 
			/* Stage is required (at index 0 in g_shader_property_attributes) */
		 	0
		};
		u32 result = node_call_attribute_handlers(node, ctx, g_shader_property_attributes, SIZEOF_ARRAY(g_shader_property_attributes), g_shader_property_attribute_handlers, NULL, required_attributes, SIZEOF_ARRAY(required_attributes));
		if(result != U32_MAX)
			DEBUG_LOG_FETAL_ERROR("Attribute \"%s\" is required to correctly define a shader property", g_shader_property_attributes[required_attributes[result]]);

		sb_emitter_close_shader_property(ctx->emitter);
	}

	sb_emitter_close_shader_property_array(ctx->emitter);
}

static void codegen_buffer_layouts_and_samplers(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	/* if unparsed content is empty, then assume SL2023 variant of Properties block */
	if(node->unparsed.start == node->unparsed.end)
		write_buffer_layouts_and_samplers(node->childs, node->child_count, ctx, user_data);
	/* otherwise, try with SL2022 variant of Properties block */
	else
	{
		debug_log_warning("SL2022 Properties block has been used in SL2023 version, trying SL2022 semantics...");
		codegen_descriptions(node, ctx, iteration, user_data);
	}
}

static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	write_layout(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, ctx->codegen_buffer, true);
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

/* 	parses the string pointed by 'str' of size 'length' (sized extension to strtoul) and returns numerical value
	str: ptr to the string
	len: length of the string pointed by 'str'
	returns the numerical value of 'str'
 */
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

CAN_BE_UNUSED_FUNCTION static s32 signed_sub(u32 op1, u32 op2)
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
	AUTO result = ppsr_v3d_generic_parse(&ctx->callbacks, unparsed.start + ctx->src, U32_PAIR_DIFF(unparsed));
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
	BUFFER color_reads = buf_new_with_callbacks(&ctx->callbacks, u32);
	/* list of binding infos, i.e [Read(color = ..., set = <set_number>, binding = <binding_number>)] attribute on a subpass */
	BUFFER color_binds = buf_new_with_callbacks(&ctx->callbacks, bind_info_t);
	/* list of color writes, i.e. [Write(color = ...)] attribute on a subpass */
	BUFFER color_writes = buf_new_with_callbacks(&ctx->callbacks, u32);
	/* list of color locations, i.e. [Write(color = ..., location = ...)] attribute on a subpass */
	BUFFER color_locations = buf_new_with_callbacks(&ctx->callbacks, color_location_t);

	/* now analyse the attributes on the subpass */

	/* for each attribute on this subpass */
	CAN_BE_UNUSED_VARIABLE s32 color_input_count = 0;
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
	ppsr_v3d_generic_parse_result_destroy(&ctx->callbacks, result);
	debug_log_info("[Subpass analysis] end");
}

/* reoders the attachments of a renderpass to put the depth stencil attachment at last */
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

/* reoders the attachments of a renderpass to put the presentable color attachment at first */
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

	CAN_BE_UNUSED_VARIABLE u32 color_attachment = renderpass->attachments[present_index];
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
	BUFFER subpasses = buf_new_with_callbacks(&ctx->callbacks, subpass_analysis_t);

	/* list of unique attachments that has to be created after loading the shader  */
	BUFFER attachments = buf_new_with_callbacks(&ctx->callbacks, attachment_type_t);
	
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

/* immediate version of 'write_layout' function */
static void write_layout2(compiler_ctx_t* ctx, const char* format, ...)
{
	buffer_t string_buffer = buf_create_with_callbacks(&ctx->callbacks, sizeof(char), 64, 0);
	va_list args;
	va_start(args, format);
 	buf_vprintf(&string_buffer, NULL, format, args);
 	va_end(args);
	write_layout(buf_get_ptr(&string_buffer), buf_peek_ptr(&string_buffer), ctx->codegen_buffer, false);
	buf_free(&string_buffer);
}

static void codegen_render_set_binding_descriptions(render_pass_analysis_t* analysis, compiler_ctx_t* ctx, u32 index)
{
	/* write the number of input descriptions */
	binary_writer_u16(ctx->codegen_buffer->main, analysis->color_bind_count + ((analysis->depth_read == U32_MAX) ? 0 : 1));

	/* write the color input descriptions */
	for(u32 i = 0; i < analysis->color_bind_count; i++)
	{
		write_layout2(ctx, 
					"fragment [%lu, %lu] uniform sampler2D rp%lucolor%lu;",
					analysis->color_binds[i].set_number,
					analysis->color_binds[i].binding_number,
					index,
					CAST_TO(u32, i));
	}

	/* write the depth input descriptions */
	if(analysis->depth_read != U32_MAX)
		write_layout2(ctx, 
					"fragment [%lu, %lu] uniform sampler2D rp%ludepth;",
					analysis->depth_bind.set_number,
					analysis->depth_bind.binding_number,
					index);
}

static bool has_color_attachment(render_pass_analysis_t* analysis)
{
	for(u32 i = 0; i < analysis->attachment_count; i++)
		if(analysis->attachments[i] == ATTACHMENT_TYPE_COLOR)
			return true;
	return false;
}

static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	render_pass_user_data_t* data = CAST_TO(render_pass_user_data_t*, user_data);
	render_pass_analysis_t* analysis = com_allocate_obj_init(&ctx->callbacks, render_pass_analysis_t);
	run_render_pass_analysis(node, ctx, data->prev_analysis, analysis);
	if(data->prev_analysis != NULL)
	{
		render_pass_analysis_destroy(&ctx->callbacks, data->prev_analysis);
		com_deallocate(&ctx->callbacks, data->prev_analysis);
	}
	/* we will be destroying this render pass analysis if this is the last one at the end of this function, therefore set the previous analysis
	 * to NULL to avoid other code using it further or at least easily detect the use of NULL pointer, if any.  */
	data->prev_analysis = (iteration == data->render_pass_count) ? NULL : analysis;

	_ASSERT(analysis->attachment_count != 0);

	/* write the type of the render pass, if this is the last pass then it must be type of SWAPCHAIN_TARGET */
	binary_writer_u32(ctx->codegen_buffer->main, ((iteration == data->render_pass_count) && has_color_attachment(analysis)) ? RENDER_PASS_TYPE_SWAPCHAIN_TARGET : RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);

	/* write the list of input attachments */
	u32 input_count = analysis->color_read_count + ((analysis->depth_read == U32_MAX) ? 0 : 1);
	binary_writer_u32(ctx->codegen_buffer->main, input_count);
	binary_writer_write(ctx->codegen_buffer->main, (void*)analysis->color_reads, analysis->color_read_count * sizeof(u32));
	binary_writer_write(ctx->codegen_buffer->main, &analysis->depth_read, (input_count - analysis->color_read_count) * sizeof(u32));

	/* write the list of attachments */
	binary_writer_u32(ctx->codegen_buffer->main, analysis->attachment_count);
	binary_writer_write(ctx->codegen_buffer->main, (void*)analysis->attachments, analysis->attachment_count * sizeof(u32));

	/* let's keep the subpass dependencies out of the picture for now */
	binary_writer_u32(ctx->codegen_buffer->main, 0);

	/* write the render set binding descriptions */
	codegen_render_set_binding_descriptions(analysis, ctx, iteration);

	/* write the list of subpass descriptions */
	binary_writer_u32(ctx->codegen_buffer->main, analysis->subpass_count);

	for(u32 i = 0; i < analysis->subpass_count; i++)
		codegen_subpass(node->childs[i], &analysis->subpasses[i], ctx, iteration, i);

	/* destroy and deallocate this render pass analysis object if this is the last one */
	if(iteration == data->render_pass_count)
	{
		render_pass_analysis_destroy(&ctx->callbacks, analysis);
		com_deallocate(&ctx->callbacks, analysis);
	}
}

static void codegen_sub_render_set_binding_descriptions(subpass_analysis_t* analysis, compiler_ctx_t* ctx, u32 rpindex, u32 spindex)
{
	/* write the number of input descriptions */
	binary_writer_u16(ctx->codegen_buffer->main, analysis->color_bind_count + ((analysis->depth_read == U32_MAX) ? 0 : 1));

	/* write the color input descriptions */
	for(u32 i = 0; i < analysis->color_bind_count; i++)
	{
		write_layout2(ctx, 
					"fragment [%lu, %lu] uniform subpassInput sb%lurp%lucolor%lu;",
					analysis->color_binds[i].set_number,
					analysis->color_binds[i].binding_number,
					rpindex,
					spindex,
					CAST_TO(u32, i));
	}

	/* write the depth input descriptions */
	if(analysis->depth_read != U32_MAX)
		write_layout2(ctx, 
					"fragment [%lu, %lu] uniform subpassInput sb%lurp%ludepth;",
					analysis->depth_bind.set_number,
					analysis->depth_bind.binding_number,
					rpindex,
					spindex);
}

static void codegen_subpass(v3d_generic_node_t* node, subpass_analysis_t* analysis, compiler_ctx_t* ctx, u32 rpindex, u32 spindex)
{
	if(ctx->sb_version == SB_VERSION_2023)
	{
		/* vertex buffer layout descriptions  */
		if(!foreach_child_node_if_name(node, keywords[KEYWORD_LAYOUT], (ctx->sl_version == SL_VERSION_2023) ? codegen_vertex_buffer_layout : codegen_descriptions, NULL, ctx))
		{
			/* if there is no layout block then just write the number of layout descriptions to be zero */
			binary_writer_u16(ctx->codegen_buffer->main, 0);
		}
	}

	/* write input attachment list */
	u32 input_count = analysis->color_read_count + ((analysis->depth_read != U32_MAX) ? 1 : 0);
	binary_writer_u32(ctx->codegen_buffer->main, input_count);
	binary_writer_write(ctx->codegen_buffer->main, (void*)analysis->color_reads, analysis->color_read_count * sizeof(u32));
	binary_writer_write(ctx->codegen_buffer->main, &analysis->depth_read, (input_count - analysis->color_read_count) * sizeof(u32));

	/* write color attachment list */
	binary_writer_u32(ctx->codegen_buffer->main, analysis->color_write_count);
	binary_writer_write(ctx->codegen_buffer->main, (void*)analysis->color_writes, analysis->color_write_count * sizeof(u32));

	/* let's keep the preserve attachment list zero for now */
	binary_writer_u32(ctx->codegen_buffer->main, 0);

	/* write the depth attachment index */
	binary_writer_u32(ctx->codegen_buffer->main, analysis->depth_write);

	/* write the pipeline offset */
	ctx->current_pipeline_index++;
	_ASSERT((MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index) < MARK_ID_PIPELINE_OFFSET_MAX);
	binary_writer_u32_mark(ctx->codegen_buffer->main, MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index);
	binary_writer_u32_set(ctx->codegen_buffer->main, MARK_ID_PIPELINE_OFFSET + ctx->current_pipeline_index, binary_writer_pos(ctx->codegen_buffer->data));

	/* WRITE PIPELINE DESCRIPTION INTO THE DATA SECTION */

	/* write the vulkan_graphics_pipeline_description_t object into the data section */
	codegen_pipeline(find_child_node_if_name(node, keywords[KEYWORD_GFXPIPELINE], ctx->src), ctx, 0, NULL);

	/* write the spir-v binaries into the data section */
	codegen_glsl(find_child_node_if_name(node, keywords[KEYWORD_GLSL], ctx->src), ctx, 0, NULL);

	/* write the sub render set binding descriptions */
	codegen_sub_render_set_binding_descriptions(analysis, ctx, rpindex, spindex);
}

static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	_ASSERT(node != NULL);
	write_gfx_pipeline(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, ctx->codegen_buffer);
}

static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, u32 iteration, void* user_data)
{
	_ASSERT(node != NULL);
	write_glsl(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, ctx);
}

SC_API void codegen(v3d_generic_node_t* node, compiler_ctx_t* ctx)
{
	codegen_shader(node, ctx);
}
