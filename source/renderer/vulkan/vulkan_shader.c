#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_shader_module.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <disk_manager/file_reader.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_shader_t* vulkan_shader_new()
{
	vulkan_shader_t* shader = heap_new(vulkan_shader_t);
	memset(shader, 0, sizeof(vulkan_shader_t));
	return shader;
}

static void get_vulkan_constants(VkFormat* out_formats, u32* out_indices)
{
	{
		u32 indices[22 + 1] =
		{	0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 17, 21, 22, 23, 24, 25, 26
		};
		memcpy(&out_indices[0], indices, sizeof(u32) * 23);
	};
	VkFormat vulkan_formats[27] =
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
		//F64
		VK_FORMAT_R64_SFLOAT,
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
	memcpy(&out_formats[0], vulkan_formats, sizeof(VkFormat) * 27);
}

static vulkan_vertex_buffer_layout_description_t decode_vulkan_vertex_info(u64 packed_attributes, u16 location_offset, VkVertexInputRate input_rate)
{
	// get the vulkan constants
	VkFormat vulkan_formats[27];
	u32 indices[23];
	get_vulkan_constants(vulkan_formats, indices);

	VkFormat formats[12];
	u32 offsets[12];
	u16 locations[12];

	u8 bits_per_type = 5;
	u64 bits_mask = ~(0xFFFFFFFFFFFFFFFFULL << bits_per_type);

	u32 i = 0;
	u32 offset = 0;
	while(packed_attributes != 0)
	{
		u64 attribute_type = (packed_attributes & bits_mask);
		if(attribute_type == 0)
		{
			packed_attributes >>= bits_per_type;
			continue;
		}
		VkFormat vulkan_format = vulkan_formats[indices[attribute_type]];
		u32 align = alignof_glsl_type(attribute_type);
		u32 field_offset = ((align - (offset % align)) % align) + offset;
		switch(attribute_type)
		{
			case GLSL_TYPE_MAT3:
				LOG_FETAL_ERR("Vertex attribute decode error: GLSL_MAT3 isn't supported yet!\n");
			break;

			case GLSL_TYPE_MAT4:
				LOG_FETAL_ERR("Vertex attribute decode error: GLSL_MAT4 isn't supported yet!\n");
			break;

			default:/*or MATERIAL_MAT2*/
				formats[i] = vulkan_format;
				offsets[i] = field_offset;
			break;
		}
		packed_attributes >>= bits_per_type;
		offset = field_offset + sizeof_glsl_type(attribute_type);
		locations[i] = i + location_offset;
		i++;
	}
	vulkan_vertex_buffer_layout_description_t info;
	info.input_rate = input_rate;
	info.attribute_count = i;
	info.size = offset;
	info.attribute_formats = heap_newv(VkFormat, info.attribute_count);
	info.attribute_offsets = heap_newv(u32, info.attribute_count);
	info.attribute_locations = heap_newv(u16, info.attribute_count);
	memcpy(info.attribute_offsets, offsets, sizeof(u32) * info.attribute_count);
	memcpy(info.attribute_formats, formats, sizeof(VkFormat) * info.attribute_count);
	memcpy(info.attribute_locations, locations, sizeof(u16) * info.attribute_count);
	return info;
}

static vulkan_vertex_buffer_layout_description_t* decode_vulkan_vertex_infos(u64* per_vertex_attribute_bindings,
												 		u32 per_vertex_attribute_binding_count,
												 		u64* per_instance_attribute_bindings,
												 		u32 per_instance_attribute_binding_count)
{
	// total number of vertex bindings must be equal to number of per vertex bindings + per instance bindings
	u32 binding_count = per_vertex_attribute_binding_count + per_instance_attribute_binding_count;

	// allocate memory
	vulkan_vertex_buffer_layout_description_t* vertex_infos = heap_newv(vulkan_vertex_buffer_layout_description_t, binding_count);
	memset(vertex_infos, 0, sizeof(vulkan_vertex_buffer_layout_description_t) * binding_count);
	
	u32 binding_number = 0;
	u16 location_number_offset = 0;

	// decode each 64 bit number for per vertex vertex bindings
	for(; binding_number < per_vertex_attribute_binding_count; binding_number++)
	{
		vertex_infos[binding_number] = decode_vulkan_vertex_info(per_vertex_attribute_bindings[binding_number], location_number_offset, VK_VERTEX_INPUT_RATE_VERTEX);
		vertex_infos[binding_number].binding = binding_number;
		location_number_offset += vertex_infos[binding_number].attribute_count;
	}
	// decode each 64 bit number for per instance vertex bindings
	for(; binding_number < binding_count; binding_number++)
	{
		vertex_infos[binding_number] = decode_vulkan_vertex_info(per_instance_attribute_bindings[binding_number], location_number_offset, VK_VERTEX_INPUT_RATE_INSTANCE);
		vertex_infos[binding_number].binding = binding_number;
		location_number_offset += vertex_infos[binding_number].attribute_count;
	}
	return vertex_infos;
}

// static vulkan_shader_resource_description_t* create_descriptors(u32 OUT descriptor_count, BUFFER* bytes, buf_ucount_t OUT cursor)
// {
// 	// get the number of descriptors
// 	u16 count = *(u16*)buf_get_ptr_at(bytes, OUT cursor); OUT cursor += 2;

// 	OUT descriptor_count = count;

// 	// if there are no descriptors then return
// 	if(count == 0)
// 		return NULL;

// 	// allocate memory
// 	vulkan_shader_resource_description_t* descriptors = heap_newv(vulkan_shader_resource_description_t, count);
// 	memset(descriptors, 0, sizeof(vulkan_shader_resource_description_t) * count);

// 	u32 temp_cursor = 0;
// 	for(u16 i = 0; i < count; i++, OUT cursor += 4)
// 	{
// 		// get the offset to the descriptor
// 		u32 offset = *(u32*)buf_get_ptr_at(bytes, OUT cursor);

