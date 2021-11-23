#pragma once

#include <renderer/renderer.h>

typedef struct material_t material_t;

material_t* material_new(renderer_t* renderer);
void material_destroy(renderer_t* renderer, material_t* material);

