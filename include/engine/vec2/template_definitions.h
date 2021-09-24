#ifndef __HPML_VEC2_TEMPLATE_DEFINITION_H__
#define __HPML_VEC2_TEMPLATE_DEFINITION_H__

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <exception/exception.h>
#include <template_system.h>
#include <no_compile_header.h>

/*Begin: Template Definitions*/
/*template signatures*/
#define vec2_t(T) template(vec2_t, T)
#define vec2(T) template(vec2, T)
#define vec2_add(T) template(vec2_add, T)
#define vec2_sub(T) template(vec2_sub, T)
#define vec2_mul(T) template(vec2_mul, T)
#define vec2_div(T) template(vec2_div, T)
#define vec2_is_null(T) template(vec2_is_null, T)
#define vec2_is_equal(T) template(vec2_is_equal, T)
#define vec2_null(T) template(vec2_null, T)
#define vec2_zero(T) vec2_null(T)
#define vec2_print(T) template(vec2_print, T)
#define vec2_dot(T) template(vec2_dot, T)
// #define vec2_cross(T) template(vec2_cross, T)
#define vec2_magnitude(T) template(vec2_magnitude, T)
#define vec2_sqrmagnitude(T) template(vec2_sqrmagnitude, T)
#define vec2_negate(T) template(vec2_negate, T)
#define vec2_normalize(T) template(vec2_normalize, T)
#define vec2_rotate(T) template(vec2_rotate, T)
#define vec2_angle(T) template(vec2_angle, T)
#define vec2_abs(T) template(vec2_abs, T)
#define vec2_up(T) template(vec2_up, T)
#define vec2_down(T) template(vec2_down, T)
#define vec2_left(T) template(vec2_left, T)
#define vec2_right(T) template(vec2_right, T)
#define vec2_project(T) template(vec2_project, T)
#define vec2_lerp(T) template(vec2_lerp, T)
#define vec2_slerp(T) template(vec2_slerp, T) NOT_IMPLEMENTED
#define vec2_scale(T) template(vec2_scale, T)			//vec2_transform(T)(vec2_t(T) v, mat22_t(T) transform)
#define vec2_transform(T) template(vec2_transform, T) NOT_IMPLEMENTED
#define vec2_internal_division(T) template(vec2_internal_division, T)
#define vec2_external_division(T) template(vec2_external_division, T)
#define vec2_normal_anticlockwise(T) template(vec2_normal_anticlockwise, T)
#define vec2_normal_clockwise(T) template(vec2_normal_clockwise, T)
#define vec2_direction_ratios(T) template(vec2_direction_ratios, T) NOT_IMPLEMENTED


#define instantiate_vec2_struct(T) \
typedef struct vec2_t(T)\
{\
	T x;\
	T y;\
} vec2_t(T)

