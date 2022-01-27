
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_stage_shader.h>
#include <renderer/defines.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <memory_allocator/memory_allocator.h>
#include <shader_compiler/compiler.h>
#include <disk_manager/file_reader.h>
#include <string.h>

vulkan_shader_t* vulkan_shader_new()
{
	vulkan_shader_t* shader = heap_new(vulkan_shader_t);
	memset(shader, 0, sizeof(vulkan_shader_t));
	return shader;
}
static VkDescriptorSetLayout get_vulkan_descriptor_set_layout(renderer_t* renderer, vulkan_shader_resource_descriptor_t* descriptors, u32 binding_count);
static vulkan_stage_shader_t** create_stage_shaders(renderer_t* renderer, BUFFER* shader_binary, u32 cursor, u8* out_stage_count);
static vulkan_shader_resource_descriptor_t* create_descriptors(BUFFER* shader_binary, u32 cursor, u16* out_descriptor_count);
static u16 sizeof_glsl_type(u8 type);
static u16 alignof_glsl_type(u8 type);

vulkan_shader_t* vulkan_shader_create(renderer_t* renderer, BUFFER* shader_binary)
{
	const char* header = "SHADER BINARY"; u8 header_len = strlen(header);
	char buffer[header_len];
	buf_ucount_t cursor = 0;
	for(; cursor < header_len; cursor++) buf_get_at(shader_binary, cursor, &buffer[cursor]);
	if(strncmp(buffer, header, header_len) != 0)
		LOG_FETAL_ERR("Shader binary loading error: invalid header \"%.*s\"\n", header_len, buffer);

	u32 shader_offset = 0xFFFFFFFF; //invalid
	u32 layout_offset = 0xFFFFFFFF; //invalid
	bool shader_found = false;
	bool layout_found = false;

	const u32 section_mask = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
	const u32 MASK = 0x00000007UL;
	for(u8 i = 0; i < 3; i++)
	{
		if((!shader_found) && (section_mask & ((1UL << 2) << (i * 3))))
		{
			shader_offset = *(u32*)buf_get_ptr_at(shader_binary, cursor);
			shader_found = true;
		}
		else if((!layout_found) && (section_mask & ((1UL << 1) << (i * 3))))
		{
			layout_offset = *(u32*)buf_get_ptr_at(shader_binary, cursor);
			layout_found = true;
		}
		if(section_mask & (MASK << (i * 3)))
			cursor += 4;
	}
	if(!shader_found)
		LOG_FETAL_ERR("Shader binary loading error: SHADER section not found!\n");
	if(!layout_found)
		LOG_WRN("Shader binary loading warning: LAYOUT section not found!\n");

	vulkan_shader_t* shader = vulkan_shader_new();
	shader->stage_shaders = create_stage_shaders(renderer, shader_binary, shader_offset, &shader->stage_count);
	if(layout_found)
	{
		shader->descriptors = create_descriptors(shader_binary, layout_offset, &shader->descriptor_count);
		shader->vk_set_layout = get_vulkan_descriptor_set_layout(renderer, shader->descriptors, shader->descriptor_count);
		vulkan_descriptor_set_create_info_t set_create_info =
		{
			.pool = renderer->vk_descriptor_pool,
			.layout = shader->vk_set_layout
		};
		if(shader->vk_set_layout != VK_NULL_HANDLE)
			shader->vk_set = vulkan_descriptor_set_create(renderer, &set_create_info);
	}
	return shader;
}

vulkan_shader_t* vulkan_shader_load_and_create(renderer_t* renderer, const char* file_path)
{
	BUFFER* shader_binary = load_binary_from_file(file_path);
	vulkan_shader_t* shader = vulkan_shader_create(renderer, shader_binary);
	buf_free(shader_binary);
	return shader;
}

void vulkan_shader_destroy(vulkan_shader_t* shader, renderer_t* renderer)
{
	for(u8 i = 0; i < shader->stage_count; i++)
		vulkan_stage_shader_destroy(ref(vulkan_stage_shader_t*, shader->stage_shaders, i), renderer);
	if(shader->vk_set_layout != VK_NULL_HANDLE)
		vkDestroyDescriptorSetLayout(renderer->vk_device, shader->vk_set_layout, NULL);
	if(shader->vk_set != NULL)
		vulkan_descriptor_set_destroy(shader->vk_set, renderer);
}

void vulkan_shader_release_resources(vulkan_shader_t* shader)
{
	if(shader->descriptors != NULL)
		heap_free(shader->descriptors);
	heap_free(shader->stage_shaders);
	if(shader->vk_set != NULL)
		vulkan_descriptor_set_release_resources(shader->vk_set);
	heap_free(shader);
}

