/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: gfx_pipeline.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/codegen/gfx_pipeline.h>
#include <shader_compiler/assert.h> 		// assert()
#include <shader_compiler/debug.h>
#include <shader_compiler/utilities/string.h>
#include <common/dictionary.h>

#include <stddef.h> 			// offsetof
#include <stdio.h>
#include <stdlib.h>

typedef struct callbacks_t
{
	void* user_data; 								// pointer to a user data

	// callback when a category is started
	void (*begin_category)(const char* str,  		// str: pointer to the name of the started category (not null terminated)
							u32 length, 			// length: length of the name of the started category
							void* user_data); 		// user_data: pointer to the user data
	
	// callback when a category is ended
	void (*end_category)(void* user_data);  		// user_data: pointer to the user data
	
	// callback when a field is started
	void (*field)(	const char* str,  				// str: pointer to the name of the started field (not null terminated)
					u32 length,  					// length: length of the name of the started field
					const char* value,  			// value: pointer to the string value (field's value) (not null terminated)
					u32 value_length,  				// value_length: length of the string value
					void* user_data); 				// user_data: pointer to the user data

	// callback when an attribute is started
	void (*attribute)(	const char* str,  			// str: pointer to the name of the started attribute (not null terminated)
						u32 length,  				// length: length of the name of the attribute
						const char* value,  		// value: pointer to the string value (attribute's value) (not null terminated)
						u32 value_length,  			// value_length: length of the string value
						void* user_data); 			// user_data: pointer to the user data

} callbacks_t;

/* 	field's value writer function
	str: pointer to a null terminated string which represents a value (in string form)
	output: pointer to a memory block enough to hold the final parsed value
*/
typedef void (*ValueWriter)(const char* str, void* output);

typedef struct Field
{
	buf_ucount_t offset;				// offset of the field
	ValueWriter write;					// pointer to a writer function corresponding to this field
} Field;

/* key = string, value = Field */
typedef dictionary_t field_map_t;
/* key = string, value = Category */
typedef dictionary_t category_map_t;

typedef struct Category
{
	field_map_t* fields;
	category_map_t* categories;
	dictionary_t* categoryArrays;
	/* true if this Category is an instance of a template category */
	bool isTemplate;
} Category;

typedef buffer_t category_stack_t;

typedef struct UserData
{
	com_allocation_callbacks_t* callbacks;
	category_stack_t categoryStack; 			// stack of Category*
	category_map_t* tree;				// tree of root catagories
	dictionary_t categoryTemplates; // list of category templates to be instantiated at the runtime of the parsing algorithm
	BUFFER* literalBuffer;

	buf_ucount_t mainBaseOffset; 	// offset of the main branch of the output buffer
	BUFFER* mainOutput; 			// main branch of the output buffer
	buf_ucount_t baseOffset; 		// this offset changes at the runtime of the parsing algorithm (possibly when categories switches)
	BUFFER* output; 				// this output buffer changes at the runtime of the parsing algorithm (possibly when categories switches)

	/* default values for variable sized parameters */
	VkPipelineColorBlendAttachmentState defaultColorBlendAttachmentState;
	VkRect2D defaultScissor;
	VkViewport defaultViewport;
} UserData;

/*
	Category template instantiator
 */
typedef Category (*CategoryTemplateInstantiator)(UserData* data);

typedef struct CategoryTemplate
{
	BUFFER instances;							// holds the memory for instantiated instances of object (corresponding to this template category)
	CategoryTemplateInstantiator instantiate;	// instantiator
} CategoryTemplate;


static const char* parse(const char* str, u32 length, callbacks_t* callbacks, u32 categoryRank);
static void begin_category(const char* name, u32 length, void* user_data);
static void end_category(void* user_data);
static void attribute(const char* name, u32 length, const char* value, u32 value_length, void* user_data);
static void field(const char* name, u32 length, const char* value, u32 value_length, void* user_data);
static dictionary_t* create_tree(UserData* data);
static void destroy_tree(dictionary_t* tree, com_allocation_callbacks_t* callbacks);
static void link(UserData* data);
static void setup_default_values(UserData* data);

SC_API void write_gfx_pipeline(const char* start, const char* const end, codegen_buffer_t* buffer)
{
	BUFFER stringLiteralBuffer = buf_create_with_callbacks(buffer->callbacks, sizeof(char*), 1024, 0);	 // 8 KB
	UserData data = {};
	data.callbacks = buffer->callbacks;
	data.categoryStack = buf_create_with_callbacks(data.callbacks, sizeof(Category*), 1, 0);
	AUTO data_writer = codegen_buffer_get_section(buffer, ".data");
	data.mainBaseOffset = buf_get_element_count(CAST_TO(BUFFER*, data_writer->user_data));
	data.mainOutput = CAST_TO(BUFFER*, data_writer->user_data);
	data.baseOffset = data.mainBaseOffset;
	data.output = data.mainOutput;
	data.literalBuffer = &stringLiteralBuffer;
	data.categoryTemplates = dictionary_create(char*, CategoryTemplate, 1, dictionary_key_comparer_string);
	data.tree = create_tree(&data);

	setup_default_values(&data);

	sc_debug_log_info_verbose("base offset: %u\n", data.baseOffset);

	callbacks_t callbacks =
	{
		.begin_category = begin_category,
		.end_category = end_category,
		.field = field,
		.attribute = attribute,
		.user_data = &data
	};
	
	// parse the string
	parse(start, end - start, &callbacks, 0);

	// merge and link the outputs
	link(&data);

	// release heap allocated resources
	destroy_tree(data.tree, data.callbacks);
	u32 template_count = dictionary_get_count(&data.categoryTemplates);
	for(u32 i = 0; i < template_count; i++)
	{
		AUTO template = CAST_TO(CategoryTemplate*, dictionary_get_value_ptr_at(&data.categoryTemplates, i));
		buf_free(&template->instances);
	}
	dictionary_free(&data.categoryTemplates);
	_com_assert(buf_get_element_count(&data.categoryStack) == 0);
	buf_free(&data.categoryStack);
	buf_free(&stringLiteralBuffer);
}


static inline dictionary_t* create_category_dictionary(com_allocation_callbacks_t* callbacks)
{
	dictionary_t* categories = com_call_allocate(callbacks, sizeof(dictionary_t));
	*categories = dictionary_create(char*, Category, 1, dictionary_key_comparer_string);
	return categories;
}
static inline dictionary_t* create_field_dictionary(com_allocation_callbacks_t* callbacks)
{
	dictionary_t* categories = com_call_allocate(callbacks, sizeof(dictionary_t));
	*categories = dictionary_create(char*, Field, 1, dictionary_key_comparer_string);
	return categories;
}