#define instantiate_delcaration_vec2_slerp(T) vec2_t(T) vec2_slerp(T)(vec2_t(T) v1, vec2_t(T) v2, T slerp_value)
#define instantiate_declaration_vec2_add(T) vec2_t(T) vec2_add(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_sub(T) vec2_t(T) vec2_sub(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_mul(T) vec2_t(T) vec2_mul(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_div(T) vec2_t(T) vec2_div(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_is_null(T) bool vec2_is_null(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_is_equal(T) bool vec2_is_equal(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_null(T) vec2_t(T) vec2_null(T)()
#define instantiate_declaration_vec2_zero(T) instantiate_declaration_vec2_null(T)
#define instantiate_declaration_vec2_print(T) void vec2_print(T)(vec2_t(T) v)
#define instantiate_declaration_vec2(T) vec2_t(T) vec2(T)(T x, T y)
#define instantiate_declaration_vec2_dot(T) float vec2_dot(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_magnitude(T) float vec2_magnitude(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_sqrmagnitude(T) float vec2_sqrmagnitude(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_negate(T) vec2_t(T) vec2_negate(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_normalize(T) vec2_t(T) vec2_normalize(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_rotate(T) vec2_t(T) vec2_rotate(T)(vec2_t(T) v, float angle)
#define instantiate_declaration_vec2_angle(T) float vec2_angle(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_abs(T) vec2_t(T) vec2_abs(T)(vec2_t(T) v)
#define instantiate_declaration_vec2_up(T) vec2_t(T) vec2_up(T)()
#define instantiate_declaration_vec2_down(T) vec2_t(T) vec2_down(T)()
#define instantiate_declaration_vec2_left(T) vec2_t(T) vec2_left(T)()
#define instantiate_declaration_vec2_right(T) vec2_t(T) vec2_right(T)()
#define instantiate_declaration_vec2_project(T) vec2_t(T) vec2_project(T)(vec2_t(T) v1, vec2_t(T) v2)
#define instantiate_declaration_vec2_lerp(T) vec2_t(T) vec2_lerp(vec2_t(T) v1, vec2_t(T) v2, float interpolation_value)
#define instantiate_declaration_vec2_internal_division(T) vec2_t(T) vec2_internal_division(vec2_t(T) v1, vec2_t(T) v2, float m, float n)
#define instantiate_declaration_vec2_external_division(T) vec2_t(T) vec2_external_division(vec2_t(T) v1, vec2_t(T) v2, float m, float n)
#define instantiate_declaration_vec2_normal_anticlockwise(T) vec2_t(T) vec2_normal_anticlockwise(vec2_t(T) v)
#define instantiate_declaration_vec2_normal_clockwise(T) vec2_t(T) vec2_normal_clockwise(vec2_t(T) v)
#define instantiate_declaration_vec2_scale(T) vec2_t(T) vec2_scale(vec2_t(T) v, float scalar_value)


/* vec2_slerp: Calculates the spherical interpolation value in between vector v1 and v2
 * vec2_t(T) v1: First vector [from]
 * vec2_t(T) v2: Second vector [to]
 * returns: vec2_t(T) spherically interpolated value
 */
#define instantiate_implementation_vec2_slerp(T)\
vec2_t(T) vec2_slerp(T)(vec2_t(T) v1, vec2_t(T) v2, float slerp_value)\
{\
	float angle = vec2_angle(T)(v1, v2);\
	float inverse_sin = 1 / sin(angle);\
	float temp = slerp_value * angle;\
	return vec2_scale(v1, sin(angle - temp) * inverse_sin) + vec2_scale(v2, sin(temp) * inverse_sin);\
}

/* vec2_scale: Calculates a scaled vector by amount 'scalar_value'
 * vec2_t(T) v: vector2 vector to be scaled
 * float scalar_value: scalar value to be multiplied with components of v vector
 * returns: vec2_t(T) scaled vector
 */
#define instantiate_implementation_vec2_scale(T)\
vec2_t(T) vec2_scale(T)(vec2_t(T) v, float scalar_value)\
{\
	vec2_t(T) scaled_v =  { v.x * scalar_value, v.y * scalar_value };\
	return scaled_v;\
}

/* vec2_normal_anticlockwise: Calculates a normal vector to vector v in anticlockwise sense
 * vec2_t(T) v: base vector to the normal to be calculated
 * returns: vec2_t(T) normal vector in anticlockwise sense
 */
#define instantiate_implementation_vec2_normal_anticlockwise(T)\
vec2_t(T) vec2_normal_anticlockwise(T)(vec2_t(T) v)\
{\
	vec2_t(T) normal;\
	normal.x = -v.y;\
	normal.y = v.x;\
	return normal;\
}

/* vec2_normal_clockwise: Calculates a normal vector to vector v in clockwise sense
 * vec2_t(T) v: base vector to the normal to be calculated
 * returns: vec2_t(T) normal vector in clockwise sense
 */
#define instantiate_implementation_vec2_normal_clockwise(T)\
vec2_t(T) vec2_normal_clockwise(T)(vec2_t(T) v)\
{\
	vec2_t(T) normal;\
	normal.x = v.y;\
	normal.y = -v.x;\
	return normal;\
}

