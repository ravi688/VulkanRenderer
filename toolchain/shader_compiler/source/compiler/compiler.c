
#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/utilities/string_utilities.h>
#include <shader_compiler/utilities/binary_writer.h>
#include <shader_compiler/utilities/binary_reader.h>
#include <shader_compiler/assert.h>
#include <shader_compiler/shaderc/shaderc.h>

#include <disk_manager/file_reader.h>

#include <glslcommon/glsl_types.h>

#include <phymac_parser/string.h>
#include <phymac_parser/v3d_generic.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h> 		// debug purpose, remove it

#define PREPROCESSOR '#'
#define STAGE_DIRECTIVE "stage"
#define SECTION_DIRECTIVE "section"

#define SECTION_SETTINGS "SETTINGS"
#define SECTION_LAYOUT "LAYOUT"
#define SECTION_SHADER "SHADER"

typedef struct shader_source_t
{
	const char* source;
	uint32_t length;
	uint8_t stage;
} shader_source_t;

static void buffer_write_string(BUFFER* buffer, const char* string, u32 len, bool null_terminate);
static void buffer_write_u8(BUFFER* buffer, u8 value);
static void buffer_write_u16(BUFFER* buffer, u16 value);
static void buffer_write_u32(BUFFER* buffer, u32 value);
static void buffer_insert_offset(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index);
static void buffer_insert_bytes(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index, const u8* bytes, u32 count);

SC_API BUFFER* sc_load_and_compile(const char* file_path)
{
	BUFFER* buffer = load_text_from_file(file_path);
	BUFFER* sb = sc_compile(buffer->bytes, buffer->element_count);
	buf_free(buffer);
	return sb;
}

/*
 		*------------------------INPUT STRING FORMAT------------------------------*

#section SETTINGS

#section LAYOUT

vertex [push_constant] uniform Push
{
	mat4 mvp_matrix; 		// 64 bytes
	mat4 model_matrix; 		// 64 bytes
} push;

vertex[0, 0] uniform Data
{
    float time;
} data;

vertex[0, 1] uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

fragment[0, 0] uniform Sampler2D albedo_texture;
fragment[0, 1] uniform Light
{
    float time;
    vec3 light_dir;
    float light_intensity;
} light;

#section SHADER

#stage vertex

// vertex shader code goes here

#stage fragment

//fragment shader code goes here

#stage geometry

//geometry shader code goes here

#stage tessellation

//tessellation shader code goes here

 0b 0000 0000 0101 0100
 0x  54 

 		*-------------------------------OUTPUT BINARY FORMAT---------------------------*

 Byte Index | Interpretation 			| Description
 ___________|___________________________|________________________________________
 0 			| string of length 13 bytes | "SHADER BINARY"
 			| u32, section masks  		| 32 bits are divided into 3 bits group with BIT(0) = settings, BIT(1) = layout, BIT(2) = shader
 			| u32, settings offset 		| settings section offset if BIT(0) is set
 			| u32, layout offset 		| layout section offset if BIT(1) is set
 			| u32, shader offset 		| shader section offset if BIT(2) is set
____________|___________________________|________________________________________
________________________________SETTINGS_________________________________________
			| VkPrimitiveTopology 		| topology
			| VkBool32 					| primitiveRestartEnable
------------|---------------------------|----------------------------------------
			| uint32_t 					| patchControlPoints
------------|---------------------------|----------------------------------------
			| uint32_t 					| viewportCount
			| array of VkViewport 		| viewports
			| uint32_t 					| scissorCount
			| array of VkRect2D 		| scissors
------------|---------------------------|----------------------------------------
			| VkBool32 					| depthClampEnable
			| VkBool32 					| rasterizerDiscardEnable
			| VkPolygonMode 			| polygonMode
			| VkCullModeFlags 			| cullMode
			| VkFrontFace 				| frontFace
			| VkBool32 					| depthBiasEnable
			| float 					| depthBiasConstantFactor
			| float 					| depthBiasClamp
			| float 					| depthBiasSlopeFactor
			| float 					| lineWidth
------------|---------------------------|----------------------------------------
			| VkSampleCountFlagBits 	| rasterizationSamples
			| VkBool32 					| sampleShadingEnable
			| float 					| minSampleShading
			| VkBool32 					| alphaToCoverageEnable
			| VkBool32 					| alphaToOneEnable
------------|---------------------------|----------------------------------------
			| VkBool32 					| depthTestEnable
			| VkBool32 					| depthWriteEnable
			| VkCompareOp 				| depthCompareOp
			| VkBool32 					| depthBoundsTestEnable
			| VkBool32 					| stencilTestEnable
			| VkStencilOpState 			| front
			| VkStencilOpState 			| back
			| float 					| minDepthBounds
			| float 					| maxDepthBounds
------------|--------------------------------------------------------------------
			| VkBool32 										| logicOpEnable
			| VkLogicOp 									| logicOp
			| uint32_t 										| attachmentCount
			| array of VkPipelineColorBlendAttachmentState 	| attachments
			| float[4] 										| blendConstants[4]
------------|-----------------------------------------------|--------------------
			| uint32_t 										| dynamicStateCount
			| array of VkDynamicState 						| dynamicStates
____________|_______________________________________________|____________________
________________________________LAYOUT___________________________________________
 0 			| u16 						| descriptor count + attribute count
 ...		| array of u32 				| offsets for each serialized descriptor
 0 			| u8 						| descriptor set number/index or vertex attribute binding number, if the type is push_constant then it would be the offset
 1 			| u8 						| descriptor binding number/index or vertex attribute layout number, if the type is push_constant then it would be 0xFF
 2 			| u32 						| BIT(18) set if the type is a per-vertex attribute
 			|							| BIT(17) set if the type is a per-instance attribute
  			| 							| BIT(16) set if the type is a push_constant
  			| 							| BIT(15) set if the type is an opaque type
 			| 							| BIT(14) set if the type is an uniform
  			| 							| BIT(13) = vertex shader, BIT(12) = tessellation shader, BIT(11) = geometry shader, BIT(10) = fragment shader
  			| 							| Lower 8 Bits contains the type information
			| null terminated string 	| if the type is block then it would be block name
 			| null terminated string 	| identifier name
 			| u16						| contains number of members in the block if the type is a block
 			| u32 						| contains length of the array if the type is an array
____________|___________________________|_________________________________________
________________________________SHADER____________________________________________
 			| u8 						| Render pass count
 			| RenderPassInfo object 	| 
 13 		| shader mask byte 			| BIT(0) = vertex shader, BIT(1) = tessellation shader, BIT(2) = geometry shader, BIT(3) = fragment shader
 14			| u32, contains a number 	| vertex shader offset if BIT(0) is set
 18			| u32, contains a number 	| vertex shader length if BIT(0) is set
 22			| u32, contains a number 	| tessellation shader offset if BIT(1) is set
 26			| u32, contains a number 	| tessellation shader length if BIT(1) is set
 30			| u32, contains a number 	| geometry shader offset if BIT(2) is set
 34			| u32, contains a number 	| geometry shader length if BIT(2) is set
 40			| u32, contains a number 	| fragment shader offset if BIT(3) is set
 44			| u32, contains a number 	| fragment shader length if BIT(3) is set
 */


