#ifndef __HPML_VEC3_TEMPLATE_DEFINITION_H__
#define __HPML_VEC3_TEMPLATE_DEFINITION_H__

#include <exception/exception.h>
#include <template_system.h>
#include <no_compile_header.h>

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

/*Begin: Template Definitions*/
/*template signatures*/
#define vec3_t(T) template(vec3_t, T)
#define vec3(T) template(vec3, T)
#define vec3_add(T) template(vec3_add, T)
#define vec3_sub(T) template(vec3_sub, T)
#define vec3_mul(T) template(vec3_mul, T)
#define vec3_div(T) template(vec3_div, T)
#define vec3_is_null(T) template(vec3_is_null, T)
#define vec3_is_equal(T) template(vec3_is_equal, T)
#define vec3_null(T) template(vec3_null, T)
#define vec3_zero(T) vec3_null(T)
#define vec3_print(T) template(vec3_print, T)
#define vec3_dot(T) template(vec3_dot, T)
#define vec3_cross(T) template(vec3_cross, T)
#define vec3_magnitude(T) template(vec3_magnitude, T)
#define vec3_negate(T) template(vec3_negate, T)
#define vec3_normalize(T) template(vec3_normalize, T)
#define vec3_rotate(T) template(vec3_rotate, T) NOT_IMPLEMENTED
#define vec3_angle(T) template(vec3_angle, T)
#define vec3_abs(T) template(vec3_abs, T)
#define vec3_up(T) template(vec3_up, T)
#define vec3_down(T) template(vec3_down, T)
#define vec3_left(T) template(vec3_left, T)
#define vec3_right(T) template(vec3_right, T)
#define vec3_forward(T) template(vec3_forward, T)
#define vec3_backward(T) template(vec3_backward, T)
#define vec3_project(T) template(vec3_project, T)
#define vec3_lerp(T) template(vec3_lerp, T)
#define vec3_slerp(T) template(vec3_slerp, T)
#define vec3_scale(T) template(vec3_scale, T)			//vec3_transform(T)(vec3_t(T) v, mat33_t(T) transform)
#define vec3_transform(T) template(vec3_transform, T) NOT_IMPLEMENTED
#define vec3_internal_division(T) template(vec3_internal_division, T)
#define vec3_external_division(T) template(vec3_external_division, T)
#define vec3_direction_ratios(T) template(vec3_direction_ratios, T) NOT_IMPLEMENTED
#define vec3_box_product(T) template(vec3_box_product, T) NOT_IMPLEMENTED

#define instantiate_vec3_struct(T) \
typedef struct vec3_t(T)\
{\
	T x;\
	T y;\
	T z;\
} vec3_t(T)

