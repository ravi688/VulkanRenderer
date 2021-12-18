

#include <renderer/text_mesh.h>
#include <renderer/assert.h>
#include <renderer/mesh.h>
#include <renderer/mesh3d.h>
#include <memory_allocator/memory_allocator.h>


text_mesh_t* text_mesh_create(font_t* font)
{
	assert(font != NULL);
	text_mesh_t* text_mesh = heap_new(text_mesh_t);
	memset(text_mesh, 0, sizeof(text_mesh_t));
	text_mesh->handle = mesh3d_new();
	text_mesh->font = font;
	return text_mesh;
}

void text_mesh_destroy(text_mesh_t* text, renderer_t* renderer)
{
	assert(text != NULL);
	mesh_destroy(text->render_handle, renderer);
	mesh3d_destroy(text->handle);
}

void text_mesh_release_resources(text_mesh_t* text)
{
	assert(text != NULL);
	mesh_release_resources(text->render_handle);
	heap_free(text);
}


void text_mesh_draw(text_mesh_t* text, renderer_t* renderer)
{
	assert(text != NULL);
	mesh_draw_indexed(text->render_handle, renderer);
}


void text_mesh_set_string(text_mesh_t* text, const char* string)
{
	assert(text != NULL);

}

void text_mesh_set_size(text_mesh_t* text, const u32 size)
{
	assert(text != NULL);

}