typedef struct str_t
{
	const char* ptr;
	u32 len;
} str_t;

static const char* write_header(const char* start, const char* end, binary_writer_t* writer)
{
	binary_writer_string(writer, "V3D Shader Binary");

	// skip the whitespaces
	start = skip(start, " /t/n", end);
	while(*start == '#')
	{
		start++;
		while(*start != '\n')
		{
			// skip the spaces and tabs but not the newline as it is a delimiter for a preprocessor line
			start = skip(start, " /t", end);

			// command is for shader binary generation
			if(strncmp(start, "sb", 2) == 0)
			{
				binary_writer_u32(writer, (u32)CMPLR_CMD_CAT_SB);
				start += 2;
			}
			// command is for shader language compilation
			else if(strncmp(start, "sl", 2) == 0)
			{
				binary_writer_u32(writer, (u32)CMPLR_CMD_CAT_SL);
				start += 2;
			}
			// version { shader binary generation, shader language generation }
			else if(strncmp(start, "version", 7) == 0)
			{
				start += 7;
				u32 ver = _strtoui64(start, (char**)&start, 10);
				if(ver == 0)
					debug_log_fetal_error("Invalid shader binary or language version");
				binary_writer_u32(writer, ver);
			}
		}
		// skip the new line character
		start++;
	}

	return start;
}

typedef struct bidirectional_writer_t
{
	/* writes towards up */
	union
	{
		binary_writer_t* up;
		binary_writer_t* top;
	};
	/* writes towards down */
	union
	{
		binary_writer_t* down;
		binary_writer_t* bottom;
	};
} bidirectional_writer_t;

static inline u32 min(u32 v1, u32 v2) { return (v1 > v2) ? v2 : v1; }

static int safe_strncmp(const char* str1, const char* const str2, u32 len)
{
	int result = strncmp(str1, str2, min(strlen(str2), len));
	return result;
}

static const char* keywords[] = 
{
	"",
	"true",
	"false",
	"Shader",
	"Properties",
	"Layout",
	"RenderPass",
	"SubPass",
	"GLSL",
	"GraphicsPipeline"
};

/* keyword enumerations */
enum
{
	KEYWORD_UNDEFINED = 0UL,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_SHADER,
	KEYWORD_PROPERTIES,
	KEYWORD_LAYOUT,
	KEYWORD_RENDERPASS,
	KEYWORD_SUBPASS,
	KEYWORD_GLSL,
	KEYWORD_GFXPIPELINE,
	KEYWORD_MAX
};

enum
{
	/*QUALIFIER_FRAGMENT,
	QUALIFIER_VERTEX,
	QUALIFIER_PER_VERTEX,
	QUALIFIER_PER_INSTANCE,
	QUALIFIER_UNIFORM,
	QUALIFIER_BUFFER,*/
	QUALIFIER_SHADER = KEYWORD_SHADER,
	QUALIFIER_PROPERTIES = KEYWORD_PROPERTIES,
	QUALIFIER_LAYOUT = KEYWORD_LAYOUT,
	QUALIFIER_RENDERPASS = KEYWORD_RENDERPASS,
	QUALIFIER_SUBPASS = KEYWORD_SUBPASS,
	QUALIFIER_GLSL = KEYWORD_GLSL,
	QUALIFIER_GFXPIPELINE = KEYWORD_GFXPIPELINE
};

static const char* attribute_keywords[] = 
{
	"Name",
	"NoParse",
	"Input"
};

enum
{
	ATTRIBUTE_NAME,
	ATTRIBUTE_NOPARSE,
	ATTRIBUTE_INPUT
};

/* pair of a pointer and u32; usually used for storing pointer to a list of elements and the number of elements in that list */
typedef struct ptr_u32_pair_t
{
	void* ptr;
	u32 size;
} ptr_u32_pair_t;

typedef ptr_u32_pair_t* look_ahead_table_t;
typedef ptr_u32_pair_t* symbol_qualifiers_table_t;
typedef ptr_u32_pair_t* symbol_attributes_table_t;

typedef struct compile_ctx_t
{
	/* table for looking the expected symbols in the current depth */
	look_ahead_table_t lat;
	/* number of elements in the look ahead table */
	u32 lat_size;
	/* table of symbol qualifiers */
	symbol_qualifiers_table_t sqt;
	/* number of elements in the symbol qualifiers table */
	u32 sqt_size;
	/* table of symbol attributes */
	symbol_attributes_table_t sat;
	/* number of elements in the symbol attribute table */
	u32 sat_size;
	/* list of keywords in the language grammar */
	char** keywords;
	/* number of elements in the keywords list */
	u32 keywords_size;
	/* current depth of a block while parsing the source */
	s32 depth;
} compile_ctx_t;

