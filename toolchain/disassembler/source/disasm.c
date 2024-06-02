/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: disasm.c is a part of VulkanRenderer

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

#include <disassembler/disasm.h>
#include <common/binary_reader.h>
#include <glslcommon/glsl_types.h>
#include <common/allocation_callbacks.h>

#include <disassembler/debug.h>
#include <disassembler/assert.h>

#include <vulkan/vulkan_core.h>

#include <string.h>
#include <stdarg.h>

INLINE static u32 min(u32 v1, u32 v2)
{
	return (v1 < v2) ? v1 : v2;
}

CAN_BE_UNUSED_FUNCTION static int safe_strncmp(const char* str1, const char* const str2, u32 len)
{
	int result = strncmp(str1, str2, min(strlen(str2), len));
	return result;
}

static void _printf(BUFFER* str, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	buf_vprintf(str, NULL, format, args);
	va_end(args);
	buf_pop_pseudo(str, 1);
}

typedef struct disassembler_t
{
	binary_reader_t* reader;
	u32 sb_version;
} disassembler_t;

static void disasm_header(disassembler_t* disasm, BUFFER* str)
{
	binary_reader_t* reader = disasm->reader;
	if(strncmp(binary_reader_str(reader), "V3D Shader Binary", strlen("V3D Shader Binary")) != 0)
		DEBUG_LOG_FETAL_ERROR("Invalid file header, file is broken");

	_printf(str, "File Header: %s\n", "V3D Shader Binary");

	u32 cmd;
	u32 bversion = U32_MAX;
	u32 lversion = U32_MAX;
	u32 count = binary_reader_u32(reader);
	while(count > 0)
	{
		cmd = binary_reader_u32(reader);
		/* shader binary */
		switch(cmd)
		{
			case 1:
			{
				/* version of the sb */
				bversion = binary_reader_u32(reader);
				if((bversion != 2022) && (bversion != 2023))
					DEBUG_LOG_FETAL_ERROR("Invalid shader binary version, this disassembler only supports version 2022");
				break;
			}

			case 2:
			{
				/* skip the version of the shader language as we don't need in dissembling the file */
				lversion = binary_reader_u32(reader);
				break;
			}
			
			default:
			{
				DEBUG_LOG_FETAL_ERROR("Unable to find the shader binary version, file is broken");
				break;
			}
		}
		if((bversion != U32_MAX) && (lversion != U32_MAX))
			break;
		--count;
	}

	disasm->sb_version = bversion;

	_printf(str, "Language Version: %lu\n", lversion);
	_printf(str, "Binary Version: %lu\n", bversion);
}

INLINE static const char* glsl_type_to_string(glsl_type_t type)
{
	switch(type)
	{
		case GLSL_TYPE_UNDEFINED: return "GLSL_TYPE_UNDEFINED";
		case GLSL_TYPE_U8: return "GLSL_TYPE_U8";
		case GLSL_TYPE_U16: return "GLSL_TYPE_U16";
		case GLSL_TYPE_U32: return "GLSL_TYPE_U32";
		case GLSL_TYPE_U64: return "GLSL_TYPE_U64";
		case GLSL_TYPE_S8: return "GLSL_TYPE_S8";
		case GLSL_TYPE_S16: return "GLSL_TYPE_S16";
		case GLSL_TYPE_S32: return "GLSL_TYPE_S32";
		case GLSL_TYPE_S64: return "GLSL_TYPE_S64";
		case GLSL_TYPE_F32: return "GLSL_TYPE_F32";
		case GLSL_TYPE_F64: return "GLSL_TYPE_F64";
		case GLSL_TYPE_VEC2: return "GLSL_TYPE_VEC2";
		case GLSL_TYPE_VEC3: return "GLSL_TYPE_VEC3";
		case GLSL_TYPE_VEC4: return "GLSL_TYPE_VEC4";
		case GLSL_TYPE_MAT2: return "GLSL_TYPE_MAT2";
		case GLSL_TYPE_MAT3: return "GLSL_TYPE_MAT3";
		case GLSL_TYPE_MAT4: return "GLSL_TYPE_MAT4";

		case GLSL_TYPE_IVEC2: return "GLSL_TYPE_IVEC2";
		case GLSL_TYPE_IVEC3: return "GLSL_TYPE_IVEC3";
		case GLSL_TYPE_IVEC4: return "GLSL_TYPE_IVEC4";
		case GLSL_TYPE_UVEC2: return "GLSL_TYPE_UVEC2";
		case GLSL_TYPE_UVEC3: return "GLSL_TYPE_UVEC3";
		case GLSL_TYPE_UVEC4: return "GLSL_TYPE_UVEC4";

		case GLSL_TYPE_MAX_NON_OPAQUE: return "GLSL_TYPE_MAX_NON_OPAQUE";

		case GLSL_TYPE_BLOCK: return "GLSL_TYPE_BLOCK";
		case GLSL_TYPE_UNIFORM_BUFFER: return "GLSL_TYPE_UNIFORM_BUFFER";
		case GLSL_TYPE_STORAGE_BUFFER: return "GLSL_TYPE_STORAGE_BUFFER";
		case GLSL_TYPE_PUSH_CONSTANT: return "GLSL_TYPE_PUSH_CONSTANT";

		case GLSL_TYPE_SAMPLER_2D: return "GLSL_TYPE_SAMPLER_2D";
		case GLSL_TYPE_SAMPLER_3D: return "GLSL_TYPE_SAMPLER_3D";
		case GLSL_TYPE_SAMPLER_CUBE: return "GLSL_TYPE_SAMPLER_CUBE";
		case GLSL_TYPE_SUBPASS_INPUT: return "GLSL_TYPE_SUBPASS_INPUT";

		default: return "undefined";
	}
}