static vulkan_stage_shader_t** create_stage_shaders(renderer_t* renderer, BUFFER* shader_binary, u32 cursor, u8* stage_count)
{
	assert(cursor != 0xFFFFFFFF);
	u8 shader_mask = *(u8*)buf_get_ptr_at(shader_binary, cursor); cursor++;
	u8 shader_count = 0; for(u8 i = 0; i < 4; i++) { if(shader_mask & (1 << i)) shader_count++; }
	*stage_count = shader_count;

	vulkan_stage_shader_t** shaders = heap_newv(vulkan_stage_shader_t*, shader_count);
	for(u8 i = 0, count = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
	{
		u8 bit = shader_mask & (1 << i);
		if(bit == 0) continue;
		u32 offset = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
		u32 length = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
		switch(bit)
		{
			case 1 << SHADER_COMPILER_SHADER_STAGE_VERTEX:
				//Vertex shader spirv
				u32* spirv = buf_get_ptr_at(shader_binary, offset);
				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_VERTEX);
				count++;
				break;
			case 1 << SHADER_COMPILER_SHADER_STAGE_TESSELLATION:
				//Tessellation shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_TESSELLATION);
				count++;
			break;
			case 1 << SHADER_COMPILER_SHADER_STAGE_GEOMETRY:
				//Geometry shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_GEOMETRY);
				count++;
			break;
			case 1 << SHADER_COMPILER_SHADER_STAGE_FRAGMENT:
				//Fragment shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_FRAGMENT);
				count++;
			break;
			default:
				LOG_FETAL_ERR("Shader binary loading error: Invalid shader bit \"%u\" in shader_mask\n", bit);
			break;
		}
	}
	return shaders;
}

static vulkan_shader_resource_descriptor_t* create_descriptors(BUFFER* shader_binary, u32 cursor, u16* out_descriptor_count)
{
	assert(cursor != 0xFFFFFFFF);
	u16 descriptor_count = *(u16*)buf_get_ptr_at(shader_binary, cursor); cursor += 2;
	*out_descriptor_count = descriptor_count;
	if(descriptor_count == 0)
	{
		assert_wrn(descriptor_count != 0);
		return NULL;
	}
	vulkan_shader_resource_descriptor_t* descriptors = heap_newv(vulkan_shader_resource_descriptor_t, descriptor_count);

	BUFFER fields = buf_create(sizeof(struct_field_t), 0, 0);
	buf_ucount_t* indices = stack_newv(buf_ucount_t, descriptor_count);
	for(u16 i = 0; i < descriptor_count; i++) indices[i] = BUF_INVALID_INDEX;
	for(u16 i = 0; i < descriptor_count; i++, cursor += 4)
	{
		u32 offset = *(u32*)buf_get_ptr_at(shader_binary, cursor);
		u32 temp_cursor = offset;
		vulkan_shader_resource_descriptor_t* descriptor = refp(vulkan_shader_resource_descriptor_t, descriptors, i);
		memset(&descriptor->handle, 0, sizeof(struct_descriptor_t));
		descriptor->set_number = *(u8*)buf_get_ptr_at(shader_binary, temp_cursor); temp_cursor += 1;
		descriptor->binding_number = *(u8*)buf_get_ptr_at(shader_binary, temp_cursor); temp_cursor += 1;
		// assert(descriptor->set_number < 255);
		// assert(descriptor->binding_number < 255);
		u32 descriptor_info = *(u32*)buf_get_ptr_at(shader_binary, temp_cursor); temp_cursor += 4;
		descriptor->is_push_constant = (descriptor_info & (1U << 16)) ? true : false;
		descriptor->is_opaque = (descriptor_info & (1U << 15)) ? true : false;
		descriptor->is_uniform = (descriptor_info & (1U << 14)) ? true : false;
		descriptor->handle.type = descriptor_info & 0xFFU;
		descriptor->stage_flags = 0;
		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_VERTEX)))
			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_VERTEX);
		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_TESSELLATION)))
			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_TESSELLATION);
		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_GEOMETRY)))
			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_GEOMETRY);
		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_FRAGMENT)))
			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_FRAGMENT);
		
		const char* name = buf_get_ptr_at(shader_binary, temp_cursor);
		u32 len = strlen(name);
		assert(len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
		temp_cursor += len + 1;
		if(descriptor->handle.type == SHADER_COMPILER_BLOCK)
		{
			// ignore the block name
			name = buf_get_ptr_at(shader_binary, temp_cursor);
			len = strlen(name);
			assert(len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
			temp_cursor += len + 1;
		}
		strcpy(descriptor->handle.name, name);

		log_msg("Descriptor[%u]: (set = %u, binding = %u), stage_flags = %u, is_push_constant = %s, is_uniform = %s, is_opaque = %s, is_block = %s, name = %s\n", 
			i, descriptor->set_number, descriptor->binding_number, descriptor->stage_flags,
			descriptor->is_push_constant ? "true" : "false", descriptor->is_uniform ? "true" : "false", descriptor->is_opaque ? "true" : "false", (descriptor->handle.type == SHADER_COMPILER_BLOCK) ? "true" : "false",
			descriptor->handle.name);

		if(descriptor->handle.type == SHADER_COMPILER_BLOCK)
		{
			descriptor->handle.field_count = *(u16*)buf_get_ptr_at(shader_binary, temp_cursor); temp_cursor += 2;
			log_msg("Field Count: %u\n", descriptor->handle.field_count);
			indices[i] = buf_get_element_count(&fields);
			for(u16 j = 0; j < descriptor->handle.field_count; j++)
			{
				u32 descriptor_info = *(u16*)buf_get_ptr_at(shader_binary, temp_cursor); temp_cursor += 4;
				const char* name = buf_get_ptr_at(shader_binary, temp_cursor);
				u32 len = strlen(name);
				assert(len < STRUCT_FIELD_MAX_NAME_SIZE);
				temp_cursor += len + 1;
				u8 type = descriptor_info & 0xFFUL;
				struct_field_t field = { .type = type, .size = sizeof_glsl_type(type), .alignment = alignof_glsl_type(type) };
				strcpy(field.name, name);
				buf_push(&fields, &field);
				log_msg("Field[%u]: type = %u, size = %u, align = %u, name = %s\n", j, field.type, field.size, field.alignment, field.name);
			}
		}
	}
	if(buf_get_element_count(&fields) == 0)
		buf_free(&fields);
	else
		buf_fit(&fields);
	for(u16 i = 0; i < descriptor_count; i++)
	{
		if(indices[i] == BUF_INVALID_INDEX)
			continue;
		descriptors[i].handle.fields = buf_get_ptr_at(&fields, indices[i]);
		struct_descriptor_recalculate(&descriptors[i].handle);
		log_msg("Struct \"%s\", size = %u\n", descriptors[i].handle.name, struct_descriptor_sizeof(&descriptors[i].handle));
	}
	stack_free(indices);
	return descriptors;
}