#define PTR_U32_NULL (ptr_u32_pair_t) { }

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

static compile_ctx_t* compile_ctx_create()
{
	compile_ctx_t* ctx = CAST_TO(compile_ctx_t*, malloc(sizeof(compile_ctx_t)));

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

	return ctx;
}

static void compile_ctx_destroy(compile_ctx_t* ctx)
{
	// TODO	
}

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
				debug_log_warning("[Syntax] The attribute \"%.*s\" has no effect on %s", len, str, description);
			else
			{
				is_found = true;
				if(!syntax_check_attribute_args(&attributes[i], start))
					debug_log_error("[Syntax] Invalid arguments passed to \"%s\"", attribute_keywords[supported[j]]);
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
					debug_log_warning("[Syntax] The qualifier \"%.*s\" has no effect", len, str);
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

static void syntax_check(v3d_generic_node_t* node, const char* start, compile_ctx_t* ctx)
{
	// lets ignore the anonymous blocks
	if(node->qualifier_count == 0)
	{
		debug_log_error("anonymous entities (blocks) are not allowed for now");
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
	const char* str = name.start + start;
	u32 len = U32_PAIR_DIFF(name);

	/* check if the current node matches with any of the expected symbols (block names only for now) */
	if(syntax_check_node(node, start, look_ahead_symbols, symbol_count, NULL, 0, NULL, true))
	{
		for(u32 i = 0; i < symbol_count; i++)
		{
			u32 symbol = look_ahead_symbols[i];
			if(safe_strncmp(str, keywords[symbol], len) == 0)
			{
				bool result = syntax_check_node(node, start,
							ctx->sqt[symbol].ptr, ctx->sqt[symbol].size,
							ctx->sat[symbol].ptr, ctx->sat[symbol].size,
							keywords[symbol], false);
				assert(result == true);
				ctx->depth = symbol;
			}
		}
	}
	else if(node->qualifier_count <= 1)
		debug_log_error("[Syntax] Unexpected symbol: %.*s", len, str);
	else
		debug_log_info("[Syntax] Assuming identifier: %.*s", len, str);

	/* -- syntax check for the value of the entity -- */
	
	/* if this node has a value node */
	if(node->has_value)
	{
		// TODO
	}

	for(u32 i = 0; i < node->child_count; i++)
		syntax_check(node->childs[i], start, ctx);

	ctx->depth = depth;
}

static void semantic_check(v3d_generic_node_t* root, const char* start)
{
	// TODO
}

static void codegen(v3d_generic_node_t* root, bidirectional_writer_t* writer)
{

}

static void optimize(v3d_generic_node_t* root, binary_reader_t* unopt_reader, binary_writer_t* opt_writer)
{

}

/* NOTE: we can't just pass pointer to buf_* functions to binary_writer
	because they are macros and contain some other invisible parameters 
 */
static void writer_push(void* user_data, const void* bytes, u32 size)
{
	/* see: shader_compiler/source/utilities/binary_writer->c::mark(...) */
	if(size == 0)
		return;
	u32 index = buf_get_element_count(user_data);
	buf_push_pseudo(CAST_TO(BUFFER*, user_data), size);
	if(bytes != NULL)
		memcpy(buf_get_ptr_at(user_data, index), bytes, size);
}

static void writer_insert(void* user_data, u32 index, const void* bytes, u32 size)
{
	buf_insert_pseudo(user_data, index, size);
	memcpy(buf_get_ptr_at(user_data, index), bytes, size);
}

static void* writer_ptr(void* user_data)
{
	return buf_get_ptr(user_data);
}

static u32 writer_pos(void* user_data)
{
	return CAST_TO(u32, buf_get_element_count(user_data));
}

SC_API BUFFER* sc_compile(const char* start, u32 length)
{
	remove_comments((char*)start, length);

	/* buffer growing downwards */
	BUFFER* down_buffer = BUFcreate(NULL, sizeof(u8), 0, 0);
	/* buffer growing upwards */
	BUFFER* up_buffer = BUFcreate(NULL, sizeof(u8), 0, 0);

	bidirectional_writer_t writer = 
	{
		binary_writer_create((void*)down_buffer, writer_push, writer_insert, writer_ptr, writer_pos),
		binary_writer_create((void*)up_buffer, writer_push, writer_insert, writer_ptr, writer_pos)
	};

	// end should point to null character
	const char* end = start + length;
	
	// write the file header
	start = write_header(start, end, writer.down);

	// parse the source code (perhaps partially)
	ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(start, CAST_TO(u32, end - start));
	switch(result.result)
	{
		case PPSR_SUCCESS:
			debug_log_info("Shader parsed success");
			break;
		case PPSR_WARNING:
			debug_log_warning(result.log_buffer);
			break;
		case PPSR_ERROR:
			debug_log_fetal_error(result.log_buffer);
			break;
		case PPSR_ERROR_UNKOWN:
			debug_log_fetal_error("Unknown error has been occured while parsing the shader");
			break;
		default:
			debug_log_fetal_error("Invalid or Unrecognized result code recieved from the parser");
	}

	compile_ctx_t* ctx = compile_ctx_create();

	/* perform syntax checking */
	syntax_check(result.root, start, ctx);
	/* perform semantic analysis */
	semantic_check(result.root, start);
	/* generate code */
	codegen(result.root, &writer);
	/* perform optimization passes */
	optimize(result.root, NULL /*&unopt_reader*/, NULL/*&opt_writer*/);

	compile_ctx_destroy(ctx);
	binary_writer_destroy(writer.up);
	binary_writer_release_resources(writer.up);
	binary_writer_destroy(writer.down);
	binary_writer_release_resources(writer.down);
	debug_log_info("Compiled shader binary info: { size = %llu bytes }\n", buf_get_element_count(up_buffer) + buf_get_element_count(down_buffer));
	return down_buffer;
}

static void buffer_insert_bytes(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index, const u8* bytes, u32 size)
{
	buf_ucount_t count = buf_get_element_count(offsets_buffer);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		buf_ucount_t _index;
		buf_get_at(offsets_buffer, i, &_index);
		if(_index < index)
			continue;
		*(u32*)buf_get_ptr_at(buffer, _index) += size;
		_index += size;
		buf_set_at(offsets_buffer, i, &_index);
	}
	buf_insert_pseudo(buffer, index, size);
	char* const ptr = buf_get_ptr_at(buffer, index);
	memcpy(ptr, bytes, size);
}

static void buffer_insert_offset(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index)
{
	// update all the previous offsets
	buf_ucount_t count = buf_get_element_count(offsets_buffer);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		buf_ucount_t _index;
		buf_get_at(offsets_buffer, i, &_index);
		if(_index < index)
			continue;
		*(u32*)buf_get_ptr_at(buffer, _index) += 4;
		_index += 4;
		buf_set_at(offsets_buffer, i, &_index);
	}

	// register for next update
	buf_push(offsets_buffer, &index);
	
	// insert 4 bytes into "buffer"
	buf_insert_pseudo(buffer, index, 4);
	*(u32*)buf_get_ptr_at(buffer, index) = buf_get_element_count(buffer);
}

