
#pragma once

typedef struct renderer_t renderer_t;
typedef struct shader_t shader_t;

#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_material_t vulkan_material_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef vulkan_texture_t texture_t;
typedef vulkan_material_t material_t;
#endif

#ifdef RENDERER_OPENGL_DRIVER
#error "OpenGL is not supported yet!"
#endif

#ifdef RENDERER_DIRECTX_DRIVER
#error "DirectX is not supported yet!"
#endif

#ifdef RENDERER_METAL_DRIVER
#error "Metal is not supported yet!"
#endif

#define BIT64(x) (1ULL << (x))
#define MATERIAL_TYPE_BITS_MASK (~(0xFFFFFFFFFFFFFFFFULL >> 3))
#define MATERIAL_TYPE_BITS(value) (((~(0xFFFFFFFFFFFFFFFFULL >> 3)) & (value)) >> 61)
#define MATERIAL_ALIGN(value, index) (MATERIAL_TYPE_BITS_MASK & (value)) | (((0xFFFFFFFFFFFFFFFFULL >> 3) & (value)) << ((index) * MATERIAL_TYPE_BITS(value)))

enum 
{
	MATERIAL_U8 	= BIT64(63) | 1,
	MATERIAL_U16 	= BIT64(63) | 2,
	MATERIAL_U32	= BIT64(63) | 3,
	MATERIAL_U64 	= BIT64(63) | 4,
	MATERIAL_S8 	= BIT64(63) | 5,
	MATERIAL_S16 	= BIT64(63) | 6,
	MATERIAL_S32 	= BIT64(63) | 7,
	MATERIAL_S64 	= BIT64(63) | 8,
	MATERIAL_F32 	= BIT64(63) | 9,
	MATERIAL_VEC2 	= BIT64(63) | 10,
	MATERIAL_VEC3 	= BIT64(63) | 11,
	MATERIAL_VEC4 	= BIT64(63) | 12,
	MATERIAL_MAT2 	= BIT64(63) | 13,
	MATERIAL_MAT3 	= BIT64(63) | 14,
	MATERIAL_MAT4	= BIT64(63) | 15,

	MATERIAL_IVEC2 	= BIT64(63) | BIT64(61) | 16,
	MATERIAL_IVEC3 	= BIT64(63) | BIT64(61) | 17,
	MATERIAL_IVEC4 	= BIT64(63) | BIT64(61) | 18,
	MATERIAL_UVEC2 	= BIT64(63) | BIT64(61) | 19,
	MATERIAL_UVEC3 	= BIT64(63) | BIT64(61) | 20,
	MATERIAL_UVEC4 	= BIT64(63) | BIT64(61) | 21,

	MATERIAL_POSITION = MATERIAL_ALIGN(MATERIAL_VEC3, 0),
	MATERIAL_NORMAL = MATERIAL_ALIGN(MATERIAL_VEC3, 1),
	MATERIAL_COLOR = MATERIAL_ALIGN(MATERIAL_VEC3, 2),
	MATERIAL_TEXCOORD = MATERIAL_ALIGN(MATERIAL_VEC2, 3)
};

typedef struct material_create_info_t
{
	u64 per_vertex_attributes;
	u64 per_instance_attributes;
	shader_t* shader;
} material_create_info_t;

material_t* material_new();
material_t* material_create(renderer_t* renderer, material_create_info_t* create_info);
void material_create_no_alloc(renderer_t* renderer, material_create_info_t* create_info, material_t* material);
void material_destroy(material_t* material, renderer_t* renderer);
void material_release_resources(material_t* material);

void material_bind(material_t* material, renderer_t* renderer);
void material_push_constants(material_t* material, renderer_t* renderer, void* bytes);
void material_set_texture(material_t* material, renderer_t* renderer, texture_t* texture);
