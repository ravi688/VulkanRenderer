

#include <renderer/font.h>


typedef struct text_mesh_t
{

} text_mesh_t;


text_mesh_t* text_mesh_create(renderer_t* renderer, const char* string, font_t* font);
void text_mesh_destroy(text_mesh_t* text_mesh, renderer_t* renderer);
void text_mesh_release_resources(text_mesh_t* text_mesh);
