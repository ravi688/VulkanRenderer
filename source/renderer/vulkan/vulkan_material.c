#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_material_t* vulkan_material_new()
{
	vulkan_material_t* material = heap_new(vulkan_material_t);
	memset(material, 0, sizeof(vulkan_material_t));
	return material;
}

static u16 calculate_uniform_resource_count(vulkan_shader_t* shader)
{
	assert(shader != NULL);
	if(shader->material_set_bindings == NULL) return 0;
	u16 count = shader->material_set_binding_count;
	u16 uniform_count = 0;
	for(u16 i = 0; i < count; i++)
		if(!shader->material_set_bindings[i].is_attribute)
			++uniform_count;
	return uniform_count;
}

static void setup_material_resources(vulkan_material_t* material)
{
	assert(material->shader != NULL);
	u16 count = calculate_uniform_resource_count(material->shader);
	if(count == 0) return;

	vulkan_shader_resource_description_t* bindings = material->shader->material_set_bindings;
	vulkan_uniform_resource_t* uniform_resources = heap_newv(vulkan_uniform_resource_t, count);
	memset(uniform_resources, 0, sizeof(vulkan_uniform_resource_t) * count);
	for(u16 i = 0, j = 0; i < material->shader->material_set_binding_count; i++)
	{
		vulkan_shader_resource_description_t* binding = &bindings[i];
		if(binding->is_attribute)
			continue;
		vulkan_uniform_resource_t* resource = &uniform_resources[j];
		j++;
		if((binding->handle.type == GLSL_TYPE_BLOCK) && (!binding->is_push_constant))
		{
			u32 size = struct_descriptor_sizeof(&binding->handle);
			resource->index = i;
			vulkan_buffer_create_info_t create_info =
			{
				.size = size,
				.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			vulkan_buffer_create_no_alloc(material->renderer, &create_info, &resource->buffer);
			vulkan_descriptor_set_write_uniform_buffer(&material->material_set, binding->binding_number, &resource->buffer);
		}
		else
			resource->index = 0xFFFF;
	}
	material->uniform_resources = uniform_resources;
	material->uniform_resource_count = count;
}

RENDERER_API void vulkan_material_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_t* shader, vulkan_material_t* material)
{
	memzero(material, vulkan_material_t);

	material->renderer = renderer;
	material->handle = VULKAN_MATERIAL_HANDLE_INVALID;
	material->shader = shader;

	// create MATERIAL_SET
	vulkan_descriptor_set_create_info_t create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &shader->material_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(renderer, &create_info, &material->material_set);
	setup_material_resources(material);
}

RENDERER_API vulkan_material_t* vulkan_material_create(vulkan_renderer_t* renderer, vulkan_shader_t* shader)
{
	// allocate memory and intitalize
	vulkan_material_t* material = vulkan_material_new();
	
	// create material
	vulkan_material_create_no_alloc(renderer, shader, material);
	return material;
}

RENDERER_API void vulkan_material_destroy(vulkan_material_t* material)
{
	vulkan_descriptor_set_destroy(&material->material_set);
	for(u16 i = 0; i < material->uniform_resource_count; i++)
		if(material->uniform_resources[i].index != 0xFFFF)
			vulkan_buffer_destroy(&material->uniform_resources[i].buffer);
}

RENDERER_API void vulkan_material_release_resources(vulkan_material_t* material)
{
	for(u16 i = 0; i < material->uniform_resource_count; i++)
		if(material->uniform_resources[i].index != 0xFFFF)
			vulkan_buffer_release_resources(&material->uniform_resources[i].buffer);
	if(material->uniform_resource_count != 0)
		heap_free(material->uniform_resources);
	// TODO
	// heap_free(material);
}

static VkShaderStageFlagBits get_vulkan_shader_flags(u8 _flags);

static void set_push_constants(vulkan_material_t* material, vulkan_shader_resource_description_t* descriptor, vulkan_pipeline_layout_t* pipeline_layout)
{
	vulkan_pipeline_layout_push_constants(pipeline_layout, 
											get_vulkan_shader_flags(descriptor->stage_flags), 
											descriptor->push_constant_range_offset, 
											struct_descriptor_sizeof(&descriptor->handle), 
											descriptor->handle.ptr);
}

