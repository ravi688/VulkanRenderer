
#ifndef __HPML_VEC3_TEMPLATE_INSTANTIATIONS_H__
#define __HPML_VEC3_TEMPLATE_INSTANTIATIONS_H__

#include <vec3/template_definitions.h>

/*Begin: Template Instantiations*/
instantiate_vec3_struct(int);
instantiate_declaration_vec3_add(int);
instantiate_declaration_vec3_sub(int);
instantiate_declaration_vec3_mul(int);
instantiate_declaration_vec3_div(int);
instantiate_declaration_vec3_is_null(int);
instantiate_declaration_vec3_null(int);
instantiate_declaration_vec3_print(int);
instantiate_declaration_vec3(int);		//constructor
instantiate_declaration_vec3_dot(int);
instantiate_declaration_vec3_magnitude(int);
instantiate_declaration_vec3_normalize(int);
instantiate_declaration_vec3_rotate(int);
instantiate_declaration_vec3_scale(int);
instantiate_declaration_vec3_cross(int);
instantiate_declaration_vec3_up(int);
instantiate_declaration_vec3_down(int);
instantiate_declaration_vec3_left(int);
instantiate_declaration_vec3_right(int);
instantiate_declaration_vec3_forward(int);
instantiate_declaration_vec3_backward(int);
instantiate_declaration_vec3_box_product(int);

instantiate_vec3_struct(float);
instantiate_declaration_vec3_add(float);
instantiate_declaration_vec3_sub(float);
instantiate_declaration_vec3_mul(float);
instantiate_declaration_vec3_div(float);
instantiate_declaration_vec3_is_null(float);
instantiate_declaration_vec3_null(float);
instantiate_declaration_vec3_print(float);
instantiate_declaration_vec3(float);	//constructor
instantiate_declaration_vec3_dot(float);
instantiate_declaration_vec3_magnitude(float);
instantiate_declaration_vec3_normalize(float);
instantiate_declaration_vec3_rotate(float);
instantiate_declaration_vec3_scale(float);
instantiate_declaration_vec3_cross(float);
instantiate_declaration_vec3_up(float);
instantiate_declaration_vec3_down(float);
instantiate_declaration_vec3_left(float);
instantiate_declaration_vec3_right(float);
instantiate_declaration_vec3_forward(float);
instantiate_declaration_vec3_backward(float);
instantiate_declaration_vec3_box_product(float);

#if defined(IMPLEMENTATION)
instantiate_implementation_vec3_add(int); 
instantiate_implementation_vec3_sub(int);
instantiate_implementation_vec3_mul(int);
instantiate_implementation_vec3_div(int);
instantiate_implementation_vec3_null(int);
instantiate_implementation_vec3_is_null(int);
instantiate_implementation_vec3(int);
instantiate_implementation_vec3_dot(int);
instantiate_implementation_vec3_magnitude(int);
instantiate_implementation_vec3_normalize(int);
instantiate_implementation_vec3_rotate(int);
instantiate_implementation_vec3_scale(int);
instantiate_implementation_vec3_cross(int);
instantiate_implementation_vec3_up(int);
instantiate_implementation_vec3_down(int);
instantiate_implementation_vec3_left(int);
instantiate_implementation_vec3_right(int);
instantiate_implementation_vec3_forward(int);
instantiate_implementation_vec3_backward(int);
instantiate_implementation_vec3_box_product(int);
void vec3_print(int)(vec3_t(int) v)
{
	printf("vec3_t(int): { %d, %d }\n", v.x, v.y);
}

instantiate_implementation_vec3_add(float); 
instantiate_implementation_vec3_sub(float);
instantiate_implementation_vec3_mul(float);
instantiate_implementation_vec3_div(float);
instantiate_implementation_vec3_null(float);
instantiate_implementation_vec3_is_null(float);
instantiate_implementation_vec3(float);
instantiate_implementation_vec3_dot(float);
instantiate_implementation_vec3_magnitude(float);
instantiate_implementation_vec3_normalize(float);
instantiate_implementation_vec3_rotate(float);
instantiate_implementation_vec3_scale(float);
instantiate_implementation_vec3_cross(float);
instantiate_implementation_vec3_up(float);
instantiate_implementation_vec3_down(float);
instantiate_implementation_vec3_left(float);
instantiate_implementation_vec3_right(float);
instantiate_implementation_vec3_forward(float);
instantiate_implementation_vec3_backward(float);
instantiate_implementation_vec3_box_product(float);
void vec3_print(float)(vec3_t(float) v)
{
	printf("vec3_t(float):  { %f, %f }\n", v.x, v.y);
}
#endif
/*End: Template Instantiations*/

#endif