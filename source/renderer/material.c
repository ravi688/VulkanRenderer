
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/material.h>
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <shader_compiler/compiler.h>

#include <string.h>

#define VERTEX_INFO_COUNT 4

instantiate_static_stack_array(VkFormat);
static void get_vulkan_constants(VkFormat* out_formats, u32* out_sizes, u32* out_indices);
static material_t* __material_create(renderer_t* renderer, VkDescriptorSetLayout vk_set_layout, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, shader_t* shader);
static void __material_create_no_alloc(renderer_t* renderer, VkDescriptorSetLayout vk_set_layout, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, shader_t* shader, material_t* material);
static u32 decode_attribute_count(u64 packed_attributes);
static void decode_vulkan_vertex_infos(u64 packed_attributes, VkVertexInputRate input_rate, vulkan_vertex_info_t* out_vertex_infos, VkFormat* formats, u32* offsets);

static void get_record_and_field_name(const char* const full_name, char out_struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE], char out_field_name[STRUCT_FIELD_MAX_NAME_SIZE]);
static VkDescriptorSetLayoutBinding* get_set_layout_bindings(shader_t* shader, u32* out_binding_count);
static void material_set_up_shader_resources(material_t* material);

static void unmap_descriptor(material_t* material, material_field_handle_t handle);
static struct_descriptor_t* map_descriptor(material_t* material, material_field_handle_t handle);

#ifndef GLOBAL_DEBUG
#	define check_precondition(material)
#else
	static void check_precondition(material_t* material);
#endif

typedef struct uniform_resource_t
{
	vulkan_buffer_t buffer;
	u16 descriptor_index;
} uniform_resource_t;

typedef struct material_t
{
	vulkan_material_t* handle;
	uniform_resource_t* uniform_resources;
	u16 uniform_resource_count;
} material_t;

material_t* material_new()
{
	material_t* material = heap_new(material_t);
	memset(material, 0, sizeof(material_t));
	material->handle = vulkan_material_new();
	return material;
}

material_t* material_create(renderer_t* renderer, material_create_info_t* create_info)
{
	u32 per_vertex_attribute_count = decode_attribute_count(create_info->per_vertex_attributes);
	u32 per_instance_attribute_count = decode_attribute_count(create_info->per_instance_attributes);
	log_u32(per_vertex_attribute_count);
	log_u32(per_instance_attribute_count);
	u32 total_attribute_count = per_vertex_attribute_count + per_instance_attribute_count;
	if(total_attribute_count == 0)
		LOG_FETAL_ERR("Material create error: total_attribute_count is equals to 0\n");
	VkFormat* formats = stack_newv(VkFormat, total_attribute_count + 5);
	u32* offsets = stack_newv(u32, total_attribute_count + 5);
	vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, total_attribute_count);
	if(per_vertex_attribute_count > 0)
		decode_vulkan_vertex_infos(create_info->per_vertex_attributes, VK_VERTEX_INPUT_RATE_VERTEX, vertex_infos, formats, offsets);
	if(per_instance_attribute_count > 0)
		decode_vulkan_vertex_infos(create_info->per_instance_attributes, VK_VERTEX_INPUT_RATE_INSTANCE, vertex_infos + per_vertex_attribute_count, formats + per_vertex_attribute_count, offsets + per_vertex_attribute_count);

	material_t* material =  __material_create(renderer, create_info->shader->vk_set_layout, total_attribute_count, vertex_infos, create_info->shader);
	material_set_up_shader_resources(material);
	stack_free(formats);
	stack_free(offsets);
	stack_free(vertex_infos);
	return material;
}

static material_t* __material_create(renderer_t* renderer, VkDescriptorSetLayout vk_set_layout, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, shader_t* shader)
{
	material_t* material = material_new();
	vulkan_material_create_info_t material_info =
	{
		.shader = shader,
		.vertex_info_count = vertex_info_count,
		//NOTE: calling vulkan_material_create() creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos,
		.vk_set_layout = vk_set_layout
	};
	material->handle = vulkan_material_create(renderer, &material_info);
	return material;
}