static void begin_category(const char* name, u32 length, void* user_data)
{
	UserData* data = user_data;

	buffer_t str_buffer = buf_create_with_callbacks(data->callbacks, sizeof(char), 64, 0);

	// clear the string buffer
	buf_clear(&str_buffer, NULL);
	// append the category name and priod character
	while(length > 0)
	{
		char ch = *name;
		if((ch != '-') && (ch != '_') && (ch != ' '))
			buf_push_char(&str_buffer, tolower(ch));
		name++;
		--length;
	}
	// add the null character again to complete the string
	buf_push_null(&str_buffer);

	Category* category;
	dictionary_t* categories;

	const char* string = buf_get_ptr(&str_buffer);

	// if starting from root
	if(buf_get_element_count(&data->categoryStack) == 0)
		categories = data->tree;
	// if not starting from root
	else
	{
		// get the category on the top of the stack
		buf_peek(&data->categoryStack, &category);

		// get the categories list inside the current category
		categories = category->categories;
	}

	// find and set the next category to a category with name 'string'
	u64 index = (categories == NULL) ? DICTIONARY_INVALID_INDEX : dictionary_find_index_of(categories, &string);
	if(index == DICTIONARY_INVALID_INDEX)
	{
		// find the template for the category with name 'string'
		index = dictionary_find_index_of(&data->categoryTemplates, &string);
		if(index == DICTIONARY_INVALID_INDEX)
			DEBUG_LOG_ERROR("Unrecognized category \"%s\"\n", string);
		CategoryTemplate* template = dictionary_get_value_ptr_at(&data->categoryTemplates, index);

		// if categoryArrays is null then create new dictionary for it
		if(category->categoryArrays == NULL)
			category->categoryArrays = create_category_dictionary(data->callbacks);

		// switch the output to template->instances
		data->output = &template->instances;
		data->baseOffset = buf_get_element_count(&template->instances);

		// create an instance of the template category
		Category categoryArrayElement = template->instantiate(data);
		categoryArrayElement.isTemplate = true;
		dictionary_push(category->categoryArrays, dictionary_get_key_ptr_at(&data->categoryTemplates, index), &categoryArrayElement);
		// set the catagories to categoryArrays
		categories = category->categoryArrays;
		// set the index to point the last element (just added)
		index = dictionary_get_count(categories) - 1;
	}
	buf_free(&str_buffer);
	category = dictionary_get_value_ptr_at(categories, index);

	// push the found category with name 'string' on top of the stack
	buf_push(&data->categoryStack, &category);
}

static void end_category(void* user_data)
{
	UserData* data = user_data;

	// pop out the current category from the top of the stack
	Category* category;
	buf_pop(&data->categoryStack, &category);

	if(category->isTemplate)
	{
		// reset the output to main output
		data->output = data->mainOutput;
		data->baseOffset = data->mainBaseOffset;
	}
}

static void attribute(const char* name, u32 length, const char* value, u32 value_length, void* user_data)
{
	if(value_length == 0)
		printf("Attribute: %.*s\n", length, name);
	else
		printf("Attribute: %.*s = %.*s\n", length, name, value_length, value);
}

static void field(const char* name, u32 length, const char* value, u32 value_length, void* user_data)
{
	if(value_length == 0) return;
	
	UserData* data = user_data;
	
	buffer_t str_buffer = buf_create_with_callbacks(data->callbacks, sizeof(char), 64, 0);

	// clear the string buffer
	buf_clear(&str_buffer, NULL);
	u32 _length = 0;
	// append the field name
	while(length > 0)
	{
		char ch = *name;
		if((ch != '-') && (ch != '_') && (ch != ' '))
		{
			buf_push_char(&str_buffer, tolower(ch));
			++_length;
		}
		name++;
		--length;
	}
	// add the null character again to complete the string
	buf_push_null(&str_buffer);
	
	char value_string[value_length + 1];
	memcpy(value_string, value, value_length);
	value_string[value_length] = 0;

	// get the current category from the top of the stack
	Category* currentCategory;
	buf_peek(&data->categoryStack, &currentCategory);
	assert(currentCategory->fields != NULL);

	const char* string = buf_get_ptr(&str_buffer);

	// find the field with name 'string' in the current category
	u64 index = dictionary_find_index_of(currentCategory->fields, &string);
	if(index == DICTIONARY_INVALID_INDEX)
		DEBUG_LOG_ERROR("Unrecognized field \"%s\"\n", string);
	buf_free(&str_buffer);
	Field* field = dictionary_get_value_ptr_at(currentCategory->fields, index);
	
	// finally write the field's value to the output buffer
	buf_ucount_t offset = data->baseOffset + field->offset;
	field->write(value_string, buf_get_ptr_at(data->output, offset));
}

static const char* parse(const char* str, u32 length, callbacks_t* callbacks, u32 categoryRank)
{
	assert(str != NULL);
	assert(callbacks != NULL);

	const char* origin = str;
	const char* const end = origin + length;
	str = skip_whitespaces(str, end);

	if(str >= end)
	{
		if(categoryRank != 0)
			DEBUG_LOG_ERROR("A category is not closed, expected '}' before the end of file\n");
		return str;
	}

	u32 len = __get_word_length(str, end, "[]{}=,");
	const char* name = str;
	str += len;
	str = skip_whitespaces(str, end);
	switch(*str)
	{
		// start of an attribute
		case '[':
		{
			break;
		}
		// start of a category
		case '{':
		{
			if(callbacks->begin_category != NULL)
				callbacks->begin_category(name, len, callbacks->user_data);
			categoryRank++;
			break;
		}
		// end of a category
		case '}':
		{
			if((categoryRank > 0) && (len != 0))
			{
				if(callbacks->field != NULL)
					callbacks->field(name, len, NULL, 0, callbacks->user_data);
			}

			if(categoryRank > 0)
			{
				if(callbacks->end_category != NULL)
					callbacks->end_category(callbacks->user_data);
				--categoryRank;
			}
			else
				DEBUG_LOG_ERROR("Unexpected '}', closing brace '}' must match with opening branch '{'\n");			
			break;
		}
		// assignment to a field
		case '=':
		{
			const char* field_name = name;
			++str;
			str = skip_whitespaces(str, end);
			u32 _len = __get_word_length(str, end, ",]");			// \n is already included
			str += _len;

			// attribute
			if((*str == ']') && (callbacks->attribute != NULL))
				callbacks->attribute(field_name, len, str - _len, _len, callbacks->user_data);
			// field
			else if(callbacks->field != NULL)
				callbacks->field(field_name, len, str - _len, _len, callbacks->user_data);
			break;
		}
		// declaration of a field but not assignment
		case ',':
		{
			if(callbacks->field  != NULL)
				callbacks->field(name, len, NULL, 0, callbacks->user_data);
			break;
		}
		case ']':
		{
			if(callbacks->attribute != NULL)
				callbacks->attribute(name, len, NULL, 0, callbacks->user_data);
			break;
		}
		case 0:
		{
			DEBUG_LOG_ERROR("Unexpected end of file\n");
			break;
		}
		default:
		{
			DEBUG_LOG_ERROR("Expected '{', '=' or ',' before \"%.*s\"\n", len, str);
			break;
		}
	}
	++str;
	return parse(str, length - (str - origin), callbacks, categoryRank);
}