// 		temp_cursor = offset;

// 		vulkan_shader_resource_description_t* descriptor = &descriptors[i];

// 		descriptor->set_number = *(u8*)buf_get_ptr_at(bytes, temp_cursor); temp_cursor += 1;
// 		descriptor->binding_number = *(u8*)buf_get_ptr_at(bytes, temp_cursor); temp_cursor += 1;
// 		// assert(descriptor->set_number < 255);
// 		// assert(descriptor->binding_number < 255);
// 		u32 descriptor_info = *(u32*)buf_get_ptr_at(bytes, temp_cursor); temp_cursor += 4;
// 		descriptor->is_push_constant = (descriptor_info & (1U << 16)) ? true : false;
// 		descriptor->is_opaque = (descriptor_info & (1U << 15)) ? true : false;
// 		descriptor->is_uniform = (descriptor_info & (1U << 14)) ? true : false;
// 		descriptor->is_per_vertex_attribute = (descriptor_info & (1UL << 18));
// 		descriptor->is_per_instance_attribute = (descriptor_info & (1UL << 17));
// 		descriptor->handle.type = descriptor_info & 0xFFU;
// 		descriptor->stage_flags = 0;
// 		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_VERTEX)))
// 			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_VERTEX);
// 		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_TESSELLATION)))
// 			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_TESSELLATION);
// 		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_GEOMETRY)))
// 			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_GEOMETRY);
// 		if(descriptor_info & (1UL << (13 - SHADER_COMPILER_SHADER_STAGE_FRAGMENT)))
// 			descriptor->stage_flags |= (1UL << VULKAN_SHADER_TYPE_FRAGMENT);
		
// 		// get the name of the descriptor
// 		const char* name = buf_get_ptr_at(bytes, temp_cursor);
// 		u32 len = strlen(name);
// 		assert(len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
// 		temp_cursor += len + 1;
// 		strcpy(descriptor->handle.name, name);

// 		if(descriptor->handle.type == GLSL_TYPE_BLOCK)
// 		{
// 			// ignore the block name
// 			name = buf_get_ptr_at(bytes, temp_cursor);
// 			len = strlen(name);
// 			assert(len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
// 			temp_cursor += len + 1;
// 		}

// 		log_msg("Descriptor[%u]: (set = %u, binding = %u), stage_flags = %u, is_push_constant = %s, is_uniform = %s, is_opaque = %s, is_block = %s, name = %s\n", 
// 			i, descriptor->set_number, descriptor->binding_number, descriptor->stage_flags,
// 			descriptor->is_push_constant ? "true" : "false", descriptor->is_uniform ? "true" : "false", descriptor->is_opaque ? "true" : "false", (descriptor->handle.type == GLSL_TYPE_BLOCK) ? "true" : "false",
// 			descriptor->handle.name);

// 		if(descriptor->handle.type == GLSL_TYPE_BLOCK)
// 		{
// 			// get the number of fields in this uniform block
// 			descriptor->handle.field_count = *(u16*)buf_get_ptr_at(bytes, temp_cursor); temp_cursor += 2;
// 			log_msg("Field Count: %u\n", descriptor->handle.field_count);

// 			// store the index of the list of fields of this uniform block in the fields buffer
// 			indices[i] = buf_get_element_count(&fields);

// 			struct_field_t* fields = NULL;

// 			if(descriptor->handle.field_count > 0)
// 				fields = heap_newv(struct_field_t, descriptor->handle.field_count);
			
// 			for(u16 j = 0; j < descriptor->handle.field_count; j++)
// 			{
// 				// get the field info
// 				u32 field_info = *(u32*)buf_get_ptr_at(bytes, temp_cursor); temp_cursor += 4;
				
// 				// get the type of the fields
// 				u8 type = field_info & 0xFFUL;
				
// 				// create a field object
// 				fields[j] = (struct_field_t) { .type = type, .size = sizeof_glsl_type(type), .alignment = alignof_glsl_type(type) };
				
// 				// get the name of this field
// 				const char* name = buf_get_ptr_at(bytes, temp_cursor);
// 				u32 len = strlen(name);
// 				assert(len < STRUCT_FIELD_MAX_NAME_SIZE);
// 				temp_cursor += len + 1;
// 				strcpy(fields[j].name, name);

// 				log_msg("Field[%u]: type = %u, size = %u, align = %u, name = %s\n", j, fields[j].type, fields[j].size, fields[j].alignment, fields[j].name);
// 			}
// 			descriptor->handle.fields = fields;
// 			struct_descriptor_recalculate(&descriptor->handle);
// 		}
// 	}

// 	OUT cursor = temp_cursor;
	
// 	return descriptors;
// }

// static vulkan_vertex_buffer_layout_description_t* create_vertex_infos(u32 OUT vertex_info_count, BUFFER* bytes, buf_ucount_t OUT cursor)
// {
// 	// get the vulkan constants
// 	VkFormat vulkan_formats[27];
// 	u32 sizes[23];
// 	u32 indices[23];
// 	u32 alignments[23];
// 	get_vulkan_constants(vulkan_formats, sizes, indices, alignments);

// 	// get the vertex attribute count across all the vertex bindings
// 	u32 binding_count;
// 	vulkan_shader_resource_description_t* binding_descriptions = create_descriptors(&binding_count, bytes, cursor);

// 	// allocate memory
// 	vulkan_vertex_buffer_layout_description_t* vertex_infos = heap_newv(vulkan_vertex_buffer_layout_description_t, binding_count);
// 	memset(vertex_infos, 0, sizeof(vulkan_vertex_buffer_layout_description_t) * binding_count);

// 	typedef struct attribute_info_t { BUFFER locations, formats, offsets; } attribute_info_t;
// 	dictionary_t bindings = dictionary_create(u32, attribute_info_t, 1, dictionary_key_comparer_u32);