static VkDescriptorSetLayout get_vulkan_descriptor_set_layout(renderer_t* renderer, vulkan_shader_resource_descriptor_t* descriptors, u32 descriptor_count)
{
	if(descriptor_count == 0)
	{
		assert_wrn(descriptor_count != 0);
		return VK_NULL_HANDLE;
	}
	VkDescriptorSetLayoutBinding* bindings = stack_newv(VkDescriptorSetLayoutBinding, descriptor_count);
	memset(bindings, 0, sizeof(VkDescriptorSetLayoutBinding) * descriptor_count);
	u32 binding_count = 0;
	for(u32 i = 0; i < descriptor_count; i++)
	{
		vulkan_shader_resource_descriptor_t* descriptor = refp(vulkan_shader_resource_descriptor_t, descriptors, i);
		if(descriptor->is_push_constant)
			continue;
		VkDescriptorSetLayoutBinding* binding = refp(VkDescriptorSetLayoutBinding, bindings, binding_count);
		binding->binding = descriptor->binding_number;
		binding->descriptorCount = 1;	//for now we are not using arrays
		binding->stageFlags = 0;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
			binding->stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
			binding->stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
			binding->stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
			binding->stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		switch(descriptor->handle.type)
		{
			case SHADER_COMPILER_BLOCK:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
			
			case SHADER_COMPILER_SAMPLER2D:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
			
			case SHADER_COMPILER_SAMPLER3D:
				LOG_FETAL_ERR("Cannot create set layout binding for the type SHADER_COMPILER_SAMPLER3D, because the implementation is still in development\n");
			break;
			
			default:
				LOG_FETAL_ERR("Cannot create set layout binding for the type \"%u\", because it is not recognized\n", descriptor->handle.type);
			break;
		}
		binding_count++;
	}
	VkDescriptorSetLayout vk_set_layout = vk_get_descriptor_set_layout(renderer->vk_device, bindings, binding_count);
	stack_free(bindings);
	return vk_set_layout;
}

