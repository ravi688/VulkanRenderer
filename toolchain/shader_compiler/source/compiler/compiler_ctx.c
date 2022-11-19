#include <shader_compiler/compiler/compiler_ctx.h>
#include <shader_compiler/compiler/grammar.h>

#include <stdlib.h>
#include <stdarg.h>

typedef ptr_u32_pair_t list_t;

static list_t new_u32_list(u32 count, ...)
{
	u32* ptr = CAST_TO(u32*, malloc(count * sizeof(u32)));
	va_list args;
	va_start(args, count);
	for(u32 i = 0; i < count; i++)
		ptr[i] = va_arg(args, u32);
	return (list_t) { ptr, count };
}

SC_API compiler_ctx_t* compiler_ctx_create(const char* src)
{
	compiler_ctx_t* ctx = CAST_TO(compiler_ctx_t*, malloc(sizeof(compiler_ctx_t)));

	/* prepare the look ahead table */
	ctx->lat_size = KEYWORD_MAX;
	ctx->lat = CAST_TO(ptr_u32_pair_t*, malloc(sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->lat[KEYWORD_UNDEFINED] = new_u32_list(1, KEYWORD_SHADER);
	ctx->lat[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->lat[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->lat[KEYWORD_SHADER] = new_u32_list(3, KEYWORD_PROPERTIES, KEYWORD_LAYOUT, KEYWORD_RENDERPASS);
	ctx->lat[KEYWORD_PROPERTIES] = PTR_U32_NULL;
	ctx->lat[KEYWORD_LAYOUT] = PTR_U32_NULL;
	ctx->lat[KEYWORD_RENDERPASS] = new_u32_list(1, KEYWORD_SUBPASS);
	ctx->lat[KEYWORD_SUBPASS] = new_u32_list(2, KEYWORD_GFXPIPELINE, KEYWORD_GLSL);
	ctx->lat[KEYWORD_GLSL] = PTR_U32_NULL;
	ctx->lat[KEYWORD_GFXPIPELINE] = PTR_U32_NULL;

	/* prepare the symbol qualifiers table */
	ctx->sqt_size = KEYWORD_MAX;
	ctx->sqt = CAST_TO(ptr_u32_pair_t*, malloc(sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->sqt[KEYWORD_UNDEFINED] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_SHADER] = new_u32_list(1, QUALIFIER_SHADER);
	ctx->sqt[KEYWORD_PROPERTIES] = new_u32_list(1, QUALIFIER_PROPERTIES);
	ctx->sqt[KEYWORD_LAYOUT] = new_u32_list(1, QUALIFIER_LAYOUT);
	ctx->sqt[KEYWORD_RENDERPASS] = new_u32_list(1, QUALIFIER_RENDERPASS);
	ctx->sqt[KEYWORD_SUBPASS] = new_u32_list(1, QUALIFIER_SUBPASS);
	ctx->sqt[KEYWORD_GLSL] = new_u32_list(1, QUALIFIER_GLSL);
	ctx->sqt[KEYWORD_GFXPIPELINE] = new_u32_list(1, QUALIFIER_GFXPIPELINE);

	/* prepare the symbol atributes table */
	ctx->sat_size = KEYWORD_MAX;
	ctx->sat = CAST_TO(ptr_u32_pair_t*, malloc(sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->sat[KEYWORD_UNDEFINED] = PTR_U32_NULL;
	ctx->sat[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->sat[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->sat[KEYWORD_SHADER] = new_u32_list(1, ATTRIBUTE_NAME);
	ctx->sat[KEYWORD_PROPERTIES] = new_u32_list(1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_LAYOUT] = new_u32_list(1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_RENDERPASS] = new_u32_list(1, ATTRIBUTE_INPUT);
	ctx->sat[KEYWORD_SUBPASS] = new_u32_list(1, ATTRIBUTE_INPUT);
	ctx->sat[KEYWORD_GLSL] = new_u32_list(1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_GFXPIPELINE] = PTR_U32_NULL;

	ctx->keywords_size = sizeof(keywords) / sizeof(const char*);
	ctx->keywords = CAST_TO(char**, keywords);

	ctx->depth = KEYWORD_UNDEFINED;

	ctx->src = src;

	return ctx;
}

SC_API void compiler_ctx_destroy(compiler_ctx_t* ctx)
{
	// TODO	
}