// 	// iterate through each vertex attribute across all the vertex bindings
// 	for(u32 i = 0, index = 0; i < binding_count; i++)
// 	{
// 		// this must be an attribute
// 		assert(binding_descriptions[i].is_attribute);

// 		// use vertex binding number as a key
// 		u32 key = binding_descriptions[i].vertex_attrib_binding_number;
// 		u32 location = binding_descriptions[i].vertex_attrib_location_number;
// 		u64 glsl_type = convert_to_glsl_type(binding_descriptions[i].handle.type);
// 		VkFormat format = vulkan_formats[indices[glsl_type]];
// 		u32 alignment = alignments[glsl_type];
// 		u32 size = sizes[glsl_type];

// 		// if the binding number 'key' is not present then create new attribute_info_t object
// 		if(!dictionary_contains(&bindings, &key))
// 		{
// 			attribute_info_t info = 
// 			{ 
// 				.locations = buf_create(sizeof(u32), 1, 0), 
// 				.formats = buf_create(sizeof(VkFormat), 1, 0), 
// 				.offsets = buf_create(sizeof(u32), 1, 0) 
// 			};
// 			dictionary_push(&bindings, &key, &info);

// 			// add zero offset to be popped later
// 			u32 offset = 0;
// 			attribute_info_t* attribute_info = dictionary_get_value_ptr_at(&bindings, dictionary_get_count(&bindings) - 1);
// 			buf_push(&attribute_info->offsets, &offset);

// 			// create vertex info object for the binding 'key'
// 			vertex_infos[index] = (vulkan_vertex_buffer_layout_description_t)
// 			{
// 				.binding = key,
// 				.input_rate = binding_descriptions[i].is_per_vertex_attribute ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE
// 			};
// 		}

// 		// get pointer to the attribute info object for binding 'key'
// 		attribute_info_t* attribute_info = dictionary_get_value_ptr(&bindings, &key);

// 		// TODO: These checks should be done at the compilation time, not at the runtime
// 		// if(buf_find_index_of(locations, &location, buf_u16_comparer) != BUF_INVALID_INDEX)
// 		// 	LOG_FETAL_ERR("Multiple vertex attributes have the same layout location \"%u\", which is not allowed!\n", location);
			
// 		// add the location
// 		buf_push(&attribute_info->locations, &location);
// 		// add the format
// 		buf_push(&attribute_info->formats, &format);

// 		u32 offset;
// 		// pop the previous offset
// 		buf_pop(&attribute_info->offsets, &offset);
// 		// snap the offset to the correct alignment for this field/attribute
// 		offset += (alignment - (offset % alignment)) % alignment;
// 		// add the offset
// 		buf_push(&attribute_info->offsets, &offset);
// 		// increment the offset by the size of this field/attribute
// 		offset += size;
// 		// save the offset for next iteration
// 		buf_push(&attribute_info->offsets, &offset);
// 	}

// 	for(u32 i = 0; i < binding_count; i++)
// 	{
// 		vulkan_vertex_buffer_layout_description_t* vinfo = &vertex_infos[i];
// 		attribute_info_t* ainfo = dictionary_get_value_ptr_at(&bindings, i);

// 		// the final stride would be the last saved offset
// 		vinfo->size = *(u32*)buf_peek_ptr(&ainfo->offsets);
// 		// assign the number of attribute count
// 		vinfo->attribute_count = buf_get_element_count(&ainfo->locations);
// 		// assign the internal pointers to their corresponding destinations
// 		vinfo->attribute_locations = buf_get_ptr(&ainfo->locations);
// 		vinfo->attribute_formats = buf_get_ptr(&ainfo->formats);
// 		vinfo->attribute_offsets = buf_get_ptr(&ainfo->offsets);
// 	}

// 	// free the temporary dictionary buffer
// 	dictionary_free(&bindings);

// 	// destroy the vulkan resource descriptors because we are done creating vertex infos out of them
// 	destroy_set_binding_descriptors(binding_descriptions, binding_count);

// 	// set the number of vulkan vertex info objects
// 	OUT vertex_info_count = binding_count;

// 	// return pointer to the final vulkan vertex info objects
// 	return vertex_infos;
// }

// static vulkan_render_pass_description_t* create_render_pass_descriptions(u32 OUT render_pass_description_count, BUFFER* bytes, buf_ucount_t OUT cursor)
// {
// 	u16 description_count = DEREF_TO(buf_get_ptr_at(bytes, OUT cursor), u16); OUT cursor += 2;
// 	if(description_count == 0)
// 	{
// 		OUT render_pass_description_count = 0;
// 		return NULL;
// 	}
// 	vulkan_render_pass_description_t* descriptions = heap_newv(vulkan_render_pass_description_t, description_count);
// 	memset(descriptions, 0, sizeof(vulkan_render_pass_description_t) * descriptor_count);
// 	for(u16 i = 0; i < descriptor_count; i++)
// 	{
// 		// TODO:
// 	}

// 	OUT render_pass_description_count = description_count;
// 	return descriptions;
// }

// RENDERER_API vulkan_shader_t* vulkan_shader_load(vulkan_renderer_t* renderer, vulkan_shader_load_info_t* file_info)
// {
// 	// load the file into the main memory
// 	BUFFER* bytes = load_binary_from_file(file_info->path);
// 	// set the cursor to the bytes zero intially
// 	buf_ucount_t cursor = 0;

// 	// deserialize the bytes into vulkan_shader_file_t object
// 	vulkan_shader_file_t* shader_file = heap_new(vulkan_shader_file_t);
// 	vulkan_shader_file_result_t result = vulkan_shader_file_deserialize(buf_get_ptr(bytes), buf_get_element_count(bytes), shader_file);
// 	ASSERT(VULKAN_SHADER_FILE_RESULT_CHECK(result) == VULKAN_SHADER_FILE_RESULT_SUCCESS);