#define instantiate_declaration_vec3_is_equal(T) bool vec3_is_equal(T)(vec3_t(T) v1, vec3_t(T v2) v2)
#define instantiate_declaration_vec3_slerp(T) vec3_t(T) vec3_slerp(T)(vec3_t(T) v1, vec3_t(T) v2, T slerp_value)
#define instantiate_declaration_vec3_add(T) vec3_t(T) vec3_add(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_sub(T) vec3_t(T) vec3_sub(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_mul(T) vec3_t(T) vec3_mul(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_div(T) vec3_t(T) vec3_div(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_is_null(T) bool vec3_is_null(T)(vec3_t(T) v)
#define instantiate_declaration_vec3_null(T) vec3_t(T) vec3_null(T)()
#define instantiate_declaration_vec3_zero(T) instantiate_declaration_vec3_null(T)
#define instantiate_declaration_vec3_print(T) void vec3_print(T)(vec3_t(T) v)
#define instantiate_declaration_vec3(T) vec3_t(T) vec3(T)(T x, T y, T z)
#define instantiate_declaration_vec3_dot(T) float vec3_dot(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_magnitude(T) float vec3_magnitude(T)(vec3_t(T) v)
#define instantiate_declaration_vec3_negate(T) vec3_t(T) vec3_negate(T)(vec3_t(T) v)
#define instantiate_declaration_vec3_normalize(T) vec3_t(T) vec3_normalize(T)(vec3_t(T) v)
#define instantiate_declaration_vec3_rotate(T) vec3_t(T) vec3_rotate(T)(vec3_t(T) v, vec3_t(T) axis, float angle)
#define instantiate_declaration_vec3_angle(T) float vec3_angle(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_abs(T) vec3_t(T) vec3_abs(T)(vec3_t(T) v)
#define instantiate_declaration_vec3_up(T) vec3_t(T) vec3_up(T)()
#define instantiate_declaration_vec3_down(T) vec3_t(T) vec3_down(T)()
#define instantiate_declaration_vec3_left(T) vec3_t(T) vec3_left(T)()
#define instantiate_declaration_vec3_right(T) vec3_t(T) vec3_right(T)()
#define instantiate_declaration_vec3_forward(T) vec3_t(T) vec3_forward(T)()
#define instantiate_declaration_vec3_backward(T) vec3_t(T) vec3_backward(T)()
#define instantiate_declaration_vec3_project(T) vec3_t(T) vec3_project(T)(T)(vec3_t(T) v1, vec3_t(T) v2)
#define instantiate_declaration_vec3_lerp(T) vec3_t(T) vec3_lerp(T)(vec3_t(T) v1, vec3_t(T) v2, float interpolation_value)
#define instantiate_declaration_vec3_internal_division(T) vec3_t(T) vec3_internal_division(T)(vec3_t(T) v1, vec3_t(T) v2, float m, float n)
#define instantiate_declaration_vec3_external_division(T) vec3_t(T) vec3_external_division(T)(vec3_t(T) v1, vec3_t(T) v2, float m, float n)
#define instantiate_declaration_vec3_scale(T) vec3_t(T) vec3_scale(T)(vec3_t(T) v, float scalar_value)
#define instantiate_declaration_vec3_cross(T) vec3_t(T) vec3_cross(T)(vec3_t(T) from, vec3_t(T) to)
#define instantiate_declaration_vec3_box_product(T) float vec3_box_product(T)(vec3_t(T) from, vec3_t(T) to, vec3_t(T) height)

/* vec3_is_equal(T): Checks whether the two vectors are equal or not
 * vec3_t(T) v1: First vector to be checked against
 * vec3_t(T) v2: Second vector to be checked against
 * returns: bool true if both vectors are approximetly equal, otherwise false
 */
#define instantiate_implementation_vec3_is_equal(T)\
bool vec3_is_equal(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);\
}

/* vec3_slerp(T): Calculates spherical interpolation value in-between the vector v1 and v2
 * vec3_t(T) v1: First vector [from]
 * vec3_t(T) v2: Second vector [to]
 * T slerp_value: interpolation parameter
 * returns: vec3_t(T) spherically interpolated value
 */
#define instantiate_implementation_vec3_slerp(T)\
vec3_t(T) vec3_slerp(T)(vec3_t(T) v1, vec3_t(T) v2, T slerp_value)\
{\
	float angle = vec3_angle(T)(v1, v2);\
	EXCEPTION_BLOCK(\
		if((angle == 0) || (angle == 3.145926f) || (angle == 2 * 3.145926f))\
			THROW_EXCEPTION(DIVIDE_BY_ZERO);\
	)\
	float inv_sin_angle = 1 / sin(angle);\
	float temp = slerp_value * angle;\
	return vec3_add(vec3_scale(T)(v1, sin(angle - temp) * inv_sin_angle), vec3_scale(v2, sin(temp)));\
}

/* vec3_box_product: Calculates box product [Value of parallelpiped]
 * vec3_t(T) from: from vector(
 * vec3_t(T) to: to vector
 * returns: float box product of from, to, and height vectors
 */
#define instantiate_implementation_vec3_box_product(T)\
float vec3_box_product(T)(vec3_t(T) from, vec3_t(T) to, vec3_t(T) height)\
{\
	return vec3_dot(T)(height, vec3_cross(T)(from, to));\
}

/* vec3_cross: Calculates cross product of 'from' (vector3) to 'to' (vector3)
 * vec3_t(T) from: from vector
 * vec3_t(T) to: to vector
 * returns: vec3_t(T) cross product, pseudo vector
 */
#define instantiate_implementation_vec3_cross(T)\
vec3_t(T) vec3_cross(T)(vec3_t(T) from, vec3_t(T) to)\
{\
	vec3_t(T) v;\
	v.x = from.y * to.z - from.z * to.y;\
	v.y = from.x * to.z - from.z * to.x;\
	v.z = from.x * to.y - from.y * to.x;\
	return v;\
}