RENDERER_API void vulkan_material_push_constants(vulkan_material_t* material, vulkan_pipeline_layout_t* pipeline_layout)
{
	u32 binding_count = material->shader->material_set_binding_count;
	for(u32 i = 0; i < binding_count; i++)
		if(material->shader->material_set_bindings[i].is_push_constant)
			set_push_constants(material, &material->shader->material_set_bindings[i], pipeline_layout);
}

/* ------------------------------------------ BEGIN: PUSH CONSTANTS -------------------------------------------*/

// TODO: fix this branching
static VkShaderStageFlagBits get_vulkan_shader_flags(u8 _flags)
{
	VkShaderStageFlagBits flags = 0;
	if(_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
		flags |= VK_SHADER_STAGE_VERTEX_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
		flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
		flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
		flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	return flags;
}

// setters
/* functions accepting handles */
#define set_push_value(material, handle, setter, in_value) __set_push_value(material, handle, (void (*)(struct_descriptor_t*, struct_field_handle_t, const void* const))(setter), in_value);
static void __set_push_value(vulkan_material_t* material, vulkan_material_field_handle_t handle, void (*setter)(struct_descriptor_t*, struct_field_handle_t, const void* const), const void* const in)
{
	vulkan_shader_resource_description_t* descriptor = &(material->shader->material_set_bindings[handle.index]);
	setter(&descriptor->handle, handle.field_handle, in);
	// set_push_constants(material, descriptor);
}

RENDERER_API void vulkan_material_set_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value)
{
	set_push_value(material, handle, struct_descriptor_set_float, &value);
}

RENDERER_API void vulkan_material_set_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value)
{
	set_push_value(material, handle, struct_descriptor_set_int, &value);
}

RENDERER_API void vulkan_material_set_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value)
{
	set_push_value(material, handle, struct_descriptor_set_uint, &value);
}

RENDERER_API void vulkan_material_set_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec2, &value);
}

RENDERER_API void vulkan_material_set_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec3, &value);
}

RENDERER_API void vulkan_material_set_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec4, &value);
}

RENDERER_API void vulkan_material_set_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t value)
{
	set_push_value(material, handle, struct_descriptor_set_mat2, &value);
}

RENDERER_API void vulkan_material_set_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t value)
{
	set_push_value(material, handle, struct_descriptor_set_mat4, &value);
}

// getters
#define get_push_value(material, handle, getter, out_value) __get_push_value(material, handle, (void (*)(struct_descriptor_t*, struct_field_handle_t, void* const))(getter), out_value);
static void __get_push_value(vulkan_material_t* material, vulkan_material_field_handle_t handle, void (*getter)(struct_descriptor_t*, struct_field_handle_t, void* const), void* const out)
{
	vulkan_shader_resource_description_t* descriptor = &(material->shader->material_set_bindings[handle.index]);
	getter(&descriptor->handle, handle.field_handle, out);
}

RENDERER_API float vulkan_material_get_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	float value;
	get_push_value(material, handle, struct_descriptor_get_float, &value);
	return value;
}

RENDERER_API int vulkan_material_get_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	int value;
	get_push_value(material, handle, struct_descriptor_get_int, &value);
	return value;
}

RENDERER_API uint vulkan_material_get_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	uint value;
	get_push_value(material, handle, struct_descriptor_get_uint, &value);
	return value;
}

RENDERER_API vec2_t vulkan_material_get_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec2_t value;
	get_push_value(material, handle, struct_descriptor_get_vec2, &value);
	return value;
}

RENDERER_API vec3_t vulkan_material_get_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec3_t value;
	get_push_value(material, handle, struct_descriptor_get_vec3, &value);
	return value;
}

RENDERER_API vec4_t vulkan_material_get_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec4_t value;
	get_push_value(material, handle, struct_descriptor_get_vec4, &value);
	return value;
}

RENDERER_API mat2_t vulkan_material_get_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat2_t value;
	get_push_value(material, handle, struct_descriptor_get_mat2, &value);
	return value;
}

RENDERER_API mat4_t vulkan_material_get_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat4_t value;
	get_push_value(material, handle, struct_descriptor_get_mat4, &value);
	return value;
}

