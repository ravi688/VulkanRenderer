

#include <renderer/material.h>
#include <renderer/shader.h>

#include <memory_allocator/memory_allocator.h>

typedef struct material_t
{
	shader_t* shader;
} material_t;

material_t* material_new(renderer_t* renderer)
{
	material_t* material = heap_new(material_t);
	return material;
}

void material_destroy(renderer_t* renderer, material_t* material)
{
	heap_free(material);
}