static void __material_create_no_alloc(renderer_t* renderer, VkDescriptorSetLayout vk_set_layout, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, shader_t* shader, material_t* material)
{
	vulkan_material_create_info_t material_info =
	{
		.shader = shader,
		.vertex_info_count = vertex_info_count,
		//NOTE: calling vulkan_material_create_no_alloc() doesn't creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos,
		.vk_set_layout = vk_set_layout
	};
	vulkan_material_create_no_alloc(renderer, &material_info, material->handle);
}

static void material_set_up_shader_resources(material_t* material)
{
	assert(material->handle->shader != NULL);
	if((material->handle->shader->descriptors == NULL) || (material->handle->shader->descriptor_count == 0))
	{
		material->uniform_resources = NULL;
		material->uniform_resource_count = 0;
		return;
	}
	u16 count = material->handle->shader->descriptor_count;
	vulkan_shader_resource_descriptor_t* descriptors = material->handle->shader->descriptors;
	uniform_resource_t* uniform_resources = heap_newv(uniform_resource_t, count);
	memset(uniform_resources, 0, sizeof(uniform_resource_t) * count);
	for(u16 i = 0; i < count; i++)
	{
		vulkan_shader_resource_descriptor_t* descriptor = refp(vulkan_shader_resource_descriptor_t, descriptors, i);
		uniform_resource_t* resource = &uniform_resources[i];
		if(descriptor->handle.type == SHADER_COMPILER_BLOCK)
		{
			u32 size = struct_descriptor_sizeof(&descriptor->handle);
			vulkan_buffer_init(&resource->buffer);
			resource->descriptor_index = i;
			vulkan_buffer_create_info_t create_info =
			{
				.size = size,
				.usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			vulkan_buffer_create_no_alloc(material->handle->renderer, &create_info, &resource->buffer);
			vulkan_material_set_uniform_buffer(material->handle, material->handle->renderer, descriptor->binding_number, &resource->buffer);
		}
		else
			resource->descriptor_index = 0xFFFF;
	}
	material->uniform_resources = uniform_resources;
	material->uniform_resource_count = material->handle->shader->descriptor_count;
}

void material_destroy(material_t* material, renderer_t* renderer)
{
	vulkan_material_destroy(material->handle, renderer);
	for(u16 i = 0; i < material->uniform_resource_count; i++)
	{
		if(material->uniform_resources[i].descriptor_index == 0xFFFF)
			continue;
		vulkan_buffer_destroy(&material->uniform_resources[i].buffer, renderer);
	}
}

void material_release_resources(material_t* material)
{
	vulkan_material_release_resources(material->handle);
	if(material->uniform_resources != NULL)
		heap_free(material->uniform_resources);
	heap_free(material);
}

void material_bind(material_t* material, renderer_t* renderer)
{
	vulkan_material_bind(material->handle, renderer);
}

void material_push_constants(material_t* material, renderer_t* renderer, void* bytes)
{
	vulkan_material_push_constants(material->handle, renderer, bytes);
}

material_field_handle_t material_get_field_handle(material_t* material, const char* name)
{
	assert(material != NULL);
	assert(material->handle->shader != NULL);

	if(material->handle->shader->descriptor_count == 0)
		LOG_WRN("Couldn't get field handle to \"%s\", reason: material->handle->shader->descriptor_count == 0\n", name);

	char struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE];
	char field_name[STRUCT_FIELD_MAX_NAME_SIZE];
	get_record_and_field_name(name, struct_name, field_name);

	u16 descriptor_count = material->handle->shader->descriptor_count;
	vulkan_shader_resource_descriptor_t* descriptors = material->handle->shader->descriptors;
	for(u16 i = 0; i < descriptor_count; i++)
	{
		vulkan_shader_resource_descriptor_t descriptor = ref(vulkan_shader_resource_descriptor_t, descriptors, i);
		if(strcmp(descriptor.handle.name, struct_name) == 0)
		{
			return (material_field_handle_t)
			{
				.descriptor_index = i,
				.field_handle = struct_descriptor_get_field_handle(&descriptor.handle, field_name)
			};
		}
	}
	LOG_FETAL_ERR("symbol \"%s\" isn't found in the shader resource descriptors\n", name);
	return (material_field_handle_t) { .descriptor_index = 0xFFFF, .field_handle = STRUCT_FIELD_INVALID_HANDLE };
}