/* vec2_internal_division: Calculates internal division vector
 * vec2_t(T) v1: first or left vector
 * vec2_t(T) v2: second or right vector
 * float m: left value of the ratio or numerator
 * float n: right value of the ratio or denominator
 * NOTE: if m + n = 0 then it will return first vector
 */
#define instantiate_implementation_vec2_internal_division(T)\
vec2_t(T) vec2_internal_division(T)(vec2_t(T) v1, vec2_t(T) v2, float m, float n)\
{\
	float ratio_sum = m + n;\
	if(ratio_sum == 0)\
		return v1;\
	return vec2_add(T)(vec2_scale(T)(v1, n), vec2_scale(T)(v2, m)) / ratio_sum;\
}

/* vec2_external_division: Calculates external division vector
 * vec2_t(T) v1: first or left vector
 * vec2_t(T) v2: second or right vector
 * float m: left value of the ratio or numerator
 * float n: right value of the ratio or denominator
 * NOTE: if m - n = 0 then it will return second vector
 */
#define instantiate_implementation_vec2_external_division(T)\
vec2_t(T) vec2_external_division(T)(vec2_t(T) v1, vec2_t(T) v2, float m, float n)\
{\
	float ratio_sum = m - n;\
	if(ratio_sum == 0)\
		return v2;\
	return vec2_sub(T)(vec2_scale(T)(v1, n), vec2_scale(T)(v2, m)) / (m - n);\
}

/* vec2_lerp: Calculates an interpolated vector from vector v1 to vector v2, having interpolation value 'interpolation_value'
 * vec2_t(T) v1: from vector
 * vec2_t(T) v2: to vector
 * float interpolation_value: interpolation value [Range (0, 1)], inclusive of 0 and 1
 * returns: vec2_t(T) interpolated vector
 */
#define instantiate_implementation_vec2_lerp(T)\
vec2_t(T) vec2_lerp(T)(vec2_t(T) v1, vec2_t(T) v2, float interpolation_value)\
{\
	return vec2_add(T)(vec2_scale(T)(v1, 1 - interpolation_value), vec2_scale(T)(v2, interpolation_value));\
}

/* vec2_project: Calculates a projection vector of v1 in the direction of v2 vector
 * vec2_t(T) v1: vector2 vector which is projected on v1 vector
 * vec2_t(T) v2: direction vector on which v1 vector is projected
 * returns: vec2_t(T) projected vector in the direction of vector v1
 */
#define instantiate_implementation_vec2_project(T)\
vec2_t(T) vec2_project(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	vec2_t(T) unit_dir = vec2_normalize(T)(v2);\
	float projection_value = vec2_dot(T)(v1, unit_dir);\
	return vec2_scale(T)(unit_dir, projection_value);\
}

/* vec2_right: Returns the right direction vector (vector2)
 * returns: vec2_t(T) right direction, i.e. Vector2.right [+ve x axis direction]
 */
#define instantiate_implementation_vec2_right(T)\
vec2_t(T) vec2_right(T)()\
{\
	vec2_t(T) v =  { 1, 0 };\
	return v;\
}

/* vec2_left: Returns the left direction vector (vector2)
 * returns: vec2_t(T) left direction, i.e. Vector2.left [-ve x axis direction]
 */
#define instantiate_implementation_vec2_left(T)\
vec2_t(T) vec2_left(T)()\
{\
	vec2_t(T) v =  { -1, 0 };\
	return v;\
}

/* vec2_down: Returns the down direction vector (vector2)
 * returns: vec2_t(T) down direction, i.e. Vector2.down [-ve y axis direction]
 */
#define instantiate_implementation_vec2_down(T)\
vec2_t(T) vec2_down(T)()\
{\
	vec2_t(T) v =  { 0, -1 };\
	return v;\
}

/* vec2_up: Returns the up direction vector (vector2)
 * returns: vec2_t(T) up direction, i.e. Vector2.up [+ve y axis direction]
 */
#define instantiate_implementation_vec2_up(T)\
vec2_t(T) vec2_up(T)()\
{\
	vec2_t(T) v =  { 0, 1 };\
	return v;\
}

