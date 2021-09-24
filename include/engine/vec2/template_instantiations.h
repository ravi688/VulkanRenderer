
#ifndef __HPML_VEC2_TEMPLATE_INSTANTIATIONS_H__
#define __HPML_VEC2_TEMPLATE_INSTANTIATIONS_H__

#include <vec2/template_definitions.h>

/*Begin: Template Instantiations*/
instantiate_vec2_struct(int);
instantiate_declaration_vec2_up(int);
instantiate_declaration_vec2_down(int); 
instantiate_declaration_vec2_left(int); 
instantiate_declaration_vec2_right(int);
instantiate_declaration_vec2_add(int);
instantiate_declaration_vec2_sub(int);
instantiate_declaration_vec2_mul(int);
instantiate_declaration_vec2_div(int);
instantiate_declaration_vec2_is_null(int);
instantiate_declaration_vec2_is_equal(int);
instantiate_declaration_vec2_null(int);
instantiate_declaration_vec2_print(int);
instantiate_declaration_vec2(int);		//constructor
instantiate_declaration_vec2_dot(int);
instantiate_declaration_vec2_magnitude(int);
instantiate_declaration_vec2_sqrmagnitude(int);
instantiate_declaration_vec2_normalize(int);
instantiate_declaration_vec2_rotate(int);
instantiate_declaration_vec2_negate(int);


instantiate_vec2_struct(float);
instantiate_declaration_vec2_up(float);
instantiate_declaration_vec2_down(float); 
instantiate_declaration_vec2_left(float); 
instantiate_declaration_vec2_right(float);
instantiate_declaration_vec2_add(float);
instantiate_declaration_vec2_sub(float);
instantiate_declaration_vec2_mul(float);
instantiate_declaration_vec2_div(float);
instantiate_declaration_vec2_is_null(float);
instantiate_declaration_vec2_is_equal(float);
instantiate_declaration_vec2_null(float);
instantiate_declaration_vec2_print(float);
instantiate_declaration_vec2(float);	//constructor
instantiate_declaration_vec2_dot(float);
instantiate_declaration_vec2_magnitude(float);
instantiate_declaration_vec2_sqrmagnitude(float);
instantiate_declaration_vec2_normalize(float);
instantiate_declaration_vec2_rotate(float);
instantiate_declaration_vec2_negate(float);

instantiate_declaration_vec2_angle(float);


#if defined(IMPLEMENTATION)
instantiate_implementation_vec2_up(int);
instantiate_implementation_vec2_down(int);
instantiate_implementation_vec2_left(int);
instantiate_implementation_vec2_right(int);
instantiate_implementation_vec2_add(int); 
instantiate_implementation_vec2_sub(int);
instantiate_implementation_vec2_mul(int);
instantiate_implementation_vec2_div(int);
instantiate_implementation_vec2_null(int);
instantiate_implementation_vec2_is_null(int);
instantiate_implementation_vec2_is_equal(int);
instantiate_implementation_vec2(int);
instantiate_implementation_vec2_dot(int);
instantiate_implementation_vec2_magnitude(int);
instantiate_implementation_vec2_sqrmagnitude(int);
instantiate_implementation_vec2_normalize(int);
instantiate_implementation_vec2_rotate(int);
instantiate_implementation_vec2_negate(int);
void vec2_print(int)(vec2_t(int) v)
{
	printf("vec2_t(int): { %d, %d }\n", v.x, v.y);
}

instantiate_implementation_vec2_up(float);
instantiate_implementation_vec2_down(float);
instantiate_implementation_vec2_left(float);
instantiate_implementation_vec2_right(float);
instantiate_implementation_vec2_add(float); 
instantiate_implementation_vec2_sub(float);
instantiate_implementation_vec2_mul(float);
instantiate_implementation_vec2_div(float);
instantiate_implementation_vec2_null(float);
instantiate_implementation_vec2_is_null(float);
instantiate_implementation_vec2_is_equal(float);
instantiate_implementation_vec2(float);
instantiate_implementation_vec2_dot(float);
instantiate_implementation_vec2_magnitude(float);
instantiate_implementation_vec2_sqrmagnitude(float);
instantiate_implementation_vec2_normalize(float);
instantiate_implementation_vec2_rotate(float);
instantiate_implementation_vec2_negate(float);
instantiate_implementation_vec2_angle(float);
void vec2_print(float)(vec2_t(float) v)
{
	printf("vec2_t(float):  { %f, %f }\n", v.x, v.y);
}
#endif
/*End: Template Instantiations*/

#endif