// 	// TODO:

// 	// create material set bindings
// 	u32 material_set_binding_count;
// 	vulkan_shader_resource_description_t* material_set_bindings = create_descriptors(&material_set_binding_count, bytes, &cursor);

// 	// create vertex attribute infos
// 	u32 vertex_info_count;
// 	vulkan_vertex_buffer_layout_description_t* vertex_infos = file_info->is_vertex_attrib_from_file ? create_vertex_infos(&vertex_info_count, bytes, &cursor) : 
// 										decode_vulkan_vertex_infos(file_info->per_vertex_attribute_bindings,
// 																   file_info->per_vertex_attribute_binding_count,
// 																   file_info->per_instance_attribute_bindings,
// 																   file_info->per_instance_attribute_binding_count);
// 	if(file_info->is_vertex_attrib_from_file)
// 		vertex_info_count = file_info->per_vertex_attribute_binding_count + file_info->per_instance_attribute_binding_count;

// 	// create render pass descriptions
// 	u32 render_pass_description_count;
// 	vulkan_render_pass_description_t* render_pass_descriptions = create_render_pass_descriptions(&render_pass_description_count, bytes, &cursor);

// 	// create vulkan shader
// 	vulkan_shader_create_info_t create_info = 
// 	{
// 		.material_set_bindings = material_set_bindings,
// 		.material_set_binding_coiunt = material_set_binding_count,

// 		.vertex_infos = vertex_infos,
// 		.vertex_info_count,

// 		.render_pass_descriptions = render_pass_descriptions,
// 		.render_pass_description_count = render_pass_description_count
// 	};

// 	// free up the allocated memories in the main memory because vulkan shader creates a deep copy internally
// 	buf_free(bytes);
// 	heap_free(material_set_bindings);
// 	heap_free(vertex_infos);
// 	heap_free(render_pass_descriptions);
// 	vulkan_shader_file_destroy(shader_file);
// 	heap_free(shader_file);

// 	return vulkan_shader_create(renderer, &create_info);
// }

static vulkan_push_constant_t create_vulkan_push_constant(vulkan_shader_resource_description_t* material_set_bindings, u32 material_set_binding_count)
{
	// holds the indices of the material set binding descriptors which are push constants
	u32* descriptor_indices = heap_newv(u32, material_set_binding_count);

	// TODO: add an extra parameter is_zero, if true then each block allocated in the buffer would be first zeroed-out otherwise uninitialized
	BUFFER ranges = buf_create(sizeof(VkPushConstantRange), 0, 0);

	u32 range_count = 0;
	for(u32 i = 0; i < material_set_binding_count; i++)
	{
		vulkan_shader_resource_description_t* descriptor = &material_set_bindings[i];

		// if this is not a push constant then skip it
		if(!descriptor->is_push_constant) continue;

		// sanity check
		assert(descriptor->handle.type == GLSL_TYPE_BLOCK);
		
		VkShaderStageFlagBits stage_flags = 0;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
			stage_flags |= VK_SHADER_STAGE_VERTEX_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
			stage_flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
			stage_flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
			stage_flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

		// create push constant range and added it to the list of ranges
		VkPushConstantRange range = 
		{
			.stageFlags = stage_flags,
			.offset = descriptor->push_constant_range_offset,
			.size = struct_descriptor_sizeof(&descriptor->handle)
		};
		buf_push(&ranges, &range);

		descriptor_indices[range_count] = i;
		++range_count;
	}

	// if there are no push constants then return NULL & zeros
	if(range_count == 0)
	{
		return (vulkan_push_constant_t)
		{
			.ranges = NULL,
			.range_count = 0,
			.buffer = NULL,
			.buffer_size = 0
		};
	}

	VkPushConstantRange max = DEREF_TO(VkPushConstantRange, buf_get_ptr_at(&ranges, 0));
	
	// find the max (offset + size)
	for(u16 i = 1; i < range_count; i++)
	{
		VkPushConstantRange range = DEREF_TO(VkPushConstantRange, buf_get_ptr_at(&ranges, i));
		if((max.offset + max.size) < (range.offset + range.size))
			max = range;
	}

	// allocate memory with the size of the push_constant_buffer to (max.offset + max.size)
	void* push_constant_buffer = heap_alloc(max.offset + max.size);

	// map each range
	for(u16 i = 0; i < range_count; i++)
	{
		VkPushConstantRange range = DEREF_TO(VkPushConstantRange, buf_get_ptr_at(&ranges, i));
		// offsets must be in inceasing order and should be in the bounds of the buffer size which is (max.offset + max.size)
		assert(range.offset < (max.offset + max.size));

		// map
		struct_descriptor_map(&material_set_bindings[descriptor_indices[i]].handle, push_constant_buffer + range.offset);
	}

	heap_free(descriptor_indices);

	// return the vulkan_push_constant_t object
	return (vulkan_push_constant_t)
	{
		.ranges = buf_get_ptr(&ranges),
		.range_count = range_count,
		.buffer = push_constant_buffer,
		.buffer_size = max.offset + max.size
	};
}

static void destroy_vulkan_push_constant(vulkan_push_constant_t* push_constant)
{
	if((push_constant->ranges == NULL) || (push_constant->buffer == NULL))
		return;
	/* NOTE: this hasn't been allocated with heap_*, instead malloc internally by BUFFER */
	free(push_constant->ranges);
	heap_free(push_constant->buffer);
}