/* vec3_scale: Calculates a scaled vector by amount 'scalar_value'
 * vec3_t(T) v: vector3 vector to be scaled
 * float scalar_value: scalar value to be multiplied with components of v vector
 * returns: vec3_t(T) scaled vector
 */
#define instantiate_implementation_vec3_scale(T)\
vec3_t(T) vec3_scale(T)(vec3_t(T) v, float scalar_value)\
{\
	vec3_t(T) scaled_v =  { v.x * scalar_value, v.y * scalar_value, v.z * scalar_value };\
	return scaled_v;\
}

/* vec3_internal_division: Calculates internal division vector
 * vec3_t(T) v1: first or left vector
 * vec3_t(T) v2: second or right vector
 * float m: left value of the ratio or numerator
 * float n: right value of the ratio or denominator
 * NOTE: if m + n = 0 then it will return first vector
 */
#define instantiate_implementation_vec3_internal_division(T)\
vec3_t(T) vec3_internal_division(T)(vec3_t(T) v1, vec3_t(T) v2, float m, float n)\
{\
	float ratio_sum = m + n;\
	if(ratio_sum == 0)\
		return v1;\
	return vec3_add(T)(vec3_scale(T)(v1, n), vec3_scale(T)(v2, m)) / ratio_sum;\
}

/* vec3_external_division: Calculates external division vector
 * vec3_t(T) v1: first or left vector
 * vec3_t(T) v2: second or right vector
 * float m: left value of the ratio or numerator
 * float n: right value of the ratio or denominator
 * NOTE: if m - n = 0 then it will return second vector
 */
#define instantiate_implementation_vec3_external_division(T)\
vec3_t(T) vec3_external_division(T)(vec3_t(T) v1, vec3_t(T) v2, float m, float n)\
{\
	float ratio_sum = m - n;\
	if(ratio_sum == 0)\
		return v2;\
	return vec3_sub(T)(vec3_scale(T)(v1, n), vec3_scale(T)(v2, m)) / (m - n);\
}

/* vec3_lerp: Calculates an interpolated vector from vector v1 to vector v2, having interpolation value 'interpolation_value'
 * vec3_t(T) v1: from vector
 * vec3_t(T) v2: to vector
 * float interpolation_value: interpolation value [Range (0, 1)], inclusive of 0 and 1
 * returns: vec3_t(T) interpolated vector
 */
#define instantiate_implementation_vec3_lerp(T)\
vec3_t(T) vec3_lerp(T)(vec3_t(T) v1, vec3_t(T) v2, float interpolation_value)\
{\
	return vec3_add(T)(vec3_scale(T)(v1, 1 - interpolation_value), vec3_scale(T)(v2, interpolation_value));\
}

/* vec3_project: Calculates a projection vector of v1 in the direction of v2 vector
 * vec3_t(T) v1: vector3 vector which is projected on v1 vector
 * vec3_t(T) v2: direction vector on which v1 vector is projected
 * returns: vec3_t(T) projected vector in the direction of vector v1
 */
#define instantiate_implementation_vec3_project(T)\
vec3_t(T) vec3_project(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	vec3_t(T) unit_dir = vec3_normalize(T)(v2);\
	float projection_value = vec3_dot(T)(v1, unit_dir);\
	return vec3_scale(T)(unit_dir, projection_value);\
}

/* vec3_forward: Returns the forward direction vector (vector3)
 * returns: vec3_t(T) forward direction, i.e. Vector3.forward [+ve z axis direction]
 */
#define instantiate_implementation_vec3_forward(T)\
vec3_t(T) vec3_forward(T)()\
{\
	vec3_t(T) v = { 0, 0, 1 };\
	return v;\
}

/* vec3_backward: Return the backward direction vector (vector3)
 * returns: vec3_t(T) backward direction, i.e. Vector3.backward [-ve z axis direction]
 */
#define instantiate_implementation_vec3_backward(T)\
vec3_t(T) vec3_backward(T)()\
{\
	vec3_t(T) v = { 0, 0, -1 };\
	return v;\
}

/* vec3_right: Returns the right direction vector (vector3)
 * returns: vec3_t(T) right direction, i.e. Vector3.right [+ve x axis direction]
 */
#define instantiate_implementation_vec3_right(T)\
vec3_t(T) vec3_right(T)()\
{\
	vec3_t(T) v =  { 1, 0, 0 };\
	return v;\
}

