

#include <renderer/text_mesh.h>
#include <renderer/mesh.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/mesh3d.h>
#include <renderer/font.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

static void build_meshes(BUFFER* meshes, font_t* font);

text_mesh_t* text_mesh_create(font_t* font)
{
	assert(font != NULL);
	text_mesh_t* text = heap_new(text_mesh_t);
	memset(text, 0, sizeof(text_mesh_t));
	text->meshes = buf_create(sizeof(mesh3d_t*), 0, 0);
	text->render_meshes = buf_create(sizeof(mesh_t*), 0, 0);
	text->instance_counts = buf_create(sizeof(u32), 0, 0);
	text->font = font;
	build_meshes(&text->meshes, font);
	return text;
}

void text_mesh_destroy(text_mesh_t* text, renderer_t* renderer)
{
	assert(text != NULL);
	for(u32 i = 0; i < text->meshes.element_count; i++)
		mesh3d_destroy(*(mesh3d_t**)buf_get_ptr_at(&text->meshes, i));
	for(u32 i = 0; i < text->render_meshes.element_count; i++)
		mesh_destroy(*(mesh_t**)buf_get_ptr_at(&text->render_meshes, i), renderer);
}

void text_mesh_release_resources(text_mesh_t* text)
{
	assert(text != NULL);
	for(u32 i = 0; i < text->render_meshes.element_count; i++)
		mesh_release_resources(*(mesh_t**)buf_get_ptr_at(&text->render_meshes, i));
	buf_free(&text->render_meshes);
	buf_free(&text->meshes);
	heap_free(text);
}


void text_mesh_draw(text_mesh_t* text, renderer_t* renderer)
{
	assert(text != NULL);
	for(u32 i = 0; i < text->render_meshes.element_count; i++)
		mesh_draw_indexed_instanced(*(mesh_t**)buf_get_ptr_at(&text->render_meshes, i), renderer, *(u32*)buf_get_ptr_at(&text->instance_counts, i));
}


void text_mesh_set_string(text_mesh_t* text, renderer_t* renderer, const char* string)
{
	assert(text != NULL);

	//maximum number of characters in the character set
	u8 unique_char_count = 126 - 33 + 1;

	//create a count_mask,
	u32 count_mask[unique_char_count];
	memset(count_mask, 0UL, sizeof(u32) * unique_char_count);

	//create instance buffers for each possible character in the character set
	BUFFER instance_buffers[unique_char_count];
	for(u8 i = 0; i < unique_char_count; i++)
		instance_buffers[i] = buf_create(sizeof(vec3_t(float)), 0, 0);

	float cursor = 0;
	while(*string != 0)
	{
		char ch = *string;
		assert(ch > 32);
		count_mask[ch - 33]++;
		font_glyph_info_t info;
		font_get_glyph_info(text->font, ch, &info);

		BUFFER* buffer = &instance_buffers[ch - 33];
		vec3_t(float) v = { 0, 0, cursor };
		cursor += info.advance_width;
		buf_push(buffer, &v);
		string++;
	}

	for(u8 i = 0; i < unique_char_count; i++)
	{
		if(count_mask[i] == 0UL) continue;
		mesh_t* mesh = mesh_create(renderer, *(mesh3d_t**)buf_get_ptr_at(&text->meshes, i));
		vulkan_vertex_buffer_create_info_t create_info =
		{
			.data = instance_buffers[i].bytes,
			.stride = sizeof(vec3_t(float)),
			.count = instance_buffers[i].element_count
		};
		vulkan_mesh_create_and_add_vertex_buffer(mesh, renderer, &create_info);
		buf_free(&instance_buffers[i]);
		buf_push(&text->render_meshes, &mesh);
	}

	for(u8 i = 0; i < unique_char_count; i++)
		if(count_mask[i] != 0UL)
			buf_push(&text->instance_counts, &count_mask[i]);
}

void text_mesh_set_size(text_mesh_t* text, const u32 size)
{
	assert(text != NULL);

}


static void build_meshes(BUFFER* meshes, font_t* font)
{
	/*NOTE: 32 is space, not printable character*/
	u16 A = 33;
	buf_clear(meshes, NULL);
	while(A <= 126)
	{
		font_glyph_info_t info;
		font_get_glyph_info(font, A, &info);
		mesh3d_t* mesh = mesh3d_new();
		font_get_glyph_mesh(font, A, FONT_GLYPH_MESH_QUALITY_LOW, mesh);
		buf_push(meshes, &mesh);
		A++;
	}
}