/* vec2_abs: Calculates the absolute value of each component of the vector2 v
 * vec2_t(T) v: input vector2 vector
 * returns: vec2_t(T) vector having absolute value components
 */
#define instantiate_implementation_vec2_abs(T)\
vec2_t(T) vec2_abs(T)(vec2_t(T) v)\
{\
	vec2_t(T) v;\
	v.x = abs(v.x);\
	v.y = abs(v.y);\
	return v;\
}

/* vec2_angle: Calculates the angle between two vector2 vectors
 * vec2_t(T) v1: first vector involved in the angle calculation
 * vec2_t(T) v2: second vector involved in the angle calculation
 * returns: float +ve angle always
 */
#define instantiate_implementation_vec2_angle(T)\
float vec2_angle(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	if(!(vec2_is_null(T)(v1) && vec2_is_null(T)(v2)))\
		return 0.0f;\
	return acos(vec2_dot(T)(v1, v2) / (vec2_magnitude(T)(v1) * vec2_magnitude(T)(v2)));\
}

/* vec2_rotate: Rotates a vector2 by angle 'angle'
 * vec2_t(T) v: vector to be rotated
 * float angle: angle by which to rotate, +ve angle means anticlockwise rotation, -ve angle means clockwise rotation
 * returns: vec2_t(T) rotated vector
 */
#define instantiate_implementation_vec2_rotate(T)\
vec2_t(T) vec2_rotate(T)(vec2_t(T) v, float angle)\
{\
	vec2_t(T) rot_v;\
	float cos_angle = cos(angle);\
	float sin_angle = sin(angle);\
	rot_v.x = v.x * cos_angle - v.y * sin_angle;\
	rot_v.y = v.x * sin_angle + v.y * cos_angle;\
	return rot_v;\
}

/* vec2_normalize: Normalizes a vector2
 * vec2_t(T) v: vector to be normalized
 * returns: vec2_t(T) normalized vector
 */
#define instantiate_implementation_vec2_normalize(T)\
vec2_t(T) vec2_normalize(T)(vec2_t(T) v)\
{\
	if(vec2_is_null(T)(v))\
		return v;\
	float inv_mag = (float)1 / vec2_magnitude(T)(v);\
	vec2_t(T) unit_v;\
	unit_v.x = v.x * inv_mag;\
	unit_v.y = v.y * inv_mag;\
	return unit_v;\
}

/* vec2_negate: Negates a vector2
 * vec2_t(T) v: vector to be negated
 * returns: vec2_t(T) negated vector
 */
#define instantiate_implementation_vec2_negate(T)\
vec2_t(T) vec2_negate(T)(vec2_t(T) v)\
{\
	vec2_t(T) neg_v;\
	neg_v.x = -v.x;\
	neg_v.y = -v.y;\
	return neg_v;\
}

/* vec2_magnitude: Calculates the magnitude of vector2
 * vec2_t(T) v: vector of which magnitude to be calculated
 * returns: float magnitude of vector2 v
 */
#define instantiate_implementation_vec2_magnitude(T)\
float vec2_magnitude(T)(vec2_t(T) v)\
{\
	return sqrt(v.x * v.x + v.y * v.y);\
}

/* vec2_sqrmagnitude: Calculates the squared magnitude of vector2
 * vec2_t(T) v: vector of which squared magnitude to be calculated
 * returns: float squared magnitude of vector2 v
 */
#define instantiate_implementation_vec2_sqrmagnitude(T)\
float vec2_sqrmagnitude(T)(vec2_t(T) v)\
{\
	return v.x * v.x + v.y * v.y;\
}

/* vec_dot: Calculates the dot product of two vector2's
 * vec2_t(T) v1: first vector involved in the dot product calculation
 * vec2_t(T) v2: second vector involved in the dot product calculation
 * returns: float dot product of v1 and v2
 */
#define instantiate_implementation_vec2_dot(T)\
float vec2_dot(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	return v1.x * v2.x + v1.y * v2.y;\
}

/* vec2: Creates vector2 object in memory taking two arguments
 * x: x-component
 * y: y-component
 * returns: vec_t(T) vector having x, y
 */