/* vec3_left: Returns the left direction vector (vector3)
 * returns: vec3_t(T) left direction, i.e. Vector3.left [-ve x axis direction]
 */
#define instantiate_implementation_vec3_left(T)\
vec3_t(T) vec3_left(T)()\
{\
	vec3_t(T) v =  { -1, 0, 0 };\
	return v;\
}

/* vec3_down: Returns the down direction vector (vector3)
 * returns: vec3_t(T) down direction, i.e. Vector3.down [-ve y axis direction]
 */
#define instantiate_implementation_vec3_down(T)\
vec3_t(T) vec3_down(T)()\
{\
	vec3_t(T) v =  { 0, -1, 0 };\
	return v;\
}

/* vec3_up: Returns the up direction vector (vector3)
 * returns: vec3_t(T) up direction, i.e. Vector3.up [+ve y axis direction]
 */
#define instantiate_implementation_vec3_up(T)\
vec3_t(T) vec3_up(T)()\
{\
	vec3_t(T) v =  { 0, 1, 0 };\
	return v;\
}

/* vec3_abs: Calculates the absolute value of each component of the vector3 v
 * vec3_t(T) v: input vector3 vector
 * returns: vec3_t(T) vector having absolute value components
 */
#define instantiate_implementation_vec3_abs(T)\
vec3_t(T) vec3_abs(T)(vec3_t(T) v)\
{\
	vec3_t(T) v;\
	v.x = abs(v.x);\
	v.y = abs(v.y);\
	v.z = abs(v.z);\
	return v;\
}

/* vec3_angle: Calculates the angle between two vector3 vectors
 * vec3_t(T) v1: first vector involved in the angle calculation
 * vec3_t(T) v2: second vector involved in the angle calculation
 * returns: float +ve angle always
 */
#define instantiate_implementation_vec3_angle(T)\
float vec3_angle(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	if(!(vec3_is_null(T)(v1) && vec3_is_null(T)(v2)))\
		return 0.0f;\
	return acos(vec3_dot(T)(v1, v2) / (vec3_magnitude(T)(v1) * vec3_magnitude(T)(v2)));\
}

/* vec3_rotate: Rotates a vector3 by angle 'angle'
 * vec3_t(T) v: vector to be rotated
 * float angle: angle by which to rotate, +ve angle means anticlockwise rotation, -ve angle means clockwise rotation
 * returns: vec3_t(T) rotated vector
 */
#define instantiate_implementation_vec3_rotate(T)\
vec3_t(T) vec3_rotate(T)(vec3_t(T) v, vec3_t(T) axis, float angle)\
{\
	vec3_t(T) rot_v;\
	float cos_angle = cos(angle);\
	float sin_angle = sin(angle);\
	rot_v.x = v.x * cos_angle - v.y * sin_angle;\
	rot_v.y = v.x * sin_angle + v.y * cos_angle;\
	return rot_v;\
}

/* vec3_normalize: Normalizes a vector3
 * vec3_t(T) v: vector to be normalized
 * returns: vec3_t(T) normalized vector
 */
#define instantiate_implementation_vec3_normalize(T)\
vec3_t(T) vec3_normalize(T)(vec3_t(T) v)\
{\
	if(vec3_is_null(T)(v))\
		return v;\
	float inv_mag = (float)1 / vec3_magnitude(T)(v);\
	vec3_t(T) unit_v;\
	unit_v.x = v.x * inv_mag;\
	unit_v.y = v.y * inv_mag;\
	unit_v.z = v.z * inv_mag;\
	return unit_v;\
}

/* vec3_negate: Negates a vector3
 * vec3_t(T) v: vector to be negated
 * returns: vec3_t(T) negated vector
 */
#define instantiate_implementation_vec3_negate(T)\
vec3_t(T) vec3_negate(T)(vec3_t(T) v)\
{\
	vec3_t(T) neg_v;\
	neg_v.x = -v.x;\
	neg_v.y = -v.y;\
	neg_v.z = -v.z;\
	return neg_v;\
}

/* vec3_magnitude: Calculates the magnitude of vector3
 * vec3_t(T) v: vector of which magnitude to be calculated
 * returns: float magnitude of vector3 v
 */
