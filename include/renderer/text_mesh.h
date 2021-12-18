

#include <renderer/font.h>
#include <renderer/mesh.h>

typedef struct renderer_t renderer_t;
typedef struct font_t font_t;
typedef struct mesh3d_t mesh3d_t;

typedef struct text_mesh_t
{
	mesh3d_t* handle;
	mesh_t* render_handle;
	font_t* font;
} text_mesh_t;


text_mesh_t* text_mesh_create(font_t* font);
void text_mesh_destroy(text_mesh_t* text, renderer_t* renderer);
void text_mesh_release_resources(text_mesh_t* text);

void text_mesh_draw(text_mesh_t* text, renderer_t* renderer);

void text_mesh_set_string(text_mesh_t* text, const char* string);
void text_mesh_set_size(text_mesh_t* text, u32 size);

