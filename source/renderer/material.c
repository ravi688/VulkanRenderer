
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/stage_shader.h>
#include <renderer/shader.h>
#include <renderer/material.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/debug.h>
#include <renderer/assert.h>

#define VERTEX_INFO_COUNT 4

instantiate_static_stack_array(VkFormat);
static void get_vulkan_constants(VkFormat* out_formats, u32* out_sizes, u32* out_indices);
static void __material_create_no_alloc(renderer_t* renderer, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, u32 shader_count, stage_shader_t** shaders, material_t* material);
static material_t* __material_create(renderer_t* renderer, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, u32 shader_count, stage_shader_t** shaders);
static u32 decode_attribute_count(u64 packed_attributes);
static void decode_vulkan_vertex_infos(u64 packed_attributes, VkVertexInputRate input_rate, vulkan_vertex_info_t* out_vertex_infos, VkFormat* formats, u32* offsets)
{
	VkFormat* vulkan_formats = stack_newv(u32, 26);
	u32* sizes = stack_newv(u32, 22);
	u32* indices = stack_newv(u32, 22);
	get_vulkan_constants(vulkan_formats, sizes, indices);

	u8 bits_per_type = MATERIAL_TYPE_BITS(packed_attributes);
	packed_attributes &= ~MATERIAL_TYPE_BITS_MASK;
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
		switch(attribute_type | BIT64(63))
		{
			case MATERIAL_MAT2:
			break;

			case MATERIAL_MAT3:
			break;

			case MATERIAL_MAT4:
			break;

			default:
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


material_t* material_new() { return vulkan_material_new(); }

material_t* material_create(renderer_t* renderer, material_create_info_t* create_info)
{
	u32 per_vertex_attribute_count = decode_attribute_count(create_info->per_vertex_attributes);
	u32 per_instance_attribute_count = decode_attribute_count(create_info->per_instance_attributes);
	u32 total_attribute_count = per_vertex_attribute_count + per_instance_attribute_count;
	if((total_attribute_count) == 0)
	{
		LOG_FETAL_ERR("Material create error: total_attribute_count is equals to 0\n");
	}
	VkFormat* formats = stack_newv(VkFormat, total_attribute_count + 5);
	u32* offsets = stack_newv(u32, total_attribute_count + 5);
	vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, total_attribute_count);
	if(per_vertex_attribute_count > 0)
		decode_vulkan_vertex_infos(create_info->per_vertex_attributes, VK_VERTEX_INPUT_RATE_VERTEX, vertex_infos, formats, offsets);
	if(per_instance_attribute_count > 0)
		decode_vulkan_vertex_infos(create_info->per_instance_attributes, VK_VERTEX_INPUT_RATE_INSTANCE, vertex_infos + per_vertex_attribute_count, formats + per_vertex_attribute_count, offsets + per_vertex_attribute_count);
	material_t* material =  __material_create(renderer, total_attribute_count, vertex_infos, create_info->shader->stage_count, create_info->shader->stage_shaders);
	stack_free(formats);
	stack_free(offsets);
	stack_free(vertex_infos);
	return material;
}

static material_t* __material_create(renderer_t* renderer, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, u32 shader_count, stage_shader_t** shaders)
{
	// VkFormat* attribute_formats1 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	// VkFormat* attribute_formats2 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	// VkFormat* attribute_formats3 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	// VkFormat* attribute_formats4 = stack_array(VkFormat, 1, VK_FORMAT_R32G32_SFLOAT);
	// u32* attribute_offsets1 = stack_array(u32, 1, 0);
	// u32* attribute_offsets2 = stack_array(u32, 1, 0);
	// u32* attribute_offsets3 = stack_array(u32, 1, 0);
	// u32* attribute_offsets4 = stack_array(u32, 1, 0);
	// vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, VERTEX_INFO_COUNT);
	// ref(vulkan_vertex_info_t, vertex_infos, 0) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats1, attribute_offsets1 };
	// ref(vulkan_vertex_info_t, vertex_infos, 1) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats2, attribute_offsets2 };
	// ref(vulkan_vertex_info_t, vertex_infos, 2) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats3, attribute_offsets3 };
	// ref(vulkan_vertex_info_t, vertex_infos, 3) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 2, 1, attribute_formats4, attribute_offsets4 };

	vulkan_material_create_info_t material_info =
	{
		.shaders = shaders,
		.shader_count = shader_count,
		.vertex_info_count = vertex_info_count,
		//NOTE: calling vulkan_material_create() creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos
	};
	vulkan_material_t* material = vulkan_material_create(renderer, &material_info);

	// stack_free(attribute_offsets1);
	// stack_free(attribute_offsets2);
	// stack_free(attribute_offsets3);
	// stack_free(attribute_offsets4);
	// stack_free(attribute_formats1);
	// stack_free(attribute_formats2);
	// stack_free(attribute_formats3);
	// stack_free(attribute_formats4);
	// stack_free(vertex_infos);
	return material;
}

static void __material_create_no_alloc(renderer_t* renderer, u32 vertex_info_count, vulkan_vertex_info_t* vertex_infos, u32 shader_count, stage_shader_t** shaders, material_t* material)
{
	vulkan_material_create_info_t material_info =
	{
		.shaders = shaders,
		.shader_count = shader_count,
		.vertex_info_count = vertex_info_count,
		//NOTE: calling vulkan_material_create_no_alloc() doesn't creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos
	};
	vulkan_material_create_no_alloc(renderer, &material_info, material);
}

void material_destroy(material_t* material, renderer_t* renderer)
{
	vulkan_material_destroy(material, renderer);
}

void material_release_resources(material_t* material)
{
	vulkan_material_release_resources(material);
}

void material_bind(material_t* material, renderer_t* renderer)
{
	vulkan_material_bind(material, renderer);
}

void material_push_constants(material_t* material, renderer_t* renderer, void* bytes)
{
	vulkan_material_push_constants(material, renderer, bytes);
}

void material_set_texture(material_t* material, renderer_t* renderer, texture_t* texture)
{
	vulkan_material_set_texture(material, renderer, texture);
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
	u8 bits_per_type = MATERIAL_TYPE_BITS(packed_attributes);
	packed_attributes &= ~MATERIAL_TYPE_BITS_MASK;
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
