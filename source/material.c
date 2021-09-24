#include <engine/material.h>
#include <debug.h>
#include <stdlib.h>
#include <string.h>

void material_set_color4(material_t* material, float r, float g, float b, float a)
{
	ensure_material_is_not_NULL();
	ensure_passed_value_is_material();
	color_assign(&(material->color), r, g, b, a);
}

void material_set_color(material_t* material, color_t* color)
{
	ensure_material_is_not_NULL();
	ensure_passed_value_is_material();
	color_assign(&(material->color), color->r, color->g, color->b, color->a);
} 

material_t* material_new(char* name)
{
	material_t* material = (material_t*)malloc(sizeof(material_t));
	material->id = Material_TYPE_ID;
	material->color.r = 0.0f;
	material->color.g = 0.0f; 
	material->color.b = 0.0f;
	material->color.a = 0.0f; 
	if(name != NULL)
		strcpy(material->name, name);
	else strcpy(material->name, "Untitled");
	log_msg("material_t is created");
	return material;
}

void material_destroy(material_t* material)
{
	ensure_material_is_not_NULL();
	ensure_passed_value_is_material();
	material->id = -1;
	material->color.r = 0.0f;
	material->color.g = 0.0f; 
	material->color.b = 0.0f;
	material->color.a = 0.0f;
	free(material);
	log_msg("material_t destroyed");
}