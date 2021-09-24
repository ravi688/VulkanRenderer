#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <engine/color.h>
#include <utilities.h>
#include <stdint.h>

#define Material_TYPE_ID ((uint64_t)(3))

#define MATERIAL(material) c_reinterpret_cast(material, material_t*)
#define IS_MATERIAL(material) ((MATERIAL(material)->id & Material_TYPE_ID) == Material_TYPE_ID)

typedef struct  material_t
{
	uint64_t id;
	char name[24];
	color_t color;
} material_t;

material_t* material_new(char* name);
void material_destroy(material_t* material);
void material_set_color4(material_t* material, float r, float g, float b, float a); 
void material_set_color(material_t* material, color_t* color); 


#define ensure_material_is_not_NULL(return_type)\
if(material == NULL)\
{\
	log_err("material_t is NULL");\
	return return_type;\
}
#define ensure_passed_value_is_material(return_type)\
if(!IS_MATERIAL(material))\
{\
	log_err("Passed argument is not a material_t");\
	return return_type;\
}
#endif/*__MATERIAL_H__*/