/* functions accepting handles */
void material_set_floatH(material_t* material, material_field_handle_t handle, float value)
{
	check_precondition(material);
	struct_descriptor_set_float(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

void material_set_intH(material_t* material, material_field_handle_t handle, int value)
{
	check_precondition(material);
	struct_descriptor_set_int(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

void material_set_uintH(material_t* material, material_field_handle_t handle, uint value)
{
	check_precondition(material);
	struct_descriptor_set_uint(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t(float) value)
{
	check_precondition(material);
	struct_descriptor_set_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t(float) value)
{
	check_precondition(material);
	struct_descriptor_set_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t(float) value)
{
	check_precondition(material);
	struct_descriptor_set_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t(float) value)
{
	check_precondition(material);
	struct_descriptor_set_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t(float) value)
{
	check_precondition(material);
	struct_descriptor_set_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

void material_set_texture2dH(material_t* material, material_field_handle_t handle, texture_t* texture)
{
	check_precondition(material);
	assert(handle.descriptor_index < material->handle->shader->descriptor_count);
	vulkan_shader_resource_descriptor_t descriptor = material->handle->shader->descriptors[handle.descriptor_index];
	assert(descriptor.set_number < 1); 	//for now we are just using one descriptor set and multiple bindings
	vulkan_material_set_texture(material->handle, material->handle->renderer, descriptor.binding_number, texture);
}

float material_get_floatH(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	float value;
	struct_descriptor_get_float(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

int material_get_intH(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	int value;
	struct_descriptor_get_int(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

uint material_get_uintH(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	uint value;
	struct_descriptor_get_uint(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

vec2_t(float) material_get_vec2H(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	vec2_t(float) value;
	struct_descriptor_get_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

vec3_t(float) material_get_vec3H(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	vec3_t(float) value;
	struct_descriptor_get_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

vec4_t(float) material_get_vec4H(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	vec4_t(float) value;
	struct_descriptor_get_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

mat2_t(float) material_get_mat2H(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	mat2_t(float) value;
	struct_descriptor_get_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

mat4_t(float) material_get_mat4H(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	mat4_t(float) value;
	struct_descriptor_get_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle)
{
	check_precondition(material);
	LOG_WRN("material_get_texture2dH isn't defined, for now it will return NULL\n");
	return NULL;
}

/* functions accepting strings */
void material_set_float(material_t* material, const char* name, float value)
{
	material_set_floatH(material, material_get_field_handle(material, name), value);
}

void material_set_int(material_t* material, const char* name, int value)
{
	material_set_intH(material, material_get_field_handle(material, name), value);
}

void material_set_uint(material_t* material, const char* name, uint value)
{
	material_set_uintH(material, material_get_field_handle(material, name), value);
}

void material_set_vec2(material_t* material, const char* name, vec2_t(float) v)
{
	material_set_vec2H(material, material_get_field_handle(material, name), v);
}

void material_set_vec3(material_t* material, const char* name, vec3_t(float) v)
{
	material_set_vec3H(material, material_get_field_handle(material, name), v);
}

void material_set_vec4(material_t* material, const char* name, vec4_t(float) v)
{
	material_set_vec4H(material, material_get_field_handle(material, name), v);
}

void material_set_mat2(material_t* material, const char* name, mat2_t(float) m)
{
	material_set_mat2H(material, material_get_field_handle(material, name), m);
}

void material_set_mat4(material_t* material, const char* name, mat4_t(float) m)
{
	material_set_mat4H(material, material_get_field_handle(material, name), m);
}

void material_set_texture2d(material_t* material, const char* name, texture_t* texture)
{
	material_set_texture2dH(material, material_get_field_handle(material, name), texture);
}

float material_get_float(material_t* material, const char* name)
{
	return material_get_floatH(material, material_get_field_handle(material, name));
}

int material_get_int(material_t* material, const char* name)
{
	return material_get_intH(material, material_get_field_handle(material, name));
}

uint material_get_uint(material_t* material, const char* name)
{
	return material_get_uintH(material, material_get_field_handle(material, name));
}

vec2_t(float) material_get_vec2(material_t* material, const char* name)
{
	return material_get_vec2H(material, material_get_field_handle(material, name));
}

vec3_t(float) material_get_vec3(material_t* material, const char* name)
{
	return material_get_vec3H(material, material_get_field_handle(material, name));
}

vec4_t(float) material_get_vec4(material_t* material, const char* name)
{
	return material_get_vec4H(material, material_get_field_handle(material, name));
}

mat2_t(float) material_get_mat2(material_t* material, const char* name)
{
	return material_get_mat2H(material, material_get_field_handle(material, name));
}

mat4_t(float) material_get_mat4(material_t* material, const char* name)
{
	return material_get_mat4H(material, material_get_field_handle(material, name));
}

texture_t* material_get_texture2d(material_t* material, const char* name)
{
	return material_get_texture2dH(material, material_get_field_handle(material, name));
}


#if GLOBAL_DEBUG
static void check_precondition(material_t* material)
{
	assert(material != NULL);
	assert(material->handle->shader != NULL);
	assert(material->handle->shader->descriptors != NULL);
	assert_wrn(material->handle->shader->descriptor_count != 0);
}
#endif


static void unmap_descriptor(material_t* material, material_field_handle_t handle)
{
	vulkan_buffer_t* buffer = &(material->uniform_resources[handle.descriptor_index].buffer);
	struct_descriptor_t* descriptor = &(material->handle->shader->descriptors[handle.descriptor_index].handle);
	struct_descriptor_unmap(descriptor);
	vulkan_buffer_unmap(buffer, material->handle->renderer);
}

static struct_descriptor_t* map_descriptor(material_t* material, material_field_handle_t handle)
{
	vulkan_buffer_t* buffer = &(material->uniform_resources[handle.descriptor_index].buffer);
	struct_descriptor_t* descriptor = &(material->handle->shader->descriptors[handle.descriptor_index].handle);
	struct_descriptor_map(descriptor, vulkan_buffer_map(buffer, material->handle->renderer));
	return descriptor;
}

static void get_record_and_field_name(const char* const full_name, char out_struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE], char out_field_name[STRUCT_FIELD_MAX_NAME_SIZE])
{
	u32 len = strlen(full_name);
	assert(len != 0);
	const char* ptr = strchr(full_name, '.');
	memset(out_field_name, 0, STRUCT_FIELD_MAX_NAME_SIZE);
	memset(out_struct_name, 0, STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
	if(ptr == NULL)
	{
		memcpy(out_struct_name, full_name, len);
		return;
	}
	u16 struct_name_len = (u16)(ptr - full_name);
	u16 field_name_len = (u16)(len - struct_name_len - 1);
	assert(struct_name_len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
	assert(field_name_len < STRUCT_FIELD_MAX_NAME_SIZE);
	memcpy(out_struct_name, full_name, struct_name_len);
	memcpy(out_field_name, ptr + 1, field_name_len);
}

static void get_vulkan_constants(VkFormat* out_formats, u32* out_sizes, u32* out_indices)
{
	{
		u32 indices[21 + 1] =
		{	0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 16, 20, 21, 22, 23, 24, 26
		};
		memcpy(refp(u32, out_indices, 0), indices, sizeof(u32) * 22);
	};
	{
		u32 sizes[21 + 1] =
		{
			0,
			1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 12, 16, 16,
			36, 64, 8, 12, 16, 8, 12, 16
		};
		memcpy(refp(u32, out_sizes, 0), sizes, sizeof(u32) * 22);
	};
	VkFormat vulkan_formats[26] =
	{
		//U8
		VK_FORMAT_R8_UINT,
		//U16
		VK_FORMAT_R16_UINT,
		//U32
		VK_FORMAT_R32_UINT,
		//U64
		VK_FORMAT_R64_UINT,
		//S8
		VK_FORMAT_R8_SINT,
		//S16
		VK_FORMAT_R16_SINT,
		//S32
		VK_FORMAT_R32_SINT,
		//S64
		VK_FORMAT_R64_SINT,
		//F32
		VK_FORMAT_R32_SFLOAT,
		//VEC2
		VK_FORMAT_R32G32_SFLOAT,
		//VEC3
		VK_FORMAT_R32G32B32_SFLOAT,
		//VEC4
		VK_FORMAT_R32G32B32A32_SFLOAT,
		//MAT2
		VK_FORMAT_R32G32B32A32_SFLOAT,

		//MAT3
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,

		//MAT4
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT,

		//IVEC2
		VK_FORMAT_R32G32_SINT,
		//IVEC3
		VK_FORMAT_R32G32B32_SINT,
		//IVEC4
		VK_FORMAT_R32G32B32A32_SINT,
		//UVEC2
		VK_FORMAT_R32G32_UINT,
		//UVEC3
		VK_FORMAT_R32G32B32_UINT,
		//UVEC4
		VK_FORMAT_R32G32B32A32_UINT,
	};
	memcpy(refp(VkFormat, out_formats, 0), vulkan_formats, sizeof(VkFormat) * 26);
}

static u32 decode_attribute_count(u64 packed_attributes)
{
	u8 bits_per_type = 5;
	log_u64(packed_attributes);
	u64 bits_mask = ~(0xFFFFFFFFFFFFFFFFULL << bits_per_type);
	u32 count = 0;
	while(packed_attributes != 0)
	{
		if((packed_attributes & bits_mask) != 0)
			++count;
		packed_attributes >>= bits_per_type;
	}
	return count;
}

static void decode_vulkan_vertex_infos(u64 packed_attributes, VkVertexInputRate input_rate, vulkan_vertex_info_t* out_vertex_infos, VkFormat* formats, u32* offsets)
{
	VkFormat* vulkan_formats = stack_newv(u32, 26);
	u32* sizes = stack_newv(u32, 22);
	u32* indices = stack_newv(u32, 22);
	get_vulkan_constants(vulkan_formats, sizes, indices);

	u8 bits_per_type = 5;
	u64 bits_mask = ~(0xFFFFFFFFFFFFFFFFULL << bits_per_type);

	u32 i = 0;
	while(packed_attributes != 0)
	{
		//WARNING: this should be like this: refp(vulkan_vertex_info_t, out_vertex_infos, i)
		vulkan_vertex_info_t* info = &out_vertex_infos[i];
		u64 attribute_type = (packed_attributes & bits_mask);
		if(attribute_type == 0)
		{
			packed_attributes >>= bits_per_type;
			continue;
		}
		VkFormat vulkan_format = ref(VkFormat, vulkan_formats, ref(u32, indices, attribute_type));
		info->input_rate = input_rate;
		info->size = ref(u32, sizes, attribute_type);
		info->attribute_count = 1;
		switch(attribute_type)
		{
			case MATERIAL_MAT3:
				LOG_FETAL_ERR("Vertex attribute decode error: MATERIAL_MAT3 isn't supported yet!\n");
			break;

			case MATERIAL_MAT4:
				LOG_FETAL_ERR("Vertex attribute decode error: MATERIAL_MAT4 isn't supported yet!\n");
			break;

			default:/*or MATERIAL_MAT2*/
				//WARNING: this should be like this: ref(VkFormat, formats, i)
				formats[i] = vulkan_format;
				//WARNING: this should be like this: refp(VkFormat, formats, i)
				info->attribute_formats = &formats[i];
				//WARNING: this should be like this: ref(u32, offsets, i)
				offsets[i] = 0;
				//WARNING: this should be like this: refp(u32, offsets, i)
				info->attribute_offsets = &offsets[i];
			break;
		}
		packed_attributes >>= bits_per_type;
		i++;
	}
	stack_free(vulkan_formats);
	stack_free(indices);
}