static vulkan_vertex_buffer_layout_description_t* create_deep_copy_of_vulkan_vertex_infos(vulkan_vertex_buffer_layout_description_t* vertex_infos, u32 vertex_info_count)
{
	if((vertex_infos == NULL) || (vertex_info_count == 0))
		return NULL;
	vulkan_vertex_buffer_layout_description_t* copy_infos = heap_newv(vulkan_vertex_buffer_layout_description_t, vertex_info_count);
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		copy_infos[i] = vertex_infos[i];

		u32 attribute_count = vertex_infos[i].attribute_count;

		copy_infos[i].attribute_formats = heap_newv(VkFormat, attribute_count);
		copy_infos[i].attribute_offsets = heap_newv(u32, attribute_count);
		copy_infos[i].attribute_locations = heap_newv(u16, attribute_count);

		memcpy(copy_infos[i].attribute_formats, vertex_infos[i].attribute_formats, sizeof(VkFormat) * attribute_count);
		memcpy(copy_infos[i].attribute_offsets, vertex_infos[i].attribute_offsets, sizeof(VkFormat) * attribute_count);
		memcpy(copy_infos[i].attribute_locations, vertex_infos[i].attribute_locations, sizeof(VkFormat) * attribute_count);
	}
	return copy_infos;
}

static void destroy_vulkan_vertex_infos(vulkan_vertex_buffer_layout_description_t* vertex_infos, u32 vertex_info_count)
{
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		heap_free(vertex_infos[i].attribute_formats);
		heap_free(vertex_infos[i].attribute_offsets);
		heap_free(vertex_infos[i].attribute_locations);
	}
	heap_free(vertex_infos);
}

static vulkan_shader_resource_description_t* create_deep_copy_of_set_binding_descriptors(vulkan_shader_resource_description_t* descriptors, u32 descriptor_count)
{
	vulkan_shader_resource_description_t* copy_descriptors = heap_newv(vulkan_shader_resource_description_t, descriptor_count);
	for(u32 i = 0; i < descriptor_count; i++)
	{
		copy_descriptors[i] = descriptors[i];
		if(descriptors[i].handle.field_count != 0)
		{
			copy_descriptors[i].handle.fields = heap_newv(struct_field_t, descriptors[i].handle.field_count);
			memcpy(copy_descriptors[i].handle.fields, descriptors[i].handle.fields, sizeof(struct_field_t) * descriptors[i].handle.field_count);
			struct_descriptor_recalculate(&copy_descriptors[i].handle);
		}
		else
			copy_descriptors[i].handle.fields = NULL;
	}
	return copy_descriptors;
}

static void destroy_set_binding_descriptors(vulkan_shader_resource_description_t* descriptors, u32 descriptor_count)
{
	for(u32 i = 0; i < descriptor_count; i++)
	{
		if(descriptors[i].handle.fields != NULL)
			heap_free(descriptors[i].handle.fields);
	}
	heap_free(descriptors);
}

typedef struct vulkan_pipeline_common_data_t
{
	vulkan_vertex_buffer_layout_description_t* vertex_attribute_bindings;
	u32 vertex_attribute_binding_count;
	VkPushConstantRange* push_constant_ranges;
	u32 push_constant_range_count;
	vulkan_descriptor_set_layout_t material_set_layout;
	vulkan_graphics_pipeline_description_t* pipeline_descriptions;
} vulkan_pipeline_common_data_t;

static u32 get_framebuffer_count_from_render_pass_type(vulkan_render_pass_type_t type, u32 swapchain_image_count)
{
	switch(type)
	{
		case VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER: return 1;
		case VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET: return swapchain_image_count;
		default:
			LOG_FETAL_ERR("Unsupported render pass type %u\n", type);
	}
	return 0;
}

static VkFormat get_image_format_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_FORMAT_D32_SFLOAT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_STENCIL isn't supported yet!\n");
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL isn't supported yet!\n");
		default:
			LOG_FETAL_ERR("Unsupported attachment type %u for any of the image format\n", type);
	}
	return VK_FORMAT_UNDEFINED;
}

static void write_render_pass_descriptors(vulkan_render_pass_t* previous_pass, vulkan_render_pass_description_t* previous_pass_description,
										  vulkan_render_pass_t* pass, vulkan_render_pass_description_t* pass_description)
{
	if(previous_pass != NULL)
	{
		u32 count = pass_description->render_set_binding_count;
		for(u32 i = 0; i < count; i++)
			vulkan_descriptor_set_write_texture(&pass->render_set, pass_description->render_set_bindings[i].binding_number,
				CAST_TO(vulkan_texture_t*, &previous_pass->attachments[pass_description->input_attachments[i]]));
	}

	// write subpass descriptions
	u32 subpass_count = pass->subpass_count;
	for(u32 i = 0; i < subpass_count; i++)
	{
		vulkan_subpass_description_t* subpass = &pass_description->subpass_descriptions[i];
		u32 count = subpass->sub_render_set_binding_count;
		for(u32 j = 0; j < count; j++)
			vulkan_descriptor_set_write_texture(&pass->sub_render_sets[i], subpass->sub_render_set_bindings[j].binding_number,
				CAST_TO(vulkan_texture_t*, &pass->attachments[subpass->input_attachments[j] - pass->supplementary_attachment_count]));
	}
}