static Category create_graphicspipeline_category(UserData* data);
// static Category create_properties_category(UserData* data);
static Category create_inputassembly_category(UserData* data);
static Category create_tessellation_category(UserData* data);
static Category create_viewport_state_category(UserData* data);
static Category create_rasterization_category(UserData* data);
static Category create_multisample_category(UserData* data);
static Category create_depthstencil_category(UserData* data);
static Category create_colorblend_category(UserData* data);
// static Category create_dynamic_category(UserData* data);

#define createStringLiteral(literal) __createStringLiteral(data->literalBuffer, literal)
static char** __createStringLiteral(BUFFER* literalBuffer, const char* literal)
{
	assert(literalBuffer->element_size == 8);
	buf_ucount_t index = buf_get_element_count(literalBuffer);
	buf_push(literalBuffer, (void*)&literal);
	return buf_get_ptr_at(literalBuffer, index);
}

static void setup_default_values(UserData* data)
{
	data->defaultColorBlendAttachmentState = (VkPipelineColorBlendAttachmentState)
	{
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	data->defaultScissor = (VkRect2D)
	{
		.offset = { .x = 0, .y = 0 },
		.extent = { .width = 0, .height = 0 }
	};

	data->defaultViewport = (VkViewport)
	{
		.x = 0, .y = 0,
		.width = 0, .height = 0,
		.minDepth = 0, .maxDepth = 1
	};
}

static category_map_t* create_tree(UserData* data)
{
	category_map_t* categories = create_category_dictionary(data->callbacks);
	Category category = create_inputassembly_category(data);
	dictionary_push(categories, createStringLiteral("inputassembly"), &category);
	category = create_tessellation_category(data);
	dictionary_push(categories, createStringLiteral("tessellation"), &category);
	category = create_viewport_state_category(data);
	dictionary_push(categories, createStringLiteral("viewport"), &category);
	category = create_rasterization_category(data);
	dictionary_push(categories, createStringLiteral("rasterization"), &category);
	category = create_multisample_category(data);
	dictionary_push(categories, createStringLiteral("multisample"), &category);
	category = create_depthstencil_category(data);
	dictionary_push(categories, createStringLiteral("depthstencil"), &category);
	category = create_colorblend_category(data);
	dictionary_push(categories, createStringLiteral("colorblend"), &category);
	buf_ucount_t offset = buf_get_element_count(data->mainOutput);
	buf_push_pseudo(data->mainOutput, sizeof(gfx_pipeline_t));
	
	gfx_pipeline_t* pipeline = buf_get_ptr_at(data->mainOutput, offset);

	pipeline->inputassembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipeline->inputassembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipeline->inputassembly.primitiveRestartEnable = VK_FALSE;

	pipeline->tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

	pipeline->viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	pipeline->rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipeline->rasterization.depthClampEnable = VK_FALSE;
	pipeline->rasterization.rasterizerDiscardEnable = VK_FALSE;
	pipeline->rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	pipeline->rasterization.lineWidth = 1.0f;
	pipeline->rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
	pipeline->rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipeline->rasterization.depthBiasEnable = VK_FALSE;

	pipeline->multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipeline->multisample.sampleShadingEnable = VK_FALSE;
	pipeline->multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	pipeline->depthstencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipeline->depthstencil.depthTestEnable = VK_FALSE;
	pipeline->depthstencil.depthWriteEnable = VK_FALSE;
	pipeline->depthstencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipeline->depthstencil.depthBoundsTestEnable = VK_FALSE;
	pipeline->depthstencil.stencilTestEnable = VK_FALSE;

	pipeline->colorblend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipeline->colorblend.logicOpEnable = VK_FALSE;

	pipeline->dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

	return categories;
}

static void destroy_tree(category_map_t* tree, com_allocation_callbacks_t* callbacks)
{
	u32 cat_count = dictionary_get_count(tree);
	for(u32 i = 0; i < cat_count; i++)
	{
		AUTO category = CAST_TO(Category*, dictionary_get_value_ptr_at(tree, i));
		if(category->fields != NULL)
		{
			dictionary_free(category->fields);
			com_call_deallocate(callbacks, category->fields);
		}
		if(category->categories != NULL)
			destroy_tree(category->categories, callbacks);
		if(category->categoryArrays != NULL)
			destroy_tree(category->categoryArrays, callbacks);
	}
	dictionary_free(tree);
	com_call_deallocate(callbacks, tree);
}


#define duplicate(ptr, type) (type*)__duplicate((void*)(ptr), sizeof(type))
CAN_BE_UNUSED_FUNCTION static void* __duplicate(void* ptr, u32 size)
{
	void* _ptr = malloc(size);
	memcpy(_ptr, ptr, size);
	return _ptr;
}

static void link(UserData* data)
{
	dictionary_t* templates = &data->categoryTemplates;
	
	// at the category templates
	const char* attachmentStr = "attachment";
	const char* scissorStr = "scissor";
	const char* viewportStr = "viewport";
	CategoryTemplate* attachmentTemplate = dictionary_get_value_ptr(templates, &attachmentStr);
	CategoryTemplate* scissorTemplate = dictionary_get_value_ptr(templates, &scissorStr);
	CategoryTemplate* viewportTemplate = dictionary_get_value_ptr(templates, &viewportStr);
	gfx_pipeline_t* pipeline = buf_get_ptr_at(data->mainOutput, data->mainBaseOffset);

	// get the instance counts as defined (explicitly) in the shader
	u32 colorAttachmentCount = buf_get_element_count(&attachmentTemplate->instances) / sizeof(VkPipelineColorBlendAttachmentState);
	u32 scissorCount = buf_get_element_count(&scissorTemplate->instances) / sizeof(VkRect2D);
	u32 viewportCount = buf_get_element_count(&viewportTemplate->instances) / sizeof(VkViewport);

	pipeline->colorblend.attachmentCount = colorAttachmentCount;
	if(colorAttachmentCount != 0)
	{
		// pAttachments would hold the offset of attachments
		pipeline->colorblend.pAttachments = (void*)(buf_get_element_count(data->mainOutput));
		buf_pushv(data->mainOutput, buf_get_ptr(&attachmentTemplate->instances), buf_get_element_count(&attachmentTemplate->instances));
	}

	/* update the pipeline object pointer as the buffer has been resized and the object lies in that buffer */
	pipeline = buf_get_ptr_at(data->mainOutput, data->mainBaseOffset);

	// pScissors would hold the offset of scissors
	pipeline->viewport.pScissors = (void*)(buf_get_element_count(data->mainOutput));
	if(scissorCount != 0)
	{
		u32 size = buf_get_element_count(&scissorTemplate->instances);
		pipeline->viewport.scissorCount = size / sizeof(VkRect2D);
		buf_pushv(data->mainOutput, buf_get_ptr(&scissorTemplate->instances), size);
	}
	else
	{
		pipeline->viewport.scissorCount = 1;
		buf_pushv(data->mainOutput, (void*)&data->defaultScissor, sizeof(VkRect2D));
	}

	/* update the pipeline object pointer as the buffer has been resized and the object lies in that buffer */
	pipeline = buf_get_ptr_at(data->mainOutput, data->mainBaseOffset);

	// pViewports would hold the offset of viewports
	pipeline->viewport.pViewports = (void*)(buf_get_element_count(data->mainOutput));	
	if(viewportCount != 0)
	{
		u32 size = buf_get_element_count(&viewportTemplate->instances);
		pipeline->viewport.viewportCount = size / sizeof(VkViewport);
		buf_pushv(data->mainOutput, buf_get_ptr(&viewportTemplate->instances), size);
	}
	else
	{
		pipeline->viewport.viewportCount = 1;
		buf_pushv(data->mainOutput, (void*)&data->defaultViewport, sizeof(VkViewport));
	}
}

static void write_bool(const char* str, void* output);

CAN_BE_UNUSED_FUNCTION static Category create_graphicspipeline_category(UserData* data)
{
	dictionary_t* categories = create_category_dictionary(data->callbacks);
	Category category = create_inputassembly_category(data);
	dictionary_push(categories, createStringLiteral("inputassembly"), &category);
	category = create_tessellation_category(data);
	dictionary_push(categories, createStringLiteral("tessellation"), &category);
	category = create_viewport_state_category(data);
	dictionary_push(categories, createStringLiteral("viewport"), &category);
	category = create_rasterization_category(data);
	dictionary_push(categories, createStringLiteral("rasterization"), &category);
	category = create_multisample_category(data);
	dictionary_push(categories, createStringLiteral("multisample"), &category);
	category = create_depthstencil_category(data);
	dictionary_push(categories, createStringLiteral("depthstencil"), &category);
	category = create_colorblend_category(data);
	dictionary_push(categories, createStringLiteral("colorblend"), &category);
	// category = create_dynamic_category(data);
	// dictionary_push(categories, createStringLiteral("dynamic"), &category);
	return (Category) { .fields = NULL, .categories = categories };
}

static void write_VkStencilOp(const char* str, void* output);
static void write_float(const char* str, void* output);
static void write_VkBlendFactor(const char* str, void* output);
static void write_VkBool32(const char* str, void* output);
static void write_uint32_t(const char* str, void* output);
static void write_int32_t(const char* str, void* output);
static void write_VkCompareOp(const char* str, void* output);
static void write_VkLogicOp(const char* str, void* output);
static void write_VkBlendOp(const char* str, void* output);
static void write_VkPrimitiveTopology(const char* str, void* output);
static void write_VkCullModeFlags(const char* str, void* output);
static void write_VkSampleCountFlagBits(const char* str, void* output);
static void write_VkPolygonMode(const char* str, void* output);
static void write_VkFrontFace(const char* str, void* output);
static void write_VkColorComponentFlags(const char* str, void* output);

static Category create_inputassembly_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
	Field field = { offsetof(gfx_pipeline_t, inputassembly.topology), write_VkPrimitiveTopology };
	dictionary_push(fields, createStringLiteral("topology"), &field);
	field = (Field) { offsetof(gfx_pipeline_t, inputassembly.primitiveRestartEnable), write_VkBool32 };
	dictionary_push(fields, createStringLiteral("primitiverestartenable"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_tessellation_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
	Field field = { offsetof(gfx_pipeline_t, tessellation.patchControlPoints), write_uint32_t };
	dictionary_push(fields, createStringLiteral("patchcontrolpoints"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_offset2d_category(UserData* data, u32 offset)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offset + offsetof(VkOffset2D, x), write_int32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "x"), &field);
    field = (Field) { offset + offsetof(VkOffset2D, y), write_int32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "y"), &field);
	return (Category) { .fields = fields, .categories = NULL };	
}

static Category create_extent2d_category(UserData* data, u32 offset)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offset + offsetof(VkExtent2D, width), write_int32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "width"), &field);
    field = (Field) { offset + offsetof(VkExtent2D, height), write_int32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "height"), &field);
	return (Category) { .fields = fields, .categories = NULL };	
}