/* shader stages */
#define VERTEX_BIT 										BIT32(8 + 0)
#define FRAGMENT_BIT 									BIT32(8 + 1)
#define GEOMETRY_BIT 									BIT32(8 + 2)
#define TESSELLATION_BIT 								BIT32(8 + 3)

/* shader resource description types */
#define PUSH_CONSTANT_BIT 								BIT32(16 + 0)
#define OPAQUE_BIT 										BIT32(16 + 1)
#define UNIFORM_BUFFER_BIT 								BIT32(16 + 2)
#define STORAGE_BUFFER_BIT 								BIT32(16 + 3)
#define PER_VERTEX_ATTRIB_BIT 							BIT32(16 + 4)
#define PER_INSTANCE_ATTRIB_BIT 						BIT32(16 + 5)

// marks in the binary_writer_t
enum
{
	MARK_ID_DESCRIPTOR_COUNT,
	MARK_ID_DESCRIPTOR_OFFSET,
	MARK_ID_IDENTIFIER_NAME,
	MARK_ID_FIELD_COUNT,
	MARK_ID_MAX
};


/*
  		*---------------------------INPUT STRING FORMAT---------------------------*

per-vertex [0, 0] vec3 position;
per-vertex [0, 1] vec3 normal;
per-vertex [0, 2] vec2 texcoord;
per-vertex [0, 3] vec3 tangent;

per-instance [0, 0] vec3 offset;

vertex [push_constant] uniform Push
{
	mat4 mvp_matrix; 		// 64 bytes
	mat4 model_matrix; 		// 64 bytes
} push;

vertex[0, 0] uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

fragment[0, 0] uniform sampler2D albedo_texture;
fragment vertex [0, 1] uniform SceneData
{
    float time;
    vec3 light_dir;
    float light_intensity;
} scene_data;

	*------------------------------OUTPUT BINARY FORMAT----------------------------*
 Byte Index 	| Interpretation			| Description
 _______________|___________________________|___________________________________________
 0 				| u16 						| descriptor count + attribute count
 ...			| array of u32 				| offsets for each serialized descriptor
 0 				| u8 						| descriptor set number/index or vertex attribute binding number, if the type is push_constant then it would be the offset
 1 				| u8 						| descriptor binding number/index or vertex attribute layout number, if the type is push_constant then it would be 0xFF
 2 				| u32 						| BIT(18) set if the type is a per-vertex attribute
 				|							| BIT(17) set if the type is a per-instance attribute
  				| 							| BIT(16) set if the type is a push_constant
  				| 							| BIT(15) set if the type is an opaque type
 				| 							| BIT(14) set if the type is an uniform
  				| 							| BIT(13) = vertex shader, BIT(12) = tessellation shader, BIT(11) = geometry shader, BIT(10) = fragment shader
  				| 							| Lower 8 Bits contains the type information
 3 				| null terminated string 	| if the type is block then it would be block name
 len0() + 4		| null terminated string 	| identifier name
 len01() + 5	| u16						| contains number of members in the block if the type is a block
 len01() + 7 	| u32 						| contains length of the array if the type is an array
 */


static const char* parse_qualifers(const char* start, const char* end, u32 OUT bits);
static const char* parse_square_brackets(const char* start, const char* end, u32 OUT bits);
static const char* write_set_and_binding_numbers(const char* start, const char* end, u8 required, binary_writer_t* writer);
static const char* parse_storage_qualifiers(const char* start, const char* end, u32 OUT bits);
static const char* write_description(const char* start, const char* end, u32 bits, binary_writer_t* writer);

static void write_layout(const char* start, const char* end, bidirectional_writer_t* writer)
{
	if(is_empty(start, end))
	{
		debug_log_info("Properties and Layout, both blocks are empty, skipping");
		binary_writer_u16(writer->down, (u16)0);
		return;
	}

	binary_writer_u16_mark(writer->down, MARK_ID_DESCRIPTOR_COUNT);

	u16 descriptor_count = 0;
	while(start < end)
	{
		while(isspace(*start)) start++;
		if(start >= end) break; //end of the file reached

		binary_writer_u32_mark(writer->top, MARK_ID_DESCRIPTOR_OFFSET + MARK_ID_MAX + descriptor_count);
		binary_writer_u32_set(writer->top, MARK_ID_DESCRIPTOR_OFFSET + MARK_ID_MAX + descriptor_count, binary_writer_pos(writer->bottom));

		u32 bits = 0;				// description info bits

		// parse the qualifiers such as per_vertex, per_instance, fragment, vertex, geometry and tessellation
		start = parse_qualifers(start, end, &bits);
		// parse the first pair of square brackets, i.e. [push_constant]
		start = parse_square_brackets(start, end, &bits);
		// parse the second pair of square backets, it shall contain list of numbers only
		// if push_constant found then look for offset (a number)
		start = write_set_and_binding_numbers(start, end, (bits & PUSH_CONSTANT_BIT) ? 1 : 2, writer->bottom);

		// look for storage qualifiers only if there is a shader stage qualifier
		if(!((bits & PER_VERTEX_ATTRIB_BIT) || (bits & PER_INSTANCE_ATTRIB_BIT)))
			start = parse_storage_qualifiers(start, end, &bits);

		start = write_description(start, end, bits, writer->bottom);
		
		binary_writer_u32(writer->bottom, bits);

		descriptor_count++;
	}

	//Write number of descriptor count, 2 BYTES
	binary_writer_u16_set(writer->down, MARK_ID_DESCRIPTOR_COUNT, descriptor_count);

	// //Reverse the descriptor_offsets (array of u32)
	// u32* descriptor_offsets = buf_get_ptr_at(buffer, descriptor_offsets_index);
	// for(u16 i = 0; i < (descriptor_count >> 1); i++)
	// {
	// 	u32 temp = descriptor_offsets[i];
	// 	descriptor_offsets[i] = descriptor_offsets[descriptor_count - i - 1];
	// 	descriptor_offsets[descriptor_count - i - 1] = temp;
	// }
}

