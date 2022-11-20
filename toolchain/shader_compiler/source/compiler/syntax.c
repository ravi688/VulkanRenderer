#include <shader_compiler/compiler/syntax.h>
#include <shader_compiler/compiler/grammar.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>

static bool syntax_check_attribute_args(v3d_generic_attribute_t* attribute, const char* start)
{
	return true;
}

/* checks if any of the attributes applied on an entity matches with any of the supported attributes by that entity */
static bool syntax_check_attributes(v3d_generic_attribute_t* attributes, u32 attribute_count, 
									const u32* supported, u32 supported_count, 
									const char* start, const char* description)
{
	bool is_found = false;
	for(u32 i = 0; i < attribute_count; i++)
	{
		u32 len = U32_PAIR_DIFF(attributes[i].name);
		const char* str = attributes[i].name.start + start;
		for(u32 j = 0; j < supported_count; j++)
			if(safe_strncmp(str, attribute_keywords[supported[j]], len) != 0)
				DEBUG_LOG_WARNING("[Syntax] The attribute \"%.*s\" has no effect on %s", len, str, description);
			else
			{
				is_found = true;
				if(!syntax_check_attribute_args(&attributes[i], start))
					DEBUG_LOG_ERROR("[Syntax] Invalid arguments passed to \"%s\"", attribute_keywords[supported[j]]);
			}
	}
	return is_found;
}

/* checks if any of the qualifiers applied on an entity mathces with any of the supported qualifiers by that entity */
static bool syntax_check_qualifiers(u32_pair_t* qualifiers, u32 qualifier_count, 
									const u32* supported, u32 supported_count,
									const char* start, const char* description, bool iswarn)
{
	bool is_found = false;
	for(u32 i = 0; i < qualifier_count; i++)
	{
		u32 len = U32_PAIR_DIFF(qualifiers[i]);
		const char* str = qualifiers[i].start + start;
		for(u32 j = 0; j < supported_count; j++)
			if(safe_strncmp(str, keywords[supported[j]], len) != 0)
			{
				if(iswarn)
					DEBUG_LOG_WARNING("[Syntax] The qualifier \"%.*s\" has no effect", len, str);
			}
			else
				is_found = true;
	}
	return is_found;
}

static bool syntax_check_node(v3d_generic_node_t* node, const char* start, 
						/* supported qualifiers */ const u32* s_qualifiers, 
						/* supported qualifier count */ u32 s_qualifier_count,
						/* supported attributes */ const u32* s_attrributes,
						/* supported attribute count */ u32 s_attribute_count,
						const char* description, bool is_look_ahead)
{
	if(is_look_ahead)
	{
		return syntax_check_qualifiers(node->qualifiers, node->qualifier_count, 
								s_qualifiers, s_qualifier_count,
								start, NULL, false);
	}
	else
	{
		// optional qualifiers
		syntax_check_qualifiers(node->qualifiers, node->qualifier_count - 1, 
								s_qualifiers, s_qualifier_count - 1,
								start, NULL, true);

		// name (mandatory qualifier)
		if(!syntax_check_qualifiers(node->qualifiers + node->qualifier_count - 1, 1,
								s_qualifiers + s_qualifier_count - 1, 1,
								start, NULL, true))
			return false;

		// optional attributes
		syntax_check_attributes(node->attributes, node->attribute_count, 
							s_attrributes, s_attribute_count,
							start, description);
	}
	return true;
}

SC_API void syntax(v3d_generic_node_t* node, compiler_ctx_t* ctx)
{
	// lets ignore the anonymous blocks
	if(node->qualifier_count == 0)
	{
		DEBUG_LOG_ERROR("anonymous entities (blocks) are not allowed for now");
		return;
	}

	/* -- syntax check for the entity definition -- */

	u32 depth = ctx->depth;
	/* get the list of expected symbols in the current depth (not the block though)*/
	const u32* look_ahead_symbols = CAST_TO(u32*, ctx->lat[ctx->depth].ptr);
	/* get the number of expected symbols */
	u32 symbol_count = ctx->lat[ctx->depth].size;
	
	/* get the identifier name (the last qualifier) */
	u32_pair_t name = node->qualifiers[node->qualifier_count - 1];
	const char* str = name.start + ctx->src;
	u32 len = U32_PAIR_DIFF(name);

	/* check if the current node matches with any of the expected symbols (block names only for now) */
	if(syntax_check_node(node, ctx->src, look_ahead_symbols, symbol_count, NULL, 0, NULL, true))
	{
		for(u32 i = 0; i < symbol_count; i++)
		{
			u32 symbol = look_ahead_symbols[i];
			if(safe_strncmp(str, keywords[symbol], len) == 0)
			{
				bool result = syntax_check_node(node, ctx->src,
							ctx->sqt[symbol].ptr, ctx->sqt[symbol].size,
							ctx->sat[symbol].ptr, ctx->sat[symbol].size,
							keywords[symbol], false);
				_assert(result == true);
				ctx->depth = symbol;
			}
		}
	}
	else if(node->qualifier_count <= 1)
		DEBUG_LOG_ERROR("[Syntax] Unexpected symbol: %.*s", len, str);
	else
		DEBUG_LOG_INFO("[Syntax] Assuming identifier: %.*s", len, str);

	/* -- syntax check for the value of the entity -- */
	
	/* if this node has a value node */
	if(node->has_value)
	{
		// TODO
	}

	for(u32 i = 0; i < node->child_count; i++)
		syntax(node->childs[i], ctx);

	ctx->depth = depth;
}