/* template */
static Category create_scissor_category(UserData* data)
{
	buf_ucount_t offset = 0;
	buf_ucount_t _offset = buf_get_element_count(data->output);
	buf_push_pseudo(data->output, sizeof(VkRect2D));
	memcpy(buf_get_ptr_at(data->output, _offset), &data->defaultScissor, sizeof(VkRect2D));
	dictionary_t* categories = create_category_dictionary(data->callbacks);
	Category category = create_offset2d_category(data, offset);
	dictionary_push(categories, createStringLiteral("offset"), &category);
	category = create_extent2d_category(data, offset + offsetof(VkRect2D, extent));
	dictionary_push(categories, createStringLiteral("extent"), &category);
	return (Category) { .fields = NULL, .categories = categories };
}

/* template */
static Category create_viewport_category(UserData* data)
{
	buf_ucount_t offset = 0;
	buf_ucount_t _offset = buf_get_element_count(data->output);
	buf_push_pseudo(data->output, sizeof(VkViewport));
	memcpy(buf_get_ptr_at(data->output, _offset), &data->defaultViewport, sizeof(VkViewport));
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offset + offsetof(VkViewport, x), write_float };
    dictionary_push(fields, createStringLiteral("x"), &field);
    field = (Field) { offset + offsetof(VkViewport, y), write_float };
    dictionary_push(fields, createStringLiteral("y"), &field);
    field = (Field) { offset + offsetof(VkViewport, width), write_float };
    dictionary_push(fields, createStringLiteral("width"), &field);
    field = (Field) { offset + offsetof(VkViewport, height), write_float };
    dictionary_push(fields, createStringLiteral("height"), &field);
    field = (Field) { offset + offsetof(VkViewport, minDepth), write_float };
    dictionary_push(fields, createStringLiteral("mindepth"), &field);
    field = (Field) { offset + offsetof(VkViewport, maxDepth), write_float };
    dictionary_push(fields, createStringLiteral("maxdepth"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_viewport_state_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
	Field field = { offsetof(gfx_pipeline_t, viewport.viewportCount), write_uint32_t };
	dictionary_push(fields, createStringLiteral("viewportcount"), &field);
	field = (Field) { offsetof(gfx_pipeline_t, viewport.scissorCount), write_uint32_t };
	dictionary_push(fields, createStringLiteral("scissorcount"), &field);

    CategoryTemplate viewportTemplate = 
    {
    	.instantiate = create_viewport_category,
    	.instances = buf_new_with_callbacks(data->callbacks, char)
    };
    dictionary_push(&data->categoryTemplates, createStringLiteral("viewport"), &viewportTemplate);
    
    CategoryTemplate scissorTemplate = 
    {
    	.instantiate = create_scissor_category,
    	.instances = buf_new_with_callbacks(data->callbacks, char)
    };
    dictionary_push(&data->categoryTemplates, createStringLiteral("scissor"), &scissorTemplate);
	
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_rasterization_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offsetof(gfx_pipeline_t, rasterization.depthClampEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("depthclampenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.rasterizerDiscardEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("rasterizerdiscardenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.polygonMode), write_VkPolygonMode };
    dictionary_push(fields, createStringLiteral("polygonmode"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.cullMode), write_VkCullModeFlags };
    dictionary_push(fields, createStringLiteral("cullmode"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.frontFace), write_VkFrontFace };
    dictionary_push(fields, createStringLiteral("frontface"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.depthBiasEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("depthbiasenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.depthBiasConstantFactor), write_float };
    dictionary_push(fields, createStringLiteral("depthbiasconstantfactor"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.depthBiasClamp), write_float };
    dictionary_push(fields, createStringLiteral("depthbiasclamp"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.depthBiasSlopeFactor), write_float };
    dictionary_push(fields, createStringLiteral("depthbiasslopefactor"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, rasterization.lineWidth), write_float };
    dictionary_push(fields, createStringLiteral("linewidth"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_multisample_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offsetof(gfx_pipeline_t, multisample.rasterizationSamples), write_VkSampleCountFlagBits };
    dictionary_push(fields, createStringLiteral("rasterizationsamples"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, multisample.sampleShadingEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("sampleshadingenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, multisample.minSampleShading), write_float };
    dictionary_push(fields, createStringLiteral("minsampleshading"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, multisample.alphaToCoverageEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("alphatocoverageenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, multisample.alphaToOneEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("alphatooneenable"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_stencil_op_category(UserData* data, u32 offset);

static Category create_depthstencil_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offsetof(gfx_pipeline_t, depthstencil.depthTestEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("depthtestenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.depthWriteEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("depthwriteenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.depthCompareOp), write_VkCompareOp };
    dictionary_push(fields, createStringLiteral("depthcompareop"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.depthBoundsTestEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("depthboundstestenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.stencilTestEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("stenciltestenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.minDepthBounds), write_float };
    dictionary_push(fields, createStringLiteral("mindepthbounds"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, depthstencil.maxDepthBounds), write_float };
    dictionary_push(fields, createStringLiteral("maxdepthbounds"), &field);

    dictionary_t* categories = create_category_dictionary(data->callbacks);
    Category category = create_stencil_op_category(data, offsetof(gfx_pipeline_t, depthstencil.front));
    dictionary_push(categories, createStringLiteral("front"), &category);
    category = create_stencil_op_category(data, offsetof(gfx_pipeline_t, depthstencil.back));
    dictionary_push(categories, createStringLiteral("back"), &category);
	return (Category) { .fields = fields, .categories = categories };
}

static Category create_stencil_op_category(UserData* data, u32 offset)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offset + offsetof(VkStencilOpState, failOp), write_VkStencilOp };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "failop"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, passOp), write_VkStencilOp };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "passop"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, depthFailOp), write_VkStencilOp };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "depthfailop"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, compareOp), write_VkCompareOp };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "compareop"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, compareMask), write_uint32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "comparemask"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, writeMask), write_uint32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "writemask"), &field);
    field = (Field) { offset + offsetof(VkStencilOpState, reference), write_uint32_t };
    dictionary_push(fields, __createStringLiteral(data->literalBuffer, "reference"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}

static Category create_colorattachment_category(UserData* data);
static Category create_blendconstants_category(UserData* data);

static Category create_colorblend_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offsetof(gfx_pipeline_t, colorblend.logicOpEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("logicopenable"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, colorblend.logicOp), write_VkLogicOp };
    dictionary_push(fields, createStringLiteral("logicop"), &field);
    field = (Field) { offsetof(gfx_pipeline_t, colorblend.attachmentCount), write_uint32_t };
    dictionary_push(fields, createStringLiteral("attachmentcount"), &field);

    CategoryTemplate colorAttachmentTemplate = 
    {
    	.instantiate = create_colorattachment_category,
    	.instances = buf_new_with_callbacks(data->callbacks, char)
    };
    dictionary_push(&data->categoryTemplates, createStringLiteral("attachment"), &colorAttachmentTemplate);

    dictionary_t* categories = create_category_dictionary(data->callbacks);
    Category category = create_blendconstants_category(data);
    dictionary_push(categories, createStringLiteral("blendconstants"), &category);
	return (Category) { .fields = fields, .categories = categories };
}

static Category create_blendconstants_category(UserData* data)
{
	dictionary_t* fields = create_field_dictionary(data->callbacks);
	Field field = { offsetof(gfx_pipeline_t, colorblend.blendConstants[0]), write_float };
	dictionary_push(fields, createStringLiteral("r"), &field);
	field = (Field) { offsetof(gfx_pipeline_t, colorblend.blendConstants[1]), write_float };
	dictionary_push(fields, createStringLiteral("g"), &field);
	field = (Field) { offsetof(gfx_pipeline_t, colorblend.blendConstants[2]), write_float };
	dictionary_push(fields, createStringLiteral("b"), &field);
	field = (Field) { offsetof(gfx_pipeline_t, colorblend.blendConstants[3]), write_float };
	dictionary_push(fields, createStringLiteral("a"), &field);
	return (Category) { .fields = fields, .categories = NULL };	
}

static Category create_colorattachment_category(UserData* data)
{
	buf_ucount_t offset = 0;
	buf_ucount_t _offset = buf_get_element_count(data->output);
	buf_push_pseudo(data->output, sizeof(VkPipelineColorBlendAttachmentState));
	memcpy(buf_get_ptr_at(data->output, _offset), &data->defaultColorBlendAttachmentState, sizeof(VkPipelineColorBlendAttachmentState));
	dictionary_t* fields = create_field_dictionary(data->callbacks);
    Field field = { offset + offsetof(VkPipelineColorBlendAttachmentState, blendEnable), write_VkBool32 };
    dictionary_push(fields, createStringLiteral("blendenable"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, srcColorBlendFactor), write_VkBlendFactor };
    dictionary_push(fields, createStringLiteral("srccolorblendfactor"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, dstColorBlendFactor), write_VkBlendFactor };
    dictionary_push(fields, createStringLiteral("dstcolorblendfactor"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, colorBlendOp), write_VkBlendOp };
    dictionary_push(fields, createStringLiteral("colorblendop"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, srcAlphaBlendFactor), write_VkBlendFactor };
    dictionary_push(fields, createStringLiteral("srcalphablendfactor"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, dstAlphaBlendFactor), write_VkBlendFactor };
    dictionary_push(fields, createStringLiteral("dstalphablendfactor"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, alphaBlendOp), write_VkBlendOp };
    dictionary_push(fields, createStringLiteral("alphablendop"), &field);
    field = (Field) { offset + offsetof(VkPipelineColorBlendAttachmentState, colorWriteMask), write_VkColorComponentFlags };
    dictionary_push(fields, createStringLiteral("colorwritemask"), &field);
	return (Category) { .fields = fields, .categories = NULL };
}


static void write_float(const char* str, void* output)
{
	float value = strtof(str, NULL);
	memcpy(output, &value, sizeof(value));
}

static void write_VkBool32(const char* str, void* output)
{
	assert(sizeof(VkBool32) == 4);
	VkBool32 value;
	if(strcmp(str, "true") == 0)
		value = VK_TRUE;
	else if(strcmp(str, "false") == 0)
		value = VK_FALSE;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkBool32\n", str);
	memcpy(output, &value, sizeof(value));
}

static void write_int32_t(const char* str, void* output)
{
	s32 value = strtol(str, NULL, 0);
	memcpy(output, &value, sizeof(value));
}

static void write_uint32_t(const char* str, void* output)
{
	u32 value = strtoul(str, NULL, 0);
	memcpy(output, &value, sizeof(value));
}

// TODO: Use hashing
static void write_VkBlendFactor(const char* str, void* output)
{
	assert(sizeof(VkBlendFactor) == 4);
	VkBlendFactor value;
    if(strcmp(str, "zero") == 0)
    	value = VK_BLEND_FACTOR_ZERO;
    else if(strcmp(str, "one") == 0)
    		value = VK_BLEND_FACTOR_ONE;
    else if(strcmp(str, "srccolor") == 0)
    		value = VK_BLEND_FACTOR_SRC_COLOR;
    else if(strcmp(str, "oneminussrccolor") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    else if(strcmp(str, "dstcolor") == 0)
    		value = VK_BLEND_FACTOR_DST_COLOR;
    else if(strcmp(str, "oneminusdstcolor") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    else if(strcmp(str, "srcalpha") == 0)
    		value = VK_BLEND_FACTOR_SRC_ALPHA;
    else if(strcmp(str, "oneminussrcalpha") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    else if(strcmp(str, "dstalpha") == 0)
    		value = VK_BLEND_FACTOR_DST_ALPHA;
    else if(strcmp(str, "oneminusdstalpha") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    else if(strcmp(str, "constantcolor") == 0)
    		value = VK_BLEND_FACTOR_CONSTANT_COLOR;
    else if(strcmp(str, "oneminusconstantcolor") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    else if(strcmp(str, "constantalpha") == 0)
    		value = VK_BLEND_FACTOR_CONSTANT_ALPHA;
    else if(strcmp(str, "oneminusconstantalpha") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    else if(strcmp(str, "srcalphasaturate") == 0)
    		value = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    else if(strcmp(str, "src1color") == 0)
    		value = VK_BLEND_FACTOR_SRC1_COLOR;
    else if(strcmp(str, "oneminussrc1color") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    else if(strcmp(str, "src1alpha") == 0)
    		value = VK_BLEND_FACTOR_SRC1_ALPHA;
    else if(strcmp(str, "oneminussrc1alpha") == 0)
    		value = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkBlendFactor\n", str);
	memcpy(output, &value, sizeof(value));
};

// TODO: Use hashing
static void write_VkStencilOp(const char* str, void* output)
{
	assert(sizeof(VkStencilOp) == 4);
	VkStencilOp value;
	if(strcmp(str, "keep") == 0)
		value = VK_STENCIL_OP_KEEP;
    else if(strcmp(str, "zero") == 0)
    	value = VK_STENCIL_OP_ZERO;
    else if(strcmp(str, "replace") == 0)
    	value = VK_STENCIL_OP_REPLACE;
    else if(strcmp(str, "incrementandclamp") == 0)
    	value = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    else if(strcmp(str, "decrementandclamp") == 0)
    	value = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    else if(strcmp(str, "invert") == 0)
    	value = VK_STENCIL_OP_INVERT;
    else if(strcmp(str, "incrementandwrap") == 0)
    	value = VK_STENCIL_OP_INCREMENT_AND_WRAP;
    else if(strcmp(str, "decrementandwrap") == 0)
    	value = VK_STENCIL_OP_DECREMENT_AND_WRAP;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkStencilOp\n", str);
	memcpy(output, &value, sizeof(value));
}

// TODO: Use hashing
static void write_VkBlendOp(const char* str, void* output)
{
	assert(sizeof(VkBlendOp) == 4);
	VkBlendOp value;
    if(strcmp(str, "add") == 0)
    	value = VK_BLEND_OP_ADD;
    else if(strcmp(str, "subtract") == 0)
    	value = VK_BLEND_OP_SUBTRACT;
    else if(strcmp(str, "reversesubtract") == 0)
    	value = VK_BLEND_OP_REVERSE_SUBTRACT;
    else if(strcmp(str, "min") == 0)
    	value = VK_BLEND_OP_MIN;
    else if(strcmp(str, "max") == 0)
    	value = VK_BLEND_OP_MAX;
    else if(strcmp(str, "zeroext") == 0)
    	value = VK_BLEND_OP_ZERO_EXT;
    else if(strcmp(str, "srcext") == 0)
    	value = VK_BLEND_OP_SRC_EXT;
    else if(strcmp(str, "dstext") == 0)
    	value = VK_BLEND_OP_DST_EXT;
    else if(strcmp(str, "srcoverext") == 0)
    	value = VK_BLEND_OP_SRC_OVER_EXT;
    else if(strcmp(str, "dstoverext") == 0)
    	value = VK_BLEND_OP_DST_OVER_EXT;
    else if(strcmp(str, "srcinext") == 0)
    	value = VK_BLEND_OP_SRC_IN_EXT;
    else if(strcmp(str, "dstinext") == 0)
    	value = VK_BLEND_OP_DST_IN_EXT;
    else if(strcmp(str, "srcoutext") == 0)
    	value = VK_BLEND_OP_SRC_OUT_EXT;
    else if(strcmp(str, "dstoutext") == 0)
    	value = VK_BLEND_OP_DST_OUT_EXT;
    else if(strcmp(str, "srcatopext") == 0)
    	value = VK_BLEND_OP_SRC_ATOP_EXT;
    else if(strcmp(str, "dstatopext") == 0)
    	value = VK_BLEND_OP_DST_ATOP_EXT;
    else if(strcmp(str, "xorext") == 0)
    	value = VK_BLEND_OP_XOR_EXT;
    else if(strcmp(str, "multiplyext") == 0)
    	value = VK_BLEND_OP_MULTIPLY_EXT;
    else if(strcmp(str, "screenext") == 0)
    	value = VK_BLEND_OP_SCREEN_EXT;
    else if(strcmp(str, "overlayext") == 0)
    	value = VK_BLEND_OP_OVERLAY_EXT;
    else if(strcmp(str, "darkenext") == 0)
    	value = VK_BLEND_OP_DARKEN_EXT;
    else if(strcmp(str, "lightenext") == 0)
    	value = VK_BLEND_OP_LIGHTEN_EXT;
    else if(strcmp(str, "colordodgeext") == 0)
    	value = VK_BLEND_OP_COLORDODGE_EXT;
    else if(strcmp(str, "colorburnext") == 0)
    	value = VK_BLEND_OP_COLORBURN_EXT;
    else if(strcmp(str, "hardlightext") == 0)
    	value = VK_BLEND_OP_HARDLIGHT_EXT;
    else if(strcmp(str, "softlightext") == 0)
    	value = VK_BLEND_OP_SOFTLIGHT_EXT;
    else if(strcmp(str, "differenceext") == 0)
    	value = VK_BLEND_OP_DIFFERENCE_EXT;
    else if(strcmp(str, "exclusionext") == 0)
    	value = VK_BLEND_OP_EXCLUSION_EXT;
    else if(strcmp(str, "invertext") == 0)
    	value = VK_BLEND_OP_INVERT_EXT;
    else if(strcmp(str, "invertrgbext") == 0)
    	value = VK_BLEND_OP_INVERT_RGB_EXT;
    else if(strcmp(str, "lineardodgeext") == 0)
    	value = VK_BLEND_OP_LINEARDODGE_EXT;
    else if(strcmp(str, "linearburnext") == 0)
    	value = VK_BLEND_OP_LINEARBURN_EXT;
    else if(strcmp(str, "vividlightext") == 0)
    	value = VK_BLEND_OP_VIVIDLIGHT_EXT;
    else if(strcmp(str, "linearlightext") == 0)
    	value = VK_BLEND_OP_LINEARLIGHT_EXT;
    else if(strcmp(str, "pinlightext") == 0)
    	value = VK_BLEND_OP_PINLIGHT_EXT;
    else if(strcmp(str, "hardmixext") == 0)
    	value = VK_BLEND_OP_HARDMIX_EXT;
    else if(strcmp(str, "hslhueext") == 0)
    	value = VK_BLEND_OP_HSL_HUE_EXT;
    else if(strcmp(str, "hslsaturationext") == 0)
    	value = VK_BLEND_OP_HSL_SATURATION_EXT;
    else if(strcmp(str, "hslcolorext") == 0)
    	value = VK_BLEND_OP_HSL_COLOR_EXT;
    else if(strcmp(str, "hslluminosityext") == 0)
    	value = VK_BLEND_OP_HSL_LUMINOSITY_EXT;
    else if(strcmp(str, "plusext") == 0)
    	value = VK_BLEND_OP_PLUS_EXT;
    else if(strcmp(str, "plusclampedext") == 0)
    	value = VK_BLEND_OP_PLUS_CLAMPED_EXT;
    else if(strcmp(str, "plusclampedalphaext") == 0)
    	value = VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
    else if(strcmp(str, "plusdarkerext") == 0)
    	value = VK_BLEND_OP_PLUS_DARKER_EXT;
    else if(strcmp(str, "minusext") == 0)
    	value = VK_BLEND_OP_MINUS_EXT;
    else if(strcmp(str, "minusclampedext") == 0)
    	value = VK_BLEND_OP_MINUS_CLAMPED_EXT;
    else if(strcmp(str, "contrastext") == 0)
    	value = VK_BLEND_OP_CONTRAST_EXT;
    else if(strcmp(str, "invertovgext") == 0)
    	value = VK_BLEND_OP_INVERT_OVG_EXT;
    else if(strcmp(str, "redext") == 0)
    	value = VK_BLEND_OP_RED_EXT;
    else if(strcmp(str, "greenext") == 0)
    	value = VK_BLEND_OP_GREEN_EXT;
    else if(strcmp(str, "blueext") == 0)
    	value = VK_BLEND_OP_BLUE_EXT;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkBlendOp\n", str);
	memcpy(output, &value, sizeof(value));  
}

static void write_VkCompareOp(const char* str, void* output)
{
	assert(sizeof(VkCompareOp) == 4);
	VkCompareOp value;
    if(strcmp(str, "never") == 0)
    	value = VK_COMPARE_OP_NEVER;
    else if(strcmp(str, "less") == 0)
    	value = VK_COMPARE_OP_LESS;
    else if(strcmp(str, "equal") == 0)
    	value = VK_COMPARE_OP_EQUAL;
    else if(strcmp(str, "lessorequal") == 0)
    	value = VK_COMPARE_OP_LESS_OR_EQUAL;
    else if(strcmp(str, "greater") == 0)
    	value = VK_COMPARE_OP_GREATER;
    else if(strcmp(str, "notequal") == 0)
    	value = VK_COMPARE_OP_NOT_EQUAL;
    else if(strcmp(str, "greaterorequal") == 0)
    	value = VK_COMPARE_OP_GREATER_OR_EQUAL;
    else if(strcmp(str, "always") == 0)
    	value = VK_COMPARE_OP_ALWAYS;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkCompareOp\n", str);
	memcpy(output, &value, sizeof(value)); 
}

static void write_VkLogicOp(const char* str, void* output)
{
	assert(sizeof(VkLogicOp) == 4);
	VkLogicOp value;
    if(strcmp(str, "clear") == 0)
    	value = VK_LOGIC_OP_CLEAR;
    else if(strcmp(str, "and") == 0)
    	value = VK_LOGIC_OP_AND;
    else if(strcmp(str, "andreverse") == 0)
    	value = VK_LOGIC_OP_AND_REVERSE;
    else if(strcmp(str, "copy") == 0)
    	value = VK_LOGIC_OP_COPY;
    else if(strcmp(str, "andinverted") == 0)
    	value = VK_LOGIC_OP_AND_INVERTED;
    else if(strcmp(str, "noop") == 0)
    	value = VK_LOGIC_OP_NO_OP;
    else if(strcmp(str, "xor") == 0)
    	value = VK_LOGIC_OP_XOR;
    else if(strcmp(str, "or") == 0)
    	value = VK_LOGIC_OP_OR;
    else if(strcmp(str, "nor") == 0)
    	value = VK_LOGIC_OP_NOR;
    else if(strcmp(str, "equivalent") == 0)
    	value = VK_LOGIC_OP_EQUIVALENT;
    else if(strcmp(str, "invert") == 0)
    	value = VK_LOGIC_OP_INVERT;
    else if(strcmp(str, "orreverse") == 0)
    	value = VK_LOGIC_OP_OR_REVERSE;
    else if(strcmp(str, "copyinverted") == 0)
    	value = VK_LOGIC_OP_COPY_INVERTED;
    else if(strcmp(str, "orinverted") == 0)
    	value = VK_LOGIC_OP_OR_INVERTED;
    else if(strcmp(str, "nand") == 0)
    	value = VK_LOGIC_OP_NAND;
    else if(strcmp(str, "set") == 0)
    	value = VK_LOGIC_OP_SET;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkLogicOp\n", str);
	memcpy(output, &value, sizeof(value)); 
}

static void write_VkPrimitiveTopology(const char* str, void* output)
{
	assert(sizeof(VkPrimitiveTopology) == 4);
	VkPrimitiveTopology value;
    if(strcmp(str, "pointlist") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    else if(strcmp(str, "linelist") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if(strcmp(str, "linestrip") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    else if(strcmp(str, "trianglelist") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    else if(strcmp(str, "trianglestrip") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    else if(strcmp(str, "trianglefan") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    else if(strcmp(str, "linelistwithadjacency") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
    else if(strcmp(str, "linestripwithadjacency") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
    else if(strcmp(str, "trianglelistwithadjacency") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
    else if(strcmp(str, "trianglestripwithadjacency") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
    else if(strcmp(str, "patchlist") == 0) 
    	value = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkPrimitiveTopology\n", str);
	memcpy(output, &value, sizeof(value));
}

static void write_VkPolygonMode(const char* str, void* output)
{
	assert(sizeof(VkPolygonMode) == 4);
	VkPolygonMode value;
    if(strcmp(str, "fill") == 0)
    	value = VK_POLYGON_MODE_FILL;
    else if(strcmp(str, "line") == 0)
    	value = VK_POLYGON_MODE_LINE;
    else if(strcmp(str, "point") == 0)
    	value = VK_POLYGON_MODE_POINT;
    else if(strcmp(str, "fillrectanglenv") == 0)
    	value = VK_POLYGON_MODE_FILL_RECTANGLE_NV;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkPolygonMode\n", str);
	memcpy(output, &value, sizeof(value));
}

// TODO: Make it customizable
static void write_VkCullModeFlags(const char* str, void* output)
{
	assert(sizeof(VkCullModeFlagBits) == 4);
	VkCullModeFlags value = VK_CULL_MODE_BACK_BIT;

	// else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkCullModeFlagBits\n", str);
	memcpy(output, &value, sizeof(value));
}

static void write_VkFrontFace(const char* str, void* output)
{
	assert(sizeof(VkFrontFace) == 4);
	VkFrontFace value;
    if(strcmp(str, "counterclockwise") == 0)
    	value = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    else if(strcmp(str, "clockwise") == 0)
    	value = VK_FRONT_FACE_CLOCKWISE;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkFrontFace\n", str);
	memcpy(output, &value, sizeof(value));
}

// TODO: Make it customizable
static void write_VkSampleCountFlagBits(const char* str, void* output)
{
	assert(sizeof(VkSampleCountFlagBits) == 4);
	VkSampleCountFlagBits value = VK_SAMPLE_COUNT_1_BIT;

	// else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkSampleCountFlagBits\n", str);
	memcpy(output, &value, sizeof(value));
}


static void write_VkColorComponentFlags(const char* str, void* output)
{
	assert(sizeof(VkColorComponentFlags) == 4);
	VkColorComponentFlags value = VK_COLOR_COMPONENT_R_BIT
								| VK_COLOR_COMPONENT_G_BIT
								| VK_COLOR_COMPONENT_B_BIT
								| VK_COLOR_COMPONENT_A_BIT;

	// else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type VkColorComponentFlags\n", str);
	memcpy(output, &value, sizeof(value));	
}

CAN_BE_UNUSED_FUNCTION static void write_bool(const char* str, void* output)
{
	assert(sizeof(bool) == 1);
	bool value;
	if(strcmp(str, "true") == 0)
		value = true;
	else if(strcmp(str, "false") == 0)
		value = false;
	else DEBUG_LOG_ERROR("Invalid value \"%s\" has been assigned to a variable of type bool\n", str);
	memcpy(output, &value, 1);
}