/*
	Scalars
	The basic non-vector types are:

	bool: conditional type, values may be either true or false
	int: a signed, two's complement, 32-bit integer
	uint: an unsigned 32-bit integer
	float: an IEEE-754 single-precision floating point number
	double: an IEEE-754 double-precision floating-point number
 
 	Vectors
	Each of the scalar types, including booleans, have 2, 3, and 4-component vector equivalents. The n digit below can be 2, 3, or 4:

	bvecn: a vector of booleans
	ivecn: a vector of signed integers
	uvecn: a vector of unsigned integers
	vecn: a vector of single-precision floating-point numbers
	dvecn: a vector of double-precision floating-point numbers

	NOTE: uniform int count: 'non-opaque uniforms outside a block' : not allowed when using GLSL for Vulkan
 */
static u16 sizeof_glsl_type(u8 type)
{
	switch(type)
	{
		case SHADER_COMPILER_BLOCK 		: LOG_FETAL_ERR("getting size of non glsl type \"SHADER_COMPILER_BLOCK\" is a invalid operation\n");
		case SHADER_COMPILER_FLOAT 		:
		case SHADER_COMPILER_INT 		:
		case SHADER_COMPILER_UINT 		: return 4;
		case SHADER_COMPILER_DOUBLE 	: LOG_FETAL_ERR("\"double\" isn't supported yet\n");
		case SHADER_COMPILER_VEC4 		:
		case SHADER_COMPILER_IVEC4 		:
		case SHADER_COMPILER_UVEC4 		:
		case SHADER_COMPILER_MAT2 		: return 16;
		case SHADER_COMPILER_IVEC3 		:
		case SHADER_COMPILER_UVEC3 		:
		case SHADER_COMPILER_VEC3 		: return 12;
		case SHADER_COMPILER_IVEC2 		:
		case SHADER_COMPILER_UVEC2 		:
		case SHADER_COMPILER_VEC2 		: return 8;
		case SHADER_COMPILER_MAT4 		: return 64;
		case SHADER_COMPILER_MAT3 		: return 36;
		case SHADER_COMPILER_SAMPLER2D 	:
		case SHADER_COMPILER_SAMPLER3D	: LOG_FETAL_ERR("getting size of opaque types are invalid operation\n");
		default							: LOG_FETAL_ERR("Unrecognized glsl type \"%u\"\n", type);
	};
}

/*
	From the spec:

	Standard Uniform Buffer Layout

	The 'base alignment' of the type of an OpTypeStruct member of is defined recursively as follows:

	A scalar of size N has a base alignment of N.
	A two-component vector, with components of size N, has a base alignment of 2 N.
	A three- or four-component vector, with components of size N, has a base alignment of 4 N.
	An array has a base alignment equal to the base alignment of its element type, rounded up to a multiple of 16.
	A structure has a base alignment equal to the largest base alignment of any of its members, rounded up to a multiple of 16.
	A row-major matrix of C columns has a base alignment equal to the base alignment of a vector of C matrix components.
	A column-major matrix has a base alignment equal to the base alignment of the matrix column type.
	The std140 layout in GLSL satisfies these rules.

 */
static u16 alignof_glsl_type(u8 type)
{
	switch(type)
	{
		case SHADER_COMPILER_BLOCK 		: LOG_FETAL_ERR("getting alignment of non glsl type \"SHADER_COMPILER_BLOCK\" is a invalid operation\n");
		case SHADER_COMPILER_FLOAT 		:
		case SHADER_COMPILER_INT 		:
		case SHADER_COMPILER_UINT 		: return 4;
		case SHADER_COMPILER_DOUBLE 	: LOG_FETAL_ERR("\"double\" isn't supported yet\n");
		case SHADER_COMPILER_VEC4 		:
		case SHADER_COMPILER_IVEC4 		:
		case SHADER_COMPILER_UVEC4 		:
		case SHADER_COMPILER_IVEC3 		:
		case SHADER_COMPILER_UVEC3 		:
		case SHADER_COMPILER_VEC3 		:
		case SHADER_COMPILER_MAT4 		:
		case SHADER_COMPILER_MAT3 		: return 16;
		case SHADER_COMPILER_IVEC2 		:
		case SHADER_COMPILER_UVEC2 		:
		case SHADER_COMPILER_VEC2 		:
		case SHADER_COMPILER_MAT2 		: return 8;
		case SHADER_COMPILER_SAMPLER2D 	:
		case SHADER_COMPILER_SAMPLER3D	: LOG_FETAL_ERR("getting alignment of opaque types are invalid operation\n");
		default							: LOG_FETAL_ERR("Unrecognized glsl type \"%u\"\n", type);
	};
}