typedef enum vulkan_shader_stage_t
{
	VULKAN_SHADER_STAGE_VERTEX = 0,
	VULKAN_SHADER_STAGE_FRAGMENT = 1,
	VULKAN_SHADER_STAGE_GEOMETRY = 2,
	VULKAN_SHADER_STAGE_TESSELLATION = 3,
	VULKAN_SHADER_STAGE_MAX,

	VULKAN_SHADER_STAGE_VERTEX_BIT = BIT32(VULKAN_SHADER_STAGE_VERTEX),
	VULKAN_SHADER_STAGE_FRAGMENT_BIT = BIT32(VULKAN_SHADER_STAGE_FRAGMENT),
	VULKAN_SHADER_STAGE_GEOMETRY_BIT = BIT32(VULKAN_SHADER_STAGE_GEOMETRY),
	VULKAN_SHADER_STAGE_TESSELLATION_BIT = BIT32(VULKAN_SHADER_STAGE_TESSELLATION)
} vulkan_shader_stage_t;

#define CASE_RETURN_STR(x) case x : return #x

INLINE static const char* shader_stage_to_string(u32 stage)
{
	switch(stage)
	{
		CASE_RETURN_STR(VULKAN_SHADER_STAGE_VERTEX);
		CASE_RETURN_STR(VULKAN_SHADER_STAGE_FRAGMENT);
		CASE_RETURN_STR(VULKAN_SHADER_STAGE_GEOMETRY);
		CASE_RETURN_STR(VULKAN_SHADER_STAGE_TESSELLATION);
		default: return "undefined";
	}	
}

typedef enum vulkan_resource_descriptor_type_t
{
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_MAX,

	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB)

} vulkan_resource_descriptor_type_t;

INLINE static const char* descriptor_type_to_string(u32 stage)
{
	switch(stage)
	{
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT);
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE);
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB);
		CASE_RETURN_STR(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB);
		default: return "undefined";
	}
}

static void disasm_descriptor_info(disassembler_t* disasm, BUFFER* str)
{
	binary_reader_t* reader = disasm->reader;
	u32 info = binary_reader_u32(reader);

	u32 descriptor_type_bits = (info & 0xFF0000) >> 16;
	for(u32 i = 0; i < VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_MAX; i++)
	{
		if((descriptor_type_bits & BIT32(i)) != 0)
			_printf(str, "%s(u32: %lu) ", descriptor_type_to_string(i), i);
	}

	u32 shader_bits = (info & 0xFF00) >> 8;
	for(u32 i = 0; i < VULKAN_SHADER_STAGE_MAX; i++)
	{
		if((shader_bits & BIT32(i)) != 0)
			_printf(str, "%s(u32: %lu) ", shader_stage_to_string(i), i);
	}
	_printf(str, "%s(u32: %lu) ", glsl_type_to_string((u8)(info & 0xFF)), (u32)(info & 0xFF));

	bool is_block = (descriptor_type_bits & VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT_BIT) ||
		(descriptor_type_bits & VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER_BIT) || 
		(descriptor_type_bits & VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER_BIT);

	if(is_block)
		_printf(str, " %s, ", binary_reader_str(reader));

	_printf(str, " %s\n", binary_reader_str(reader));

	if(is_block)
	{
		u16 field_count = binary_reader_u16(reader);
		_printf(str, "field count: %u\n", field_count);
		for(u16 i = 0; i < field_count; i++)
			disasm_descriptor_info(disasm, str);
	}
}