/*
	fragment[0, 0] uniform sampler2D albedo_texture;
	fragment vertex [0, 1] uniform SceneData
	{
	    float time;
	    vec3 light_dir;
	    float light_intensity;
	} scene_data;
 */
static const char* write_description(const char* start, const char* end, u32 bits, binary_writer_t* writer)
{
	while(isspace(*start)) start++;
	u32 count = get_word_length(start, 0);
	bool found = true;
	buf_ucount_t identifier_name_index;
	if(strncmp(start, "sampler2D", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_2D; 
		bits |= OPAQUE_BIT; 
	}
	else if(strncmp(start, "sampler3D", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_3D; 
		bits |= OPAQUE_BIT; 
	}
	else if(strncmp(start, "samplerCube", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_CUBE;
		bits |= OPAQUE_BIT;
	}
	else if(strncmp(start, "vec4", count) == 0)
		bits |= GLSL_TYPE_VEC4;
	else if(strncmp(start, "vec3", count) == 0)
		bits |= GLSL_TYPE_VEC3;
	else if(strncmp(start, "vec2", count) == 0)
		bits |= GLSL_TYPE_VEC2;
	else if(strncmp(start, "ivec4", count) == 0)
		bits |= GLSL_TYPE_IVEC4;
	else if(strncmp(start, "ivec3", count) == 0)
		bits |= GLSL_TYPE_IVEC3;
	else if(strncmp(start, "ivec2", count) == 0)
		bits |= GLSL_TYPE_IVEC2;
	else if(strncmp(start, "uvec4", count) == 0)
		bits |= GLSL_TYPE_UVEC4;
	else if(strncmp(start, "uvec3", count) == 0)
		bits |= GLSL_TYPE_UVEC3;
	else if(strncmp(start, "uvec2", count) == 0)
		bits |= GLSL_TYPE_UVEC2;
	else if(strncmp(start, "int", count) == 0)
		bits |= GLSL_TYPE_INT;
	else if(strncmp(start, "uint", count) == 0)
		bits |= GLSL_TYPE_UINT;
	else if(strncmp(start, "float", count) == 0)
		bits |= GLSL_TYPE_FLOAT;
	else if(strncmp(start, "mat4", count) == 0)
		bits |= GLSL_TYPE_MAT4;
	else if(strncmp(start, "mat3", count) == 0)
		bits |= GLSL_TYPE_MAT3;
	else if(strncmp(start, "mat2", count) == 0)
		bits |= GLSL_TYPE_MAT2;
	else
	{
		found = false;
		const char* block_name = start;
		start += count;
		while(*start != '{')
		{
			if(!isspace(*start))
			{
				count = get_word_length(start, 0);
				debug_log_fetal_error("shader layout parse error: Unexpected symbol \"%.*s\", '{' is expected\n", count, start);
			}
			start++;
		}
		if((count == 0) || is_empty(block_name, block_name + count))
			debug_log_fetal_error("shader layout parse error: Block name cannot be empty\n");
		start++;

		u32 _bits = bits;
		if(bits & STORAGE_BUFFER_BIT)
			bits |= GLSL_TYPE_STORAGE_BUFFER;
		else if(bits & PUSH_CONSTANT_BIT)
			bits |= GLSL_TYPE_PUSH_CONSTANT;
		else 
		{
			bits |= UNIFORM_BUFFER_BIT;
			bits |= GLSL_TYPE_UNIFORM_BUFFER;
		}

		// write type info to the buffer, 4 BYTES
		binary_writer_u32(writer, bits);

		// write block name to the buffer, count + 1 BYTES
		binary_writer_stringn(writer, block_name, count);
		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME);
		binary_writer_u16_mark(writer, MARK_ID_FIELD_COUNT);

		u16 num_elements = 0;
		while(*start != '}')
		{
			u16 info;
			start = write_description(start, end, _bits, writer);
			while(isspace(*start)) start++;
			num_elements++;
		}

		// write number of elements to the buffer, 2 BYTES
		binary_writer_u16_set(writer, MARK_ID_FIELD_COUNT, num_elements);

		start++;
	}

	if(found)
	{
		// write type info to the buffer, 4 BYTES
		binary_writer_u32(writer, bits);
		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME);
		start += count;
	}

	while(isspace(*start)) start++;
	count = get_word_length(start, ';');
	if((count == 0) || is_empty(start, start + count))
		debug_log_fetal_error("shader layout parse error: Identifier name cannot be empty\n");

	// write identifier name to the buffer, count + 1 BYTES
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME, start, count);
	char null_char = 0;
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME, &null_char, 1);

	start += count;
	while(*start != ';')
	{
		if(!isspace(*start))
		{
			count = get_word_length(start, 0);
			debug_log_fetal_error("shader layout parse error: Expected ';' before \"%.*s\"\n", count, start);
		}
		start++;
	}
	start++;
	return start;
}