#define instantiate_implementation_vec3_magnitude(T)\
float vec3_magnitude(T)(vec3_t(T) v)\
{\
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);\
}

/* vec_dot: Calculates the dot product of two vector3's
 * vec3_t(T) v1: first vector involved in the dot product calculation
 * vec3_t(T) v2: second vector involved in the dot product calculation
 * returns: float dot product of v1 and v2
 */
#define instantiate_implementation_vec3_dot(T)\
float vec3_dot(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;\
}

/* vec3: Creates vector3 object in memory taking two arguments
 * x: x-component
 * y: y-component
 * returns: vec_t(T) vector having x, y
 */
#define instantiate_implementation_vec3(T)\
vec3_t(T) vec3(T)(T x, T y, T z)\
{\
	vec3_t(T) v = { x, y, z };\
	return v;\
}

/* See: vec3_null
 */
#define instantiate_implementation_vec3_zero(T) instantiate_implementation_vec3_null(T)

/* vec3_null: Creates a vector3 object in memory having x = 0, and y = 0
 * returns: vec3_t(T) vector having x = 0, y = 0
 */
#define instantiate_implementation_vec3_null(T)\
vec3_t(T) vec3_null(T)()\
{\
	vec3_t(T) null_vector =  { 0, 0, 0 };\
	return null_vector;\
}

/* vec3_is_null: Checks if passed vector3 is null or not
 * vec3_t(T) v: vector3 to be checked for
 * returns: true if vector3 is null vector, otherwise false [boolean]
 */
#define instantiate_implementation_vec3_is_null(T)\
bool vec3_is_null(T)(vec3_t(T) v)\
{\
	return (v.x == 0) || (v.y == 0) || (v.z == 0);\
}

/* vec3_add: Adds two vector3 vectors component wise, i.e. v1 + v2
 * vec3_t(T) v1: first vector involved in addition calculation
 * vec3_t(T) v2: second vector involved in addition calculation
 * returns: vec3_t(T) resulting vector3 vector
 */
#define instantiate_implementation_vec3_add(T)\
vec3_t(T) vec3_add(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	vec3_t(T) result;\
	result.x = v1.x + v2.x;\
	result.y = v1.y + v2.y;\
	result.z = v1.z + v2.z;\
	return result;\
}

/* vec3_sub: Subtracts second vector (v2) from first vector (v1) component wise, i.e. v1 - v2
 * vec3_t(T) v1: first vector involved in subtraction calculation
 * vec3_t(T) v2: second vector involved in subtraction calculation
 * returns: vec3_t(T) resulting vector3 vector
 */
#define instantiate_implementation_vec3_sub(T)\
vec3_t(T) vec3_sub(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	vec3_t(T) result;\
	result.x = v1.x - v2.x;\
	result.y = v1.y - v2.y;\
	result.z = v1.z - v2.z;\
	return result;\
}

/* vec3_mul: Multiplies two vector3 vectors v1 and v2 component wise
 * vec3_t(T) v1: first vector involved in multiplication calculation
 * vec3_t(T) v2: second vector involved in multiplication calculation
 * returns: vec3_t(T) resulting vector3 vector
 */
#define instantiate_implementation_vec3_mul(T)\
vec3_t(T) vec3_mul(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	vec3_t(T) result;\
	result.x = v1.x * v2.x;\
	result.y = v1.y * v2.y;\
	result.z = v1.z * v2.z;\
	return result;\
}

/* vec3_div: Divides first vector (v1) by second vector (v2), i.e. v1 / v2
 * vec3_t(T) v1: first vector involved in division calculation
 * vec3_t(T) v2: second vector involved in division calculation
 * returns: vec3_t(T) resulting vector3 vector
 * exceptions: throws DIVIDE_BY_ZERO exception if v2 is a null vector
 */
#define instantiate_implementation_vec3_div(T)\
vec3_t(T) vec3_div(T)(vec3_t(T) v1, vec3_t(T) v2)\
{\
	EXCEPTION_BLOCK\
	(\
	if(vec3_is_null(T)(v2))\
		THROW_EXCEPTION(DIVIDE_BY_ZERO);\
	)\
	vec3_t(T) result;\
	result.x = v1.x / v2.x;\
	result.y = v1.y / v2.y;\
	result.z = v1.z / v2.z;\
	return result;\
}
/*End: Template Definitions*/

#endif