static void disasm_descriptions(disassembler_t* disasm, BUFFER* str)
{
	binary_reader_t* reader = disasm->reader;
	/* material property descriptions */
	u32 count = binary_reader_u16(reader);
	_printf(str, "count: %lu\n", count);

	for(u32 i = 0; i < count; i++)
	{
		u32 offset = binary_reader_u32(reader);
		binary_reader_push(reader);
		binary_reader_jump(reader, offset);
		u32 set_number = binary_reader_u8(reader);
		u32 binding_number = binary_reader_u8(reader);
		_printf(str, "[%lu, %lu] ", set_number, binding_number);
		disasm_descriptor_info(disasm, str);
		binary_reader_pop(reader);
	}
}

typedef enum render_pass_type_t
{
	RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER,
	RENDER_PASS_TYPE_SWAPCHAIN_TARGET
} render_pass_type_t;

INLINE static const char* render_pass_type_to_string(render_pass_type_t type)
{
	switch(type)
	{
		CASE_RETURN_STR(RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
		CASE_RETURN_STR(RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
		default: return "undefined";
	}
}

typedef enum attachment_type_t
{
	ATTACHMENT_TYPE_COLOR = 0UL,
	ATTACHMENT_TYPE_DEPTH = 2UL,
	ATTACHMENT_TYPE_STENCIL = 4UL
} attachment_type_t;

INLINE static const char* attachment_type_to_string(attachment_type_t type)
{
	switch(type)
	{
		CASE_RETURN_STR(ATTACHMENT_TYPE_COLOR);
		CASE_RETURN_STR(ATTACHMENT_TYPE_DEPTH);
		CASE_RETURN_STR(ATTACHMENT_TYPE_STENCIL);
		default: return "undefined";
	}
}

typedef struct gfx_pipeline_t
{
	VkPipelineInputAssemblyStateCreateInfo	inputassembly;
	VkPipelineTessellationStateCreateInfo	tessellation;
	VkPipelineViewportStateCreateInfo		viewport;
	VkPipelineRasterizationStateCreateInfo	rasterization;
	VkPipelineMultisampleStateCreateInfo	multisample;
	VkPipelineDepthStencilStateCreateInfo	depthstencil;
	VkPipelineColorBlendStateCreateInfo		colorblend;
	VkPipelineDynamicStateCreateInfo		dynamic;
} gfx_pipeline_t;

static u32 gfx_pipeline_get_satelite_size(const gfx_pipeline_t* pipeline)
{
	u32 size = 0;
	size += pipeline->colorblend.attachmentCount * sizeof(VkPipelineColorBlendAttachmentState);
	size += pipeline->viewport.scissorCount * sizeof(VkRect2D);
	size += pipeline->viewport.viewportCount * sizeof(VkViewport);
	return size;
}

static void disasm_pipeline(disassembler_t* disasm, BUFFER* str)
{
	AUTO reader = disasm->reader;
	/* 	TODO: write a deserializer to deserialize aggregate data types such as
		arrays and structs.
		here we can use that deserializer to deserialize gfx_pipeline_t object and
		print the human-readable output to stdout.
	 */
	const gfx_pipeline_t* pipeline = binary_reader_read(reader, gfx_pipeline_t);	
	CAN_BE_UNUSED_VARIABLE const void* satellite_data = __binary_reader_read(reader, gfx_pipeline_get_satelite_size(pipeline));
}

static void disasm_pipeline_description(disassembler_t* disasm, BUFFER* str)
{
	AUTO reader = disasm->reader;
	u32 offset = binary_reader_u32(reader);
	binary_reader_push(reader);
	binary_reader_jump(reader, offset);
	disasm_pipeline(disasm, str);
	u8 shader_mask = binary_reader_u8(reader);
	_printf(str, "shader_mask: %u", shader_mask);
	for(u32 i = 0; i < VULKAN_SHADER_STAGE_MAX; i++)
		if(shader_mask & (1 << i))
			_printf(str, " %s", shader_stage_to_string(i));
	_printf(str, "\n");
	for(u32 i = 0; i < VULKAN_SHADER_STAGE_MAX; i++)
	{
		if(shader_mask & (1UL << i))
		{
			/*
				TODO: we have to check if the spirv is a valid spirv binary or not.
				to do that we can just check if the spirv is disassemblable or not via some tools
				like spirv-dis.
			 */
			u32 spirv_length = binary_reader_u32(reader);
			u32 spirv_offset = binary_reader_u32(reader);
			/*
				NOTE: we can't just use __binary_reader_read(reader, spirv_length) after read length and offset
				because we have jumped once and the internal read_cursor is changed.
				so we have to save the state and jump again to the read offset to correctly read the data.
			 */
			binary_reader_push(reader);
			binary_reader_jump(reader, spirv_offset);
			__binary_reader_read(reader, spirv_length);
			binary_reader_pop(reader);
		}
	}
	binary_reader_pop(reader);
}

static void disasm_subpasses(disassembler_t* disasm, BUFFER* str, u32 attachment_count, attachment_type_t* attachments)
{
	AUTO reader = disasm->reader;
	u32 count = binary_reader_u32(reader);
	_printf(str, "count: %lu\n", count);
	for(u32 i = 0; i < count; i++)
	{
		_printf(str, "-----Begin Sub Pass %lu---------\n", i);
		if(disasm->sb_version >= 2023)
			disasm_descriptions(disasm, str);
		u32 read_count = binary_reader_u32(reader);
		_printf(str, "read count: %lu", read_count);
		for(u32 j = 0; j < read_count; j++)
		{
			u32 index = binary_reader_u32(reader);
			_printf(str, " %s(id: %lu)", attachment_type_to_string(attachments[index]), index);
		}
		u32 color_count = binary_reader_u32(reader);
		_printf(str, "\ncolor count: %lu", color_count);
		for(u32 j = 0; j < color_count; j++)
		{
			u32 index = binary_reader_u32(reader);
			_printf(str, " %s(id: %lu)", attachment_type_to_string(attachments[index]), index);
		}
		u32 preserve_count = binary_reader_u32(reader);
		_printf(str, "\npreserve count: %lu", preserve_count);
		for(u32 j = 0; j < preserve_count; j++)
		{
			u32 index = binary_reader_u32(reader);
			_printf(str, " %s(id: %lu)", attachment_type_to_string(attachments[index]), index);
		}
		u32 depth_stencil = binary_reader_u32(reader);
		if(depth_stencil == U32_MAX)
			_printf(str, "\ndepth stencil: U32_MAX\n");
		else
			_printf(str, "\ndepth stencil: %s(id: %lu)\n", attachment_type_to_string(attachments[depth_stencil]), depth_stencil);
		disasm_pipeline_description(disasm, str);
		_printf(str, "sub render set bindings: ");
		disasm_descriptions(disasm, str);
		_printf(str, "-----End Sub Pass %lu-----------\n", i);
	}
}

static void disasm_renderpasses(disassembler_t* disasm, BUFFER* str)
{
	binary_reader_t* reader = disasm->reader;
	u32 count = binary_reader_u32(reader);
	_printf(str, "count: %lu\n", count);
	attachment_type_t* prev_attachments = NULL;
	CAN_BE_UNUSED_VARIABLE u32 prev_attachment_count = U32_MAX;
	for(u32 i = 0; i < count; i++)
	{
		_printf(str, "-----Begin Render Pass %lu-------\n", i);
		_printf(str, "type: %s\n", render_pass_type_to_string(binary_reader_u32(reader)));
		u32 read_count = binary_reader_u32(reader);
		_printf(str, "read count: %lu", read_count);
		for(u32 j = 0; j < read_count; j++)
			_printf(str, " %s", attachment_type_to_string(prev_attachments[binary_reader_u32(reader)]));
		u32 attachment_count = binary_reader_u32(reader);
		_printf(str, "\nattachment count: %lu", attachment_count);
		attachment_type_t* attachments = CAST_TO(attachment_type_t*, binary_reader_ptr(reader));
		prev_attachments = attachments;
		prev_attachment_count = attachment_count;
		for(u32 j = 0; j < attachment_count; j++)
		{
			attachment_type_t type = binary_reader_u32(reader);
			_printf(str, " %s(u32: %lu)", attachment_type_to_string(type), type);
		}
		u32 subpass_dependency_count = binary_reader_u32(reader);
		_printf(str, "\nsubpass dependency count: %lu\n", subpass_dependency_count);
		_printf(str, "render set bindings: ");
		disasm_descriptions(disasm, str);
		_printf(str, "subpasses: ");
		disasm_subpasses(disasm, str, attachment_count, attachments);
		_printf(str, "-----End Render Pass %lu---------\n", i);
	}
}

DISASM_API BUFFER* disassemble(const void* bytes, u32 size)
{
	BUFFER* str = BUFcreate(NULL, sizeof(char), 1024, 0);

	binary_reader_t* reader = binary_reader_create(bytes, size);

	disassembler_t* disasm = com_allocate_obj_init(NULL, disassembler_t);
	disasm->reader = reader;

	disasm_header(disasm, str);

	_printf(str, "material property descriptions: ");
	disasm_descriptions(disasm, str);
	_printf(str, "layout descriptions: ");
	disasm_descriptions(disasm, str);
	_printf(str, "render passes: ");
	disasm_renderpasses(disasm, str);
	
	binary_reader_destroy(reader);
	binary_reader_release_resources(reader);
	buf_push_null(str);

	com_deallocate(NULL, disasm);
	return str;
}