static const char* parse_storage_qualifiers(const char* start, const char* end, u32 OUT bits)
{
	while(isspace(*start)) start++;
	u32 mask = 0;
	u32 count = get_word_length(start, 0);
	// if(strncmp(start, "uniform", count) == 0) do nothing
	if(strncmp(start, "buffer", count) == 0)
		mask |= STORAGE_BUFFER_BIT;
	else
		debug_log_fetal_error("shader data layout parse error: Unrecognized storage qualifier \"%.*s\"\n", count, start);
	OUT bits |= mask;
	return start + count;
}

static const char* parse_qualifers(const char* start, const char* end, u32 OUT bits)
{
	u32 count = 0;
	u8 stage_count = 0;
	u32 mask = 0;
	bool found_attribute = false;
	while(true)
	{
		while(isspace(*start) && (start < end)) start++;
		count = get_word_length(start, '[');
		if(count == 0) break;
		if(strncmp(start, "per", min(3, count)) == 0)
		{
			if(strncmp(start + 3, "_vertex", min(7, count - 3)) == 0)
				mask |= PER_VERTEX_ATTRIB_BIT;
			else if(strncmp(start + 3, "_instance", min(9, count - 3)) == 0)
				mask |= PER_INSTANCE_ATTRIB_BIT;
			else
				debug_log_fetal_error("shader data layout parse error: Unrecognized vertex attribute qualifer \"%.*s\"\n", count, start);
			start += count;
			found_attribute = true;
			break;
		}
		else if(strncmp(start, "vertex", count) == 0)
			mask |= VERTEX_BIT;
		else if(strncmp(start, "tessellation", count) == 0)
			mask |= TESSELLATION_BIT;
		else if(strncmp(start, "geometry", count) == 0)
			mask |= GEOMETRY_BIT;
		else if(strncmp(start, "fragment", count) == 0)
			mask |= FRAGMENT_BIT;
		else break;
		start += count;
		stage_count++;
	}
	if((stage_count == 0) && !found_attribute)
		debug_log_fetal_error("shader layout parse error: Unrecognized symbol \"%.*s\", expected shader stage or vertex attribute qualifer!\n", count, start);
	OUT bits |= mask;
	return start;
}

static const char* parse_square_brackets(const char* start, const char* end, u32 OUT bits)
{
	const char* origin = start;
	while(isspace(*start)) start++;
	if(strncmp(start, "[", 1) == 0)
	{
		start++;
		while(isspace(*start)) start++;
		u8 len = get_word_length(start, ']');
		if(strncmp(start, "push_constant", len) == 0)
			OUT bits |= PUSH_CONSTANT_BIT;
		else
			return origin;
		start += len;
		while(*start != ']')
		{
			if(!isspace(*start))
			{
				len = get_word_length(start, 0);
				debug_log_fetal_error("shader layout parse error: Unrecognized symbol \"%.*s\" in square brackets\n", len, start);
			}
			start++;
		}
		start++;
	}
	return start;
}

static const char* write_set_and_binding_numbers(const char* start, const char* end, u8 required, binary_writer_t* writer)
{
	while(isspace(*start)) start++;
	u8 count = 0;
	u32 arr_len = 0;
	bool found = false;
	if(strncmp(start, "[", 1) == 0)
	{
		found = true;
PARSE_NUMBER:
		start++;
		while(isspace(*start)) start++;
		u8 len = 0;
		while(isdigit(*start)) { start++; len++; };
		char sub_str[len + 1];
		/*	
			WARNING: 	Don't use strncpy(sub_str, start - len, len) here!
		 	strncpy: 	No null-character is implicitly appended at the end of destination if source is longer than num. 
		 				Thus, in this case, destination shall not be considered a null terminated C string (reading it as such would overflow).
		*/
		memcpy(sub_str, start - len, len); sub_str[len] = 0;
		arr_len = strtoul(sub_str, NULL, 0);


		// first iteration: write descriptor set number/index or binding number in case of vertex attribute, 1 BYTE
		// second iteration: write descriptor binding number/index or layout number in case of vertex attribute, 1 BYTE
		binary_writer_u8(writer, (u8)arr_len);
		
		// log_u32(arr_len);
		
		count++;
		if(count > required)
			debug_log_fetal_error("shader layout parse error: More than required elements found in the array index operator, \"%u\"\n", arr_len);
		while(*start != ']')
		{
			if(*start == ',')	
				goto PARSE_NUMBER;
			// if(!isspace(*start))
			// 	shader_data_layout_parse_error(SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE, 1, start + 1);
			start++;
		}
		start++;
	}
	if(!found)
	{
		u32 len = get_word_length(start, 0);
		debug_log_fetal_error("shader layout parse error: Expected open square bracket '[' before \"%.*s\"\n", count, start);
	}
	if(count < required)
		debug_log_fetal_error("shader layout parse error: Less than required elements found in the array index operator, \"%u\"\n", arr_len);
	return start;
}

// static void shader_data_layout_parse_error(u32 err, u32 len, const char* string)
// {
// 	switch(err)
// 	{
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_SHADER_STAGE : debug_log_fetal_error("shader_data_layout parse error: Unsupported shader stage: \"%.*s\"\n", len, string - len); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_DIRECTIVE : debug_log_fetal_error("shader_data_layout parse error: Unsupported preprocessor directive \"%s\"\n", len, string - len); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_DECLARATION : debug_log_fetal_error("shader_data_layout parse error: Invalid array declaration\n"); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_SIZE_INVALID : debug_log_fetal_error("shader_data_layout parse error: Array size is invalid\n"); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_ARRAY_SIZE_ZERO : debug_log_fetal_error("shader_data_layout parse error: Array size can't be zero\n"); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE : debug_log_fetal_error("shader_data_layout parse error: Invalid symbol \"%.*s\" in array index operator\n", len, string - len); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNRECOGNIZED_DATA_TYPE : debug_log_fetal_error("shader_data_layout parse error: Unrecognised data type: \"%.*s\"\n", len, string - len); break;
// 		case SHADER_DATA_LAYOUT_PARSE_ERROR_SHADER_STAGE_NOT_FOUND: debug_log_fetal_error("shader_data_layout parse error: Shader stage not found\n");
// 		default: debug_log_fetal_error("shader_data_layout parse error: Unkown\n"); break;
// 	}
// }