static vulkan_render_pass_create_info_t* convert_render_pass_description_to_create_info(vulkan_renderer_t* renderer, vulkan_render_pass_description_t* pass, vulkan_render_pass_description_t* next_pass)
{
	vulkan_render_pass_create_info_t* create_info = heap_new(vulkan_render_pass_create_info_t);
	memzero(create_info, vulkan_render_pass_create_info_t);

	// number of frame buffers to create
	create_info->framebuffer_count = get_framebuffer_count_from_render_pass_type(pass->type, renderer->swapchain->image_count);
	
	// list of attachments to bound for each frame buffer
	assert(pass->attachment_count > 0);
	create_info->attachment_description_count = pass->attachment_count;
	create_info->attachment_descriptions = heap_newv(VkAttachmentDescription, pass->attachment_count);
	create_info->attachment_usages = heap_newv(vulkan_attachment_next_pass_usage_t, pass->attachment_count);

	if(pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET)
	{
		assert(create_info->framebuffer_count == renderer->swapchain->image_count);
		create_info->supplementary_attachments = heap_newv(VkImageView, renderer->swapchain->image_count);
		create_info->supplementary_attachment_count = 1;
		for(u32 i = 0; i < renderer->swapchain->image_count; i++)
			create_info->supplementary_attachments[i] = renderer->swapchain->vo_image_views[i];
	}

	for(u32 i = 0; i < pass->attachment_count; i++)
	{
		bool is_supplementary = i < create_info->supplementary_attachment_count;
		VkAttachmentStoreOp store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkAttachmentLoadOp load_op = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ? 
									 (((pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET) && is_supplementary) ?
									  VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR) : VK_ATTACHMENT_LOAD_OP_CLEAR;

		VkImageLayout final_layout = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ? 
									 (((pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET) && is_supplementary) ?
									  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		vulkan_attachment_next_pass_usage_t usage = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;

		// supplementary attachments would always be swapchain image
		if(is_supplementary)
		{
			assert(pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR);
			store_op = VK_ATTACHMENT_STORE_OP_STORE;
			usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;
		}

		for(u32 j = 0; j < pass->subpass_description_count; j++)
		{
			u32 sub_input_count = pass->subpass_descriptions[j].input_attachment_count;
			for(u32 k = 0; k < sub_input_count; k++)
			{
				if(i == pass->subpass_descriptions[j].input_attachments[k])
					usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT;
			}
		}

		u32 input_count = (next_pass == NULL) ? 0 : next_pass->input_attachment_count;
		for(u32 j = 0; j < input_count; j++)
		{
			if(next_pass->input_attachments[j] == i)
			{
				store_op = VK_ATTACHMENT_STORE_OP_STORE;
				final_layout = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ?
								VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL :
								VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED;
			}
		}

		create_info->attachment_descriptions[i] = (VkAttachmentDescription)
		{
			.format = get_image_format_from_attachment_type(pass->attachments[i]),
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = load_op,
			.storeOp = store_op,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = final_layout
		};

		create_info->attachment_usages[i] = usage;
	}

	create_info->render_set_bindings = pass->render_set_bindings;
	create_info->render_set_binding_count = pass->render_set_binding_count;

	assert(pass->subpass_description_count > 0);
	create_info->subpass_count = pass->subpass_description_count;
	create_info->subpasses = heap_newv(vulkan_subpass_create_info_t, pass->subpass_count);
	memzerov(create_info->subpasses, vulkan_subpass_create_info_t, pass->subpass_count);
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		create_info->subpasses[i].sub_render_set_bindings = pass->subpass_descriptions[i].sub_render_set_bindings;
		create_info->subpasses[i].sub_render_set_binding_count = pass->subpass_descriptions[i].sub_render_set_binding_count;
		create_info->subpasses[i].pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
		
		create_info->subpasses[i].color_attachment_count = pass->subpass_descriptions[i].color_attachment_count;
		
		if(pass->subpass_descriptions[i].color_attachment_count > 0)
			create_info->subpasses[i].color_attachments = heap_newv(VkAttachmentReference, create_info->subpasses[i].color_attachment_count);
		
		for(u32 j = 0; j < create_info->subpasses[i].color_attachment_count; j++)
		{
			u32 index = pass->subpass_descriptions[i].color_attachments[j];
			create_info->subpasses[i].color_attachments[j].attachment = index;
			create_info->subpasses[i].color_attachments[j].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		create_info->subpasses[i].input_attachment_count = pass->subpass_descriptions[i].input_attachment_count;
		if(pass->subpass_descriptions[i].input_attachment_count > 0)
			create_info->subpasses[i].input_attachments = heap_newv(VkAttachmentReference, create_info->subpasses[i].input_attachment_count);
		
		for(u32 j = 0; j < create_info->subpasses[i].input_attachment_count; j++)
		{
			u32 index = pass->subpass_descriptions[i].input_attachments[j];
			create_info->subpasses[i].input_attachments[j].attachment = index;
			create_info->subpasses[i].input_attachments[j].layout = (pass->attachments[index] == VULKAN_ATTACHMENT_TYPE_COLOR) ?
																	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL :
																	VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}

		if(pass->subpass_descriptions[i].depth_stencil_attachment != U32_MAX)
		{
			VkAttachmentReference* depth_stencil_attachment = heap_new(VkAttachmentReference);
			assert(pass->attachments[pass->subpass_descriptions[i].depth_stencil_attachment] == VULKAN_ATTACHMENT_TYPE_DEPTH);
			depth_stencil_attachment->attachment = pass->subpass_descriptions[i].depth_stencil_attachment;
			depth_stencil_attachment->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			create_info->subpasses[i].depth_stencil_attachment = depth_stencil_attachment;
		}

		// for now we won't be using preserve attachments
		create_info->subpasses[i].preserve_attachments = NULL;
		create_info->subpasses[i].preserve_attachment_count = 0;
	}

	return create_info;
}

static vulkan_shader_render_pass_t* create_shader_render_passes(vulkan_renderer_t* renderer, vulkan_render_pass_description_t* descriptions, u32 description_count, vulkan_pipeline_common_data_t* common_data)
{
	// allocate memory
	vulkan_shader_render_pass_t* passes = heap_newv(vulkan_shader_render_pass_t, description_count);

	BUFFER set_layouts = buf_create(sizeof(VkDescriptorSetLayout), 1, 0);
	BUFFER shader_modules = buf_create(sizeof(vulkan_shader_module_t), 1, 0);

	// push camera set layout, at CAMERA_SET = 0
	buf_push(&set_layouts, &renderer->camera_set_layout.vo_handle);

	// push global set layout, at GLOBAL_SET = 1
	buf_push(&set_layouts, &renderer->global_set_layout.vo_handle);

	vulkan_render_pass_t* previous_pass = NULL;
	for(u32 i = 0; i < description_count; i++)
	{
		// create a render pass object in the pool, no duplicate render pass would be created
		vulkan_render_pass_create_info_t* create_info = convert_render_pass_description_to_create_info(renderer, &descriptions[i], (i < (description_count - 1)) ? &descriptions[i + 1] : NULL);
		vulkan_render_pass_handle_t handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, create_info);
		heap_free(create_info);

		// assing the handle and subpass count
		passes[i].handle = handle;
		u32 subpass_count = passes[i].subpass_count = descriptions[i].subpass_count;

		// allocate memory for pipeline layouts & graphics pipelines
		vulkan_pipeline_layout_t* pipeline_layouts = heap_newv(vulkan_pipeline_layout_t, subpass_count);
		memzerov(pipeline_layouts, vulkan_pipeline_layout_t, subpass_count);
		vulkan_graphics_pipeline_t* pipelines = heap_newv(vulkan_graphics_pipeline_t, subpass_count);
		memzerov(pipelines, vulkan_graphics_pipeline_t, subpass_count);

		vulkan_render_pass_t* render_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, handle);
		write_render_pass_descriptors(previous_pass, (i > 0) ? &descriptions[i - 1] : NULL,
									  render_pass, &descriptions[i]);
		previous_pass = render_pass;

		// push render set layout, at RENDER_SET = 2
		buf_push(&set_layouts, &render_pass->render_set_layout.vo_handle);

		for(u32 j = 0; j < subpass_count; j++)
		{
			// push sub render set layout, at SUB_RENDER_SET = 3
			buf_push(&set_layouts, &render_pass->sub_render_set_layouts[j].vo_handle);
			// push material set layout, at MATERIAL_SET = 4
			buf_push(&set_layouts, &common_data->material_set_layout.vo_handle);
			// push object set layout, at OBJECT_SET = 5
			buf_push(&set_layouts, &renderer->object_set_layout.vo_handle);

			// create pipeline layout for this subpass
			vulkan_pipeline_layout_create_info_t layout_create_info =
			{
				.vo_push_constant_ranges = common_data->push_constant_ranges,
				.push_constant_range_count = common_data->push_constant_range_count,
				.vo_set_layouts = buf_get_ptr(&set_layouts),
				.set_layout_count = buf_get_element_count(&set_layouts)
			};
			vulkan_pipeline_layout_create_no_alloc(renderer, &layout_create_info, &pipeline_layouts[j]);

			// pop out the set layouts { SUB_RENDER_SET, MATERIAL_SET, and OBJECT_SET }
			buf_pop(&set_layouts, NULL);
			buf_pop(&set_layouts, NULL);
			buf_pop(&set_layouts, NULL);

			buf_clear(&shader_modules, NULL);
			vulkan_graphics_pipeline_description_t* pipeline_description = &common_data->pipeline_descriptions[descriptions[i].subpass_descriptions[j].pipeline_description_index];
			u32 module_count = pipeline_description->spirv_code_count;
			
			for(u32 k = 0; k < module_count; k++)
			{
				vulkan_shader_module_create_info_t module_create_info = REINTERPRET_TO(vulkan_shader_module_create_info_t, pipeline_description->spirv_codes[k]);
				buf_push_pseudo(&shader_modules, 1);
				vulkan_shader_module_create_no_alloc(renderer, &module_create_info, CAST_TO(vulkan_shader_module_t*, buf_peek_ptr(&shader_modules)));
			}

			// create graphics pipeline for this subpass
			vulkan_graphics_pipeline_create_info_t pipeline_create_info = 
			{
				.layout = &pipeline_layouts[j],
				.settings = pipeline_description->settings,
				.shader_modules = CAST_TO(vulkan_shader_module_t*, buf_get_ptr(&shader_modules)),
				.shader_module_count = module_count,
				.vertex_attribute_bindings = common_data->vertex_attribute_bindings,
				.vertex_attribute_binding_count = common_data->vertex_attribute_binding_count,
				.render_pass = render_pass,
				.subpass_index = j
			};
			vulkan_graphics_pipeline_create_no_alloc(renderer, &pipeline_create_info, &pipelines[j]);

			for(u32 k = 0; k < module_count; k++)
			{
				vulkan_shader_module_destroy(buf_peek_ptr(&shader_modules));
				buf_pop(&shader_modules, NULL);
			}
		}

		// pop out the RENDER_SET layout
		buf_pop(&set_layouts, NULL);

		passes[i].pipeline_layouts = pipeline_layouts;
		passes[i].pipelines = pipelines;
	}

	// pop out the GLOBAL_SET layout
	buf_pop(&set_layouts, NULL);

	// pop out the CAMERA_SET layout
	buf_pop(&set_layouts, NULL);

	buf_free(&set_layouts);
	buf_free(&shader_modules);
	
	return passes;
}

RENDERER_API vulkan_shader_t* vulkan_shader_create(vulkan_renderer_t* renderer, vulkan_shader_create_info_t* create_info)
{
	vulkan_shader_t* shader = vulkan_shader_new();
	shader->renderer = renderer;

	shader->handle = VULKAN_SHADER_HANDLE_INVALID;
	shader->push_constant = create_vulkan_push_constant(create_info->material_set_bindings, create_info->material_set_binding_count);
	shader->vertex_infos = create_deep_copy_of_vulkan_vertex_infos(create_info->vertex_infos, create_info->vertex_info_count);
	shader->vertex_info_count = create_info->vertex_info_count;
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->material_set_bindings, create_info->material_set_binding_count, &shader->material_set_layout);
	shader->material_set_bindings = create_deep_copy_of_set_binding_descriptors(create_info->material_set_bindings, create_info->material_set_binding_count);
	shader->material_set_binding_count = create_info->material_set_binding_count;
	vulkan_pipeline_common_data_t common_data = 
	{
		.vertex_attribute_bindings = shader->vertex_infos,
		.vertex_attribute_binding_count = shader->vertex_info_count,
		.push_constant_ranges = shader->push_constant.ranges,
		.push_constant_range_count = shader->push_constant.range_count,
		.material_set_layout = shader->material_set_layout,
		.pipeline_descriptions = create_info->pipeline_descriptions
	};
	shader->render_passes = create_shader_render_passes(renderer, create_info->render_pass_descriptions, create_info->render_pass_description_count, &common_data);
	shader->render_pass_count = create_info->render_pass_description_count;

	return shader;
}