/* functions accepting strings */
RENDERER_API void vulkan_material_set_push_float(vulkan_material_t* material, const char* name, float value)
{
	vulkan_material_set_push_floatH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_push_int(vulkan_material_t* material, const char* name, int value)
{
	vulkan_material_set_push_intH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_push_uint(vulkan_material_t* material, const char* name, uint value)
{
	vulkan_material_set_push_uintH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_push_vec2(vulkan_material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_push_vec2H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_push_vec3(vulkan_material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_push_vec3H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_push_vec4(vulkan_material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_push_vec4H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_push_mat2(vulkan_material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_push_mat2H(material, vulkan_material_get_field_handle(material, name), m);
}

RENDERER_API void vulkan_material_set_push_mat4(vulkan_material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_push_mat4H(material, vulkan_material_get_field_handle(material, name), m);
}

RENDERER_API float vulkan_material_get_push_float(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_floatH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API int vulkan_material_get_push_int(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_intH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API uint vulkan_material_get_push_uint(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_uintH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec2_t vulkan_material_get_push_vec2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec2H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec3_t vulkan_material_get_push_vec3(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec3H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec4_t vulkan_material_get_push_vec4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec4H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API mat2_t vulkan_material_get_push_mat2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_mat2H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API mat4_t vulkan_material_get_push_mat4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_mat4H(material, vulkan_material_get_field_handle(material, name));
}

/* ------------------------------------------ END: PUSH CONSTANTS -------------------------------------------*/

static void unmap_descriptor(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vulkan_buffer_t* buffer = &(material->uniform_resources[handle.uniform_index].buffer);
	struct_descriptor_t* descriptor = &(material->shader->material_set_bindings[handle.index].handle);
	struct_descriptor_unmap(descriptor);
	vulkan_buffer_unmap(buffer);
}

static struct_descriptor_t* map_descriptor(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vulkan_buffer_t* buffer = &(material->uniform_resources[handle.uniform_index].buffer);
	struct_descriptor_t* descriptor = &(material->shader->material_set_bindings[handle.index].handle);
	struct_descriptor_map(descriptor, vulkan_buffer_map(buffer));
	return descriptor;
}

RENDERER_API void vulkan_material_set_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value)
{
	struct_descriptor_set_float(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value)
{
	struct_descriptor_set_int(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value)
{
	struct_descriptor_set_uint(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t value)
{
	struct_descriptor_set_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t value)
{
	struct_descriptor_set_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t value)
{
	struct_descriptor_set_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t value)
{
	struct_descriptor_set_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t value)
{
	struct_descriptor_set_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

RENDERER_API void vulkan_material_set_textureH(vulkan_material_t* material, vulkan_material_field_handle_t handle, vulkan_texture_t* texture)
{
	assert(handle.index < material->shader->material_set_binding_count);
	vulkan_shader_resource_description_t descriptor = material->shader->material_set_bindings[handle.index];
	// assert(descriptor.set_number < 1); 	//for now we are just using one descriptor set and multiple bindings
	vulkan_descriptor_set_write_texture(&material->material_set, descriptor.binding_number, texture);
}

RENDERER_API float vulkan_material_get_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	float value;
	struct_descriptor_get_float(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

RENDERER_API int vulkan_material_get_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	int value;
	struct_descriptor_get_int(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

RENDERER_API uint vulkan_material_get_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	uint value;
	struct_descriptor_get_uint(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

RENDERER_API vec2_t vulkan_material_get_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec2_t value;
	struct_descriptor_get_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

RENDERER_API vec3_t vulkan_material_get_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec3_t value;
	struct_descriptor_get_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

RENDERER_API vec4_t vulkan_material_get_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec4_t value;
	struct_descriptor_get_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

RENDERER_API mat2_t vulkan_material_get_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat2_t value;
	struct_descriptor_get_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

RENDERER_API mat4_t vulkan_material_get_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat4_t value;
	struct_descriptor_get_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

RENDERER_API vulkan_texture_t* vulkan_material_get_texture2dH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	LOG_WRN("material_get_texture2dH isn't defined, for now it will return NULL\n");
	return NULL;
}

/* functions accepting strings */
RENDERER_API void vulkan_material_set_float(vulkan_material_t* material, const char* name, float value)
{
	vulkan_material_set_floatH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_int(vulkan_material_t* material, const char* name, int value)
{
	vulkan_material_set_intH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_uint(vulkan_material_t* material, const char* name, uint value)
{
	vulkan_material_set_uintH(material, vulkan_material_get_field_handle(material, name), value);
}

RENDERER_API void vulkan_material_set_vec2(vulkan_material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_vec2H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_vec3(vulkan_material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_vec3H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_vec4(vulkan_material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_vec4H(material, vulkan_material_get_field_handle(material, name), v);
}

RENDERER_API void vulkan_material_set_mat2(vulkan_material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_mat2H(material, vulkan_material_get_field_handle(material, name), m);
}

RENDERER_API void vulkan_material_set_mat4(vulkan_material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_mat4H(material, vulkan_material_get_field_handle(material, name), m);
}

RENDERER_API void vulkan_material_set_texture(vulkan_material_t* material, const char* name, vulkan_texture_t* texture)
{
	vulkan_material_set_textureH(material, vulkan_material_get_field_handle(material, name), texture);
}

RENDERER_API float vulkan_material_get_float(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_floatH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API int vulkan_material_get_int(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_intH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API uint vulkan_material_get_uint(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_uintH(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec2_t vulkan_material_get_vec2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec2H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec3_t vulkan_material_get_vec3(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec3H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vec4_t vulkan_material_get_vec4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec4H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API mat2_t vulkan_material_get_mat2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_mat2H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API mat4_t vulkan_material_get_mat4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_mat4H(material, vulkan_material_get_field_handle(material, name));
}

RENDERER_API vulkan_texture_t* vulkan_material_get_texture2d(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_texture2dH(material, vulkan_material_get_field_handle(material, name));
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
		strcpy(out_field_name, "<UndefinedField>");
		return;
	}
	u16 struct_name_len = (u16)(ptr - full_name);
	u16 field_name_len = (u16)(len - struct_name_len - 1);
	assert(struct_name_len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
	assert(field_name_len < STRUCT_FIELD_MAX_NAME_SIZE);
	memcpy(out_struct_name, full_name, struct_name_len);
	memcpy(out_field_name, ptr + 1, field_name_len);
}

RENDERER_API vulkan_material_field_handle_t vulkan_material_get_field_handle(vulkan_material_t* material, const char* name)
{
	assert(material != NULL);
	assert(material->shader != NULL);

	if(material->shader->material_set_binding_count == 0)
		LOG_WRN("Couldn't get field handle to \"%s\", reason: material->shader->material_set_binding_count == 0\n", name);

	char struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE];
	char field_name[STRUCT_FIELD_MAX_NAME_SIZE];
	get_record_and_field_name(name, struct_name, field_name);

	u16 binding_count = material->shader->material_set_binding_count;
	vulkan_shader_resource_description_t* bindings = material->shader->material_set_bindings;
	u16 index = 0xFFFF;
	u16 uniform_index = 0xFFFF;
	u16 field_handle = STRUCT_FIELD_INVALID_HANDLE;
	for(u16 i = 0, j = 0; i < binding_count; i++)
	{
		vulkan_shader_resource_description_t* binding = &bindings[i];
		if(binding->is_attribute)
			continue;
		if(strcmp(binding->handle.name, struct_name) == 0)
		{
			field_handle = struct_descriptor_get_field_handle(&binding->handle, field_name);
			index = i;
			uniform_index = j;
			if(field_handle == STRUCT_FIELD_INVALID_HANDLE)
				continue;
			break;
		}
		j++;
	}
	if((index != 0xFFFF) && (uniform_index != 0xFFFF))
		return (vulkan_material_field_handle_t) { .index = index, .uniform_index = uniform_index, .field_handle = field_handle };
	LOG_FETAL_ERR("symbol \"%s\" isn't found in the shader resource bindings\n", name);
	return (vulkan_material_field_handle_t) { .index = 0xFFFF, .uniform_index = 0xFFFF, .field_handle = STRUCT_FIELD_INVALID_HANDLE };
}