static void buffer_write_string(BUFFER* buffer, const char* string, u32 len, bool null_terminate)
{
	buf_pushv(buffer, (char*)string, len);
	if(!null_terminate)
		return;
	char null_char = 0;
	buf_push(buffer, &null_char);
}

static void buffer_write_u8(BUFFER* buffer, u8 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_push(buffer, &value);
}

static void buffer_write_u16(BUFFER* buffer, u16 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_ucount_t index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 2);
	*(u16*)buf_get_ptr_at(buffer, index) = value;
}

static void buffer_write_u32(BUFFER* buffer, u32 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_ucount_t index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 4);
	*(u32*)buf_get_ptr_at(buffer, index) = value;
}


static const char* parse_glsl_code(const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer);

// static const char* parse_pass(const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer, const char* pass_name)
// {
// 	const char* c = _source;
// 	const char* end = _source + length;
// 	c = skip_whitespaces(c, end);

// 	while(*c == '[')
// 	{
// 		// ...
// 		c = skip_whitespaces(c, end);
// 	}

// 	if(strncmp(c, pass_name, strlen(pass_name)) == 0)
// 	{
// 		c += strlen(pass_name);
// 		c = skip_whitespaces(c, end);
// 		if(*c != '{')
// 			debug_log_fetal_error("Pass parse error: Unrecognized symbol \"%c\", expected a open brace \"{\"\n", *c);
// 		c++;		// skip the '{' character
// 		const char* str = c;

// 		// if this is RenderPass then parse the SubPasses inside it
// 		if(strcmp(pass_name, "RenderPass") == 0)
// 		{
// 			str = skip_whitespaces(str, end);
// 			while(strncmp(str, "SubPass", strlen("SubPass")) == 0)
// 				str = parse_pass(str, end - str, buffer, offsets_buffer, "SubPass");
// 			c = skip_whitespaces(str, end);
// 			if(*c != '}')
// 				debug_log_fetal_error("Pass parse error: %s is not closed, expected \"}\"\n", pass_name);
// 			c++;	 // skip '}' character
// 		}
// 		else // if this is SubPass
// 		{
// 			// calculate the length of the str
// 			int open_brace_count = 0;
// 			while(true)
// 			{
// 				switch(*c)
// 				{
// 					case '{' : open_brace_count++; break;
// 					case '}' : open_brace_count--; break;
// 				}
// 				++c;
// 				if((open_brace_count < 0) || (c >= end)) break;
// 			}
// 			if(open_brace_count >= 0)
// 				debug_log_fetal_error("Pass parse error: %s is not closed, expected \"}\"\n", pass_name);

// 			parse_glsl_code(str, c - str - 1, buffer, offsets_buffer);
// 		}
// 	}
// 	else
// 	{
// 		u32 len = __get_word_length(c, end, "\0{");
// 		debug_log_fetal_error("Pass parse error: Unrecognized symbol \"%.*s\", expected \"%s\"\n", len, c, pass_name);
// 	}

// 	return skip_whitespaces(c, end);
// }

// static const char* parse_glsl_code(const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer)
// {
// 	const char* string = _source;
// 	const char* end = _source + length;
// 	//Parse the _source string to separate vertex, tessellation, geometry, and fragment shaders
// 	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
// 	shader_source_t sources[SHADER_COMPILER_SHADER_STAGE_MAX]; memset(sources, 0, sizeof(shader_source_t) * SHADER_COMPILER_SHADER_STAGE_MAX);
// 	const char* temp = NULL;
// 	shader_source_t* previous_source = NULL;
// 	uint8_t shader_count = 0;
// 	u8 vertex_shader_count = 0;
// 	u8 tessellation_shader_count = 0;
// 	u8 geometry_shader_count = 0;
// 	u8 fragment_shader_count = 0;
// 	while(((string = strchr(string, PREPROCESSOR)) != NULL) && (string < end))
// 	{
// 		temp = string - 1;
// 		string += 1;
// 		while((*string == ' ') || (*string == '\t')) string++;
// 		if(strncmp(string, STAGE_DIRECTIVE, strlen(STAGE_DIRECTIVE)) != 0)
// 			continue;
// 		string += strlen(STAGE_DIRECTIVE);
// 		while((*string == ' ') || (*string == '\t')) string++;
// 		uint32_t count = 0;
// 		while(!isspace(*string)) { string++; count++; }
// 		if(count > strlen(SHADER_STAGE_TESSELLATION_STR))
// 			debug_log_fetal_error(	"Shader parse error: Shader stage name must one of the followings:\n"
// 							"1. %s\n"
// 							"2. %s\n"
// 							"3. %s\n"
// 							"4. %s\n",
// 							SHADER_STAGE_VERTEX_STR,
// 							SHADER_STAGE_TESSELLATION_STR,
// 							SHADER_STAGE_GEOMETRY_STR,
// 							SHADER_STAGE_FRAGMENT_STR
// 						 );
// 		if(previous_source != NULL)
// 			previous_source->length = temp - previous_source->source;
// 		string -= count;
// 		if(strncmp(string, SHADER_STAGE_VERTEX_STR, count) == 0)
// 		{
// 			if(vertex_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_VERTEX_SHADER_FOUND);
// 			//set vertex shader
// 			string = strchr(string, '\n') + 1;
// 			sources[SHADER_COMPILER_SHADER_STAGE_VERTEX] = (shader_source_t)
// 			{
// 				.source = string,
// 				.stage = SHADER_COMPILER_SHADER_STAGE_VERTEX,
// 			};
// 			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_VERTEX];
// 			shader_count++;
// 			vertex_shader_count++;
// 			continue;
// 		}
// 		if(strncmp(string, SHADER_STAGE_TESSELLATION_STR, count) == 0)
// 		{
// 			if(tessellation_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_TESSELLATION_SHADER_FOUND);
// 			//set tessellation shader
// 			string = strchr(string, '\n') + 1;
// 			sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION] = (shader_source_t)
// 			{
// 				.source = string,
// 				.stage = SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
// 			};
// 			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION];
// 			shader_count++;
// 			tessellation_shader_count++;
// 			continue;
// 		}
// 		if(strncmp(string, SHADER_STAGE_GEOMETRY_STR, count) == 0)
// 		{
// 			if(geometry_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_GEOMETRY_SHADER_FOUND);
// 			//set geometry shader
// 			string = strchr(string, '\n') + 1;
// 			sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY] = (shader_source_t)
// 			{
// 				.source = string,
// 				.stage = SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
// 			};
// 			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY];
// 			shader_count++;
// 			geometry_shader_count++;
// 			continue;
// 		}
// 		if(strncmp(string, SHADER_STAGE_FRAGMENT_STR, count) == 0)
// 		{
// 			if(fragment_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_FRAGMENT_SHADER_FOUND);
// 			//set fragment shader
// 			string = strchr(string, '\n') + 1;
// 			sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT] = (shader_source_t)
// 			{
// 				.source = string,
// 				.stage = SHADER_COMPILER_SHADER_STAGE_FRAGMENT,
// 			};
// 			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT];
// 			shader_count++;
// 			fragment_shader_count++;
// 			continue;
// 		}

