
#include <renderer/font.h>
#include <renderer/mesh3d.h>
#include <renderer/memory_allocator.h>
#include <disk_manager/file_reader.h>
#include <ttf2mesh.h>
#include <renderer/assert.h>


RENDERER_API font_t* font_load_and_create(const char* file_name)
{
	assert(file_name != NULL);
	BUFFER* file_data = load_binary_from_file(file_name);
	font_t* font = font_create(file_data->bytes, file_data->element_count);
	buf_free(file_data);
	return font;
}

RENDERER_API font_t* font_create(void* bytes, u64 length)
{
	assert(bytes != NULL);
	assert(length != 0);
	font_t* font = heap_new(font_t);
	memset(font, 0, sizeof(font_t));
	int result = ttf_load_from_mem(bytes, length, &font->handle, false);
	if((result != 0) || (font->handle == NULL))
	{
		LOG_FETAL_ERR("Failed to create font, bytes = %p, length = %u\n", bytes, length);
	}
	return font;
}

RENDERER_API void font_destroy(font_t* font)
{
	assert(font != NULL);
	ttf_free(get_element_ptr(font_t, font, 0)->handle);
}


RENDERER_API void font_release_resources(font_t* font)
{
	heap_free(font);
}

RENDERER_API void font_get_glyph_mesh(font_t* font, u16 wide_char, u8 mesh_quality,  mesh3d_t* out_mesh)
{
	assert(out_mesh != NULL);
	mesh3d_positions_new(out_mesh, 0);
	// mesh3d_colors_new(out_mesh, 0);
	mesh3d_triangles_new(out_mesh, 0);
	font_glyph_info_t info;
	font_get_glyph_info(font, wide_char, &info);
	ttf_mesh_t* mesh;
	u8 quality;
	switch(mesh_quality)
	{
		case FONT_GLYPH_MESH_QUALITY_LOW: quality = TTF_QUALITY_LOW; break;
		case FONT_GLYPH_MESH_QUALITY_NORMAL: quality = TTF_QUALITY_NORMAL; break;
		case FONT_GLYPH_MESH_QUALITY_HIGH: quality = TTF_QUALITY_HIGH; break;
		default: LOG_FETAL_ERR("Invalid font mesh quality: %u\n", mesh_quality);
	}
	int result = ttf_glyph2mesh(&font->handle->glyphs[info.index], &mesh, quality, TTF_FEATURES_DFLT);
	assert(result == TTF_DONE);

	float max_y = 0, min_y = 0, max_x = 0, min_x = 0;
	for(int i = 0; i < mesh->nvert; i++)
	{
		if(mesh->vert[i].y > max_y)
			max_y = mesh->vert[i].y;
		if(mesh->vert[i].y < min_y)
			min_y = mesh->vert[i].y;

		if(mesh->vert[i].x > max_x)
			max_x = mesh->vert[i].x;
		if(mesh->vert[i].x < min_x)
			min_x = mesh->vert[i].x;

		mesh3d_position_add(out_mesh, 0, mesh->vert[i].y, mesh->vert[i].x);
		// mesh3d_color_add(out_mesh, 1, 1, 1);
	}
	for(int i = 0; i < mesh->nfaces; i++)
		mesh3d_triangle_add(out_mesh, mesh->faces[i].v3, mesh->faces[i].v2, mesh->faces[i].v1);
	mesh3d_optimize_buffer(out_mesh);
	ttf_free_mesh(mesh);
}


RENDERER_API void font_get_glyph_info(font_t* font, u16 wide_char, font_glyph_info_t* out_info)
{
	int index = ttf_find_glyph(font->handle, wide_char);
	if(index < 0)
	{
		LOG_FETAL_ERR("Font error: couldn't find glyph \"%c\"\n", wide_char);
	}
	ttf_glyph_t info = font->handle->glyphs[index];
	out_info->left_side_bearing =  info.lbearing;
	out_info->right_side_bearing = info.rbearing;
	out_info->advance_width = info.advance;
	out_info->min_x = info.xbounds[0];
	out_info->max_x = info.xbounds[1];
	out_info->min_y = info.ybounds[0];
	out_info->max_y = info.ybounds[1];
	out_info->index = index;
}