static vulkan_shader_create_info_t* deserialize_shader_create_info(void* bytes, u32 length)
{
	vulkan_shader_create_info_t* create_info = heap_new(vulkan_shader_create_info_t);
	// TODO:
	return create_info;
}

RENDERER_API vulkan_shader_t* vulkan_shader_load(vulkan_renderer_t* renderer, vulkan_shader_load_info_t* load_info)
{
	BUFFER* buffer = load_binary_from_file(load_info->path);
	vulkan_shader_create_info_t* create_info = deserialize_shader_create_info(buffer->bytes, buffer->element_count);
	buf_free(buffer);
	return vulkan_shader_create(renderer, create_info);
}

RENDERER_API void vulkan_shader_destroy(vulkan_shader_t* shader)
{
	shader->handle = VULKAN_SHADER_HANDLE_INVALID;
	vulkan_descriptor_set_layout_destroy(&shader->material_set_layout);
	u32 count = shader->render_pass_count;
	for(u32 i = 0; i < count; i++)
	{
		for(u32 j = 0; j < shader->render_passes[i].subpass_count; j++)
		{
			vulkan_pipeline_layout_destroy(&shader->render_passes[i].pipeline_layouts[j]);
			vulkan_graphics_pipeline_destroy(&shader->render_passes[i].pipelines[j]);
		}
	}
}

RENDERER_API void vulkan_shader_release_resources(vulkan_shader_t* shader)
{
	destroy_vulkan_push_constant(&shader->push_constant);
	destroy_vulkan_vertex_infos(shader->vertex_infos, shader->vertex_info_count);
	destroy_set_binding_descriptors(shader->material_set_bindings, shader->material_set_binding_count);
	// TODO
	// heap_free(shader);
}

RENDERER_API vulkan_graphics_pipeline_t* vulkan_shader_get_pipeline(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index)
{
	u32 count = shader->render_pass_count;
	for(u32 i = 0; i < count; i++)
	{
		if(shader->render_passes[i].handle == handle)
		{
			assert(subpass_index < shader->render_passes[i].subpass_count);
			return &shader->render_passes[i].pipelines[subpass_index];
		}
	}
	return NULL;
}

RENDERER_API vulkan_pipeline_layout_t* vulkan_shader_get_pipeline_layout(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index)
{
	u32 count = shader->render_pass_count;
	for(u32 i = 0; i < count; i++)
	{
		if(shader->render_passes[i].handle == handle)
		{
			assert(subpass_index < shader->render_passes[i].subpass_count);
			return &shader->render_passes[i].pipeline_layouts[subpass_index];
		}
	}
	return NULL;
}


// static vulkan_stage_shader_t** create_stage_shaders(vulkan_renderer_t* renderer, BUFFER* shader_binary, u32 cursor, u8* stage_count)
// {
// 	assert(cursor != 0xFFFFFFFF);
	
// 	// for now render pass count must be 1
// 	u8 render_pass_count = *(u8*)buf_get_ptr_at(shader_binary, cursor); cursor++;
// 	assert(render_pass_count == 1);

// 	u8 shader_mask = *(u8*)buf_get_ptr_at(shader_binary, cursor); cursor++;
// 	u8 shader_count = 0; for(u8 i = 0; i < 4; i++) { if(shader_mask & (1 << i)) shader_count++; }
// 	*stage_count = shader_count;

// 	vulkan_stage_shader_t** shaders = heap_newv(vulkan_stage_shader_t*, shader_count);
// 	for(u8 i = 0, count = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
// 	{
// 		u8 bit = shader_mask & (1 << i);
// 		if(bit == 0) continue;
// 		u32 offset = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
// 		u32 length = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
// 		switch(bit)
// 		{
// 			case 1 << SHADER_COMPILER_SHADER_STAGE_VERTEX:
// 				//Vertex shader spirv
// 				u32* spirv = buf_get_ptr_at(shader_binary, offset);
// 				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_VERTEX);
// 				count++;
// 				break;
// 			case 1 << SHADER_COMPILER_SHADER_STAGE_TESSELLATION:
// 				//Tessellation shader spirv
// 				spirv = buf_get_ptr_at(shader_binary, offset);
// 				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_TESSELLATION);
// 				count++;
// 			break;
// 			case 1 << SHADER_COMPILER_SHADER_STAGE_GEOMETRY:
// 				//Geometry shader spirv
// 				spirv = buf_get_ptr_at(shader_binary, offset);
// 				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_GEOMETRY);
// 				count++;
// 			break;
// 			case 1 << SHADER_COMPILER_SHADER_STAGE_FRAGMENT:
// 				//Fragment shader spirv
// 				spirv = buf_get_ptr_at(shader_binary, offset);
// 				ref(vulkan_stage_shader_t*, shaders, count) = vulkan_stage_shader_create(renderer, spirv, length, VULKAN_SHADER_TYPE_FRAGMENT);
// 				count++;
// 			break;
// 			default:
// 				LOG_FETAL_ERR("Shader binary loading error: Invalid shader bit \"%u\" in shader_mask\n", bit);
// 			break;
// 		}
// 	}
// 	return shaders;
// }

static u32 decode_attribute_count(u64 packed_attributes)
{
	u8 bits_per_type = 5;
	// log_u64(packed_attributes);
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