// 		debug_log_fetal_error("Shader parse error: Shader stage not found\n");
// 	}
// 	if(previous_source != NULL)
// 		previous_source->length = _source + length - previous_source->source;
// 	serialize_shader(stage_strings, sources, shader_count, buffer, offsets_buffer);
// 	return _source + length;
// }

// static function_signature(const char*, parse_shader, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer)
// {
// 	CALLTRACE_BEGIN();
// 	const char* string = _source;

// 	// check for the empty section
// 	if(is_empty(string, string + length))
// 	{
// 		LOG_MSG("SHADER section is empty, skipping\n");
// 		CALLTRACE_RETURN(_source + length);
// 	}

// 	// allocate 1 byte space for the render pass count
// 	buf_ucount_t index = buf_get_element_count(buffer);
// 	buf_push_pseudo(buffer, 1);
	
// 	u32 render_pass_count = 0;
// 	// parse each render pass
// 	while(string < (_source + length))
// 	{
// 		string = parse_pass(string, length - (string - _source), buffer, offsets_buffer, "RenderPass");
// 		++render_pass_count;
// 	}

// 	// set the render pass count
// 	buf_set_at(buffer, index, &render_pass_count);

// 	CALLTRACE_RETURN(string);
// }

// static function_signature(void, serialize_shader, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count, BUFFER* buffer, BUFFER* offsets_buffer)
// {
// 	CALLTRACE_BEGIN();

// 	uint8_t shader_mask = 0x00;
// 	for(uint8_t i = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
// 	{
// 		shader_source_t source = sources[i];
// 		if(source.length == 0) continue;
// 		shader_mask |= (0x01 << source.stage);
// 	}

// 	//Write the shader mask 0 0 0 0  1 0 0 1 -> fragment & vertex shaders
// 	buf_push(buffer, &shader_mask);

// 	// log_u32(shader_mask);

// 	//Allocate space for offsets and lengths for each shader SPIRV binary
// 	buf_ucount_t indices[shader_count];
// 	for(uint8_t i = 0; i < shader_count; i++)
// 	{
// 		indices[i] = buf_get_element_count(buffer);
// 		buf_push_pseudo(buffer, 8);
// 		// register for next update
// 		buf_push(offsets_buffer, &indices[i]);
// 	}

// 	shaderc_compiler_t compiler = shaderc_compiler_initialize();
// 	shaderc_compile_options_t options = shaderc_compile_options_initialize();
// 	for(uint8_t i = 0, j = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
// 	{
// 		shader_source_t source = sources[i];
// 		if(source.length == 0) continue;
// 		uint32_t shader_type = 0;
// 		switch(source.stage)
// 		{
// 			case SHADER_COMPILER_SHADER_STAGE_VERTEX: shader_type = shaderc_vertex_shader; break;
// 			case SHADER_COMPILER_SHADER_STAGE_TESSELLATION: shader_type = shaderc_tess_control_shader; break;
// 			case SHADER_COMPILER_SHADER_STAGE_GEOMETRY: shader_type = shaderc_geometry_shader; break;
// 			case SHADER_COMPILER_SHADER_STAGE_FRAGMENT: shader_type = shaderc_fragment_shader; break;
// 			default: debug_log_fetal_error("Shader compile error: stage \"%u\" is undefined or unsupported shader stage\n", source.stage); break;
// 		}
// 		shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source.source, source.length, shader_type, stage_strings[source.stage], "main", options);
// 		assert(result != NULL);
// 		if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
// 			debug_log_fetal_error("Shader compile error: %s | %s\n", stage_strings[source.stage], shaderc_result_get_error_message(result));

// 		void* bytes = (void*)shaderc_result_get_bytes(result);
// 		assert(bytes != NULL);
// 		uint64_t length = shaderc_result_get_length(result);
// 		assert(length > 0);
// 		uint32_t offset = buf_get_element_count(buffer);
// 		// log_u32(offset);
// 		// log_u32(length);

// 		//Write offset
// 		memcpy(buf_get_ptr_at(buffer, indices[j]), &offset, 4);

// 		//Write length
// 		memcpy(buf_get_ptr_at(buffer, indices[j]) + 4, &length, 4);

// 		//Write SPIRV binary
// 		buf_pushv(buffer, bytes, length);

// 		shaderc_result_release(result);
// 		j++;
// 	}
// 	shaderc_compile_options_release(options);
// 	shaderc_compiler_release(compiler);
// 	CALLTRACE_END();
// }
