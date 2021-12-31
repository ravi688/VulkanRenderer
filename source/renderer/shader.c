
#include <renderer/shader.h>
#include <renderer/defines.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <memory_allocator/memory_allocator.h>
#include <shader_compiler/compiler.h>
#include <disk_manager/file_reader.h>
#include <string.h>

shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary)
{
	const char* header = "SHADER BINARY"; u8 header_len = strlen(header);
	char buffer[header_len];
	buf_ucount_t cursor = 0;
	for(; cursor < header_len; cursor++) buf_get_at(shader_binary, cursor, &buffer[cursor]);
	if(strncmp(buffer, header, header_len) != 0)
		LOG_FETAL_ERR("Shader binary loading error: invalid header \"%.*s\"\n", header_len, buffer);

	u32 section_mask = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
	u32 shader_offset;
	bool shader_found = false;
	const u32 MASK = 0x00000007UL;
	for(u8 i = 0; i < 3; i++)
	{
		if((!shader_found) && (section_mask & ((1UL << 2) << (i * 3))))
		{
			shader_offset = *(u32*)buf_get_ptr_at(shader_binary, cursor);
			shader_found = true;
		}
		if(section_mask & (MASK << (i * 3)))
			cursor += 4;
	}
	if(!shader_found)
		LOG_FETAL_ERR("Shader binary loading error: SHADER section not found!\n");

	cursor = shader_offset;
	u8 shader_mask = *(u8*)buf_get_ptr_at(shader_binary, cursor); cursor++;
	u8 shader_count = 0; for(u8 i = 0; i < 4; i++) { if(shader_mask & (1 << i)) shader_count++; }

	stage_shader_t** shaders = stack_newv(stage_shader_t*, shader_count);
	for(u8 i = 0, count = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
	{
		u8 bit = shader_mask & (1 << i);
		if(bit == 0) continue;
		u32 offset = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
		u32 length = *(u32*)buf_get_ptr_at(shader_binary, cursor); cursor += 4;
		switch(bit)
		{
			case 1 << 0:
				//Vertex shader spirv
				u32* spirv = buf_get_ptr_at(shader_binary, offset);
				ref(stage_shader_t*, shaders, count) = stage_shader_create(renderer, spirv, length, SHADER_STAGE_VERTEX);
				count++;
				break;
			case 1 << 1:
				//Tessellation shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(stage_shader_t*, shaders, count) = stage_shader_create(renderer, spirv, length, SHADER_STAGE_TESSELLATION);
				count++;
			break;
			case 1 << 2:
				//Geometry shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(stage_shader_t*, shaders, count) = stage_shader_create(renderer, spirv, length, SHADER_STAGE_GEOMETRY);
				count++;
			break;
			case 1 << 3:
				//Fragment shader spirv
				spirv = buf_get_ptr_at(shader_binary, offset);
				ref(stage_shader_t*, shaders, count) = stage_shader_create(renderer, spirv, length, SHADER_STAGE_FRAGMENT);
				count++;
			break;
			default:
				LOG_FETAL_ERR("Shader binary loading error: Invalid shader bit \"%u\" in shader_mask\n", bit);
			break;
		}
	}
	shader_t* shader = heap_new(shader_t);
	memset(shader, 0, sizeof(shader_t));
	shader->stage_shaders = heap_newv(stage_shader_t*, shader_count);
	memcpy(shader->stage_shaders, refp(stage_shader_t*, shaders, 0), sizeof(stage_shader_t*) * shader_count);
	shader->stage_count = shader_count;
	stack_free(shaders);
	return shader;
}

shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path)
{
	BUFFER* shader_binary = load_binary_from_file(file_path);
	shader_t* shader = shader_create(renderer, shader_binary);
	buf_free(shader_binary);
	return shader;
}

void shader_destroy(shader_t* shader, renderer_t* renderer)
{
	for(u8 i = 0; i < shader->stage_count; i++)
		stage_shader_destroy(ref(stage_shader_t*, shader->stage_shaders, i), renderer);
}

void shader_release_resources(shader_t* shader)
{
	heap_free(shader->stage_shaders);
	heap_free(shader);
}

