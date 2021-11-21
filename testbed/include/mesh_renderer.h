// #ifndef __RENDERER_H__
// #define __RENDERER_H__

// #include <ecs/object.h>
// #include <ecs/component.h>
// #include <ecs/mesh_filter.h>
// #include <debug.h>
// #include <ecs/material.h>
// #include <buffer.h>

// #define mesh_renderer_t_TYPE_ID ((uint64_t)(Component_TYPE_ID | (((uint64_t)5) << 32)))

// typedef struct mesh_renderer_t
// {
// 	component_t component;
// 	LIST(material_t*) materials;
// 	mesh_filter_t* mesh_filter;
// 	material_t* default_material;
// } mesh_renderer_t;

// void mesh_renderer_materials_new(mesh_renderer_t* mesh_renderer, uint32_t mesh_count);
// LIST(material_t*) mesh_renderer_get_material_list(mesh_renderer_t* mesh_renderer);
// void mesh_renderer_set_material(mesh_renderer_t* mesh_renderer, material_t* material, uint32_t index);

// void mesh_renderer_destroy(mesh_renderer_t* mesh_renderer);

// component_declare_callbacks(mesh_renderer_t);

// #endif/*__RENDERER_H__*/