#define instantiate_implementation_vec2(T)\
vec2_t(T) vec2(T)(T x, T y)\
{\
	vec2_t(T) v = { x, y };\
	return v;\
}

/* See: vec2_null
 */
#define instantiate_implementation_vec2_zero(T) instantiate_implementation_vec2_null(T)

/* vec2_null: Creates a vector2 object in memory having x = 0, and y = 0
 * returns: vec2_t(T) vector having x = 0, y = 0
 */
#define instantiate_implementation_vec2_null(T)\
vec2_t(T) vec2_null(T)()\
{\
	vec2_t(T) null_vector =  { 0, 0 };\
	return null_vector;\
}

/* vec2_is_null: Checks if passed vector2 is null or not
 * vec2_t(T) v: vector2 to be checked for
 * returns: true if vector2 is null vector, otherwise false [boolean]
 */
#define instantiate_implementation_vec2_is_null(T)\
bool vec2_is_null(T)(vec2_t(T) v)\
{\
	return (v.x == 0) && (v.y == 0);\
}

/* TODO: Make approximate calculation, not exact [with floating point numbers]
 * vec2_is_equal: Checks if passed vectors are equal or not
 * vec2_t(T) v1: first vector to be checked against
 * vec2_t(T) v2: second vector to be checked against
 * returns: true if v2 approximately equals to v1, otherwise false
 */
#define instantiate_implementation_vec2_is_equal(T)\
bool vec2_is_equal(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	return (v1.x == v2.x) && (v1.y == v2.y);\
}

/* vec2_add: Adds two vector2 vectors component wise, i.e. v1 + v2
 * vec2_t(T) v1: first vector involved in addition calculation
 * vec2_t(T) v2: second vector involved in addition calculation
 * returns: vec2_t(T) resulting vector2 vector
 */
#define instantiate_implementation_vec2_add(T)\
vec2_t(T) vec2_add(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	vec2_t(T) result;\
	result.x = v1.x + v2.x;\
	result.y = v1.y + v2.y;\
	return result;\
}

/* vec2_sub: Subtracts second vector (v2) from first vector (v1) component wise, i.e. v1 - v2
 * vec2_t(T) v1: first vector involved in subtraction calculation
 * vec2_t(T) v2: second vector involved in subtraction calculation
 * returns: vec2_t(T) resulting vector2 vector
 */
#define instantiate_implementation_vec2_sub(T)\
vec2_t(T) vec2_sub(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	vec2_t(T) result;\
	result.x = v1.x - v2.x;\
	result.y = v1.y - v2.y;\
	return result;\
}

/* vec2_mul: Multiplies two vector2 vectors v1 and v2 component wise
 * vec2_t(T) v1: first vector involved in multiplication calculation
 * vec2_t(T) v2: second vector involved in multiplication calculation
 * returns: vec2_t(T) resulting vector2 vector
 */
#define instantiate_implementation_vec2_mul(T)\
vec2_t(T) vec2_mul(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	vec2_t(T) result;\
	result.x = v1.x * v2.x;\
	result.y = v1.y * v2.y;\
	return result;\
}

/* vec2_div: Divides first vector (v1) by second vector (v2), i.e. v1 / v2
 * vec2_t(T) v1: first vector involved in division calculation
 * vec2_t(T) v2: second vector involved in division calculation
 * returns: vec2_t(T) resulting vector2 vector
 * exceptions: throws DIVIDE_BY_ZERO exception if v2 is a null vector
 */
#define instantiate_implementation_vec2_div(T)\
vec2_t(T) vec2_div(T)(vec2_t(T) v1, vec2_t(T) v2)\
{\
	EXCEPTION_BLOCK\
	(\
	if(vec2_is_null(T)(v2))\
		THROW_EXCEPTION(DIVIDE_BY_ZERO);\
	)\
	vec2_t(T) result;\
	result.x = v1.x / v2.x;\
	result.y = v1.y / v2.y;\
	return result;\
}
/*End: Template Definitions*/

#endif