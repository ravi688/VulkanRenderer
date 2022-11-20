#include <shader_compiler/compiler/codegen/codegen.h>
#include <shader_compiler/compiler/codegen/gfx_pipeline.h>
#include <shader_compiler/compiler/codegen/glsl.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/compiler/grammar.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/debug.h>



static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_subpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);

static void codegen_shader(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	for(u32 i = 0; i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		u32_pair_t pair = child->qualifiers[child->qualifier_count - 1];
		if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_PROPERTIES], U32_PAIR_DIFF(pair)) == 0)
			codegen_descriptions(child, ctx, writer);
		else if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_LAYOUT], U32_PAIR_DIFF(pair)) == 0)
			codegen_descriptions(child, ctx, writer);
		else if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_RENDERPASS], U32_PAIR_DIFF(pair)) == 0)
			codegen_renderpass(child, ctx, writer);
		else DEBUG_LOG_ERROR("[Codegen] Unknown Error");
	}
}

static void codegen_descriptions(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	write_layout(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer);
}

static void codegen_renderpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	for(u32 i = 0;i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		u32_pair_t pair = child->qualifiers[child->qualifier_count - 1];
		if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_SUBPASS], U32_PAIR_DIFF(pair)) == 0)
			codegen_subpass(child, ctx, writer);
		else DEBUG_LOG_ERROR("[Codegen] Unkown Error");
	}
}

static void codegen_subpass(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	for(u32 i = 0;i < node->child_count; i++)
	{
		v3d_generic_node_t* child = node->childs[i];
		u32_pair_t pair = child->qualifiers[node->qualifier_count - 1];
		if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_GFXPIPELINE], U32_PAIR_DIFF(pair)) == 0)
			codegen_pipeline(child, ctx, writer);
		else if(safe_strncmp(pair.start + ctx->src, keywords[KEYWORD_GLSL], U32_PAIR_DIFF(pair)) == 0)
			codegen_glsl(child, ctx, writer);
		else DEBUG_LOG_ERROR("[Codegen] Unkown Error");
	}
}

static void codegen_pipeline(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	write_gfx_pipeline(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer);
}

static void codegen_glsl(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	write_glsl(node->unparsed.start + ctx->src, node->unparsed.end + ctx->src, writer, ctx);
}

SC_API void codegen(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer)
{
	codegen_shader(node, ctx, writer);
}