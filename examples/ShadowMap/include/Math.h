
#pragma once

#include <Defines.h>

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f
#define MILLISECONDS * 0.001f
#define SECONDS 

#include <hpml/vec4.h>
#include <hpml/vec3.h>
#include <hpml/mat4.h>
#include <hpml/affine_transformation.h>
#include <math.h>

#define mat4f mat4_t(float)
#define vec4f vec4_t(float)
#define vec3f vec3_t(float)
#define vec2f vec2_t(float)

#define MAT4 (mat4f)
#define VEC4 (vec4f)
#define VEC3 (vec3f)
#define VEC2 (vec2f)

static ALWAYS_INLINE vec4f vec4f_add(vec4f v1, vec4f v2) { return vec4_add(float)(2, v1, v2); }
static ALWAYS_INLINE vec4f vec4f_sub(vec4f v1, vec4f v2) { return vec4_sub(float)(2, v1, v2); }
static ALWAYS_INLINE vec4f vec4f_mul(vec4f v1, vec4f v2) { return vec4_mul(float)(2, v1, v2); }
static ALWAYS_INLINE vec4f vec4f_scale(vec4f v1, float v2) { return vec4_scale(float)(v1, v2); }
static ALWAYS_INLINE vec4f vec4f_normalize(vec4f v) { return vec4_normalize(float)(v); }

static ALWAYS_INLINE vec3f vec3f_add(vec3f v1, vec3f v2) { return vec3_add(float)(2, v1, v2); }
static ALWAYS_INLINE vec3f vec3f_sub(vec3f v1, vec3f v2) { return vec3_sub(float)(2, v1, v2); }
static ALWAYS_INLINE vec3f vec3f_mul(vec3f v1, vec3f v2) { return vec3_mul(float)(2, v1, v2); }
static ALWAYS_INLINE vec3f vec3f_scale(vec3f v1, float v2) { return vec3_scale(float)(v1, v2); }
static ALWAYS_INLINE vec3f vec3f_normalize(vec3f v) { return vec3_normalize(float)(v); }

static ALWAYS_INLINE mat4f mat4f_rotate(vec3f v) { return mat4_rotation(float)(v.x, v.y, v.z); }
static ALWAYS_INLINE mat4f mat4f_translate(vec3f v) { return mat4_translation(float)(v.x, v.y, v.z); }
static ALWAYS_INLINE mat4f mat4f_scale(vec3f v) { return mat4_scale(float)(v.x, v.y, v.z); }
static ALWAYS_INLINE mat4f mat4f_mul(mat4f m1, mat4f m2) { return mat4_mul(float)(2, m1, m2); }
static ALWAYS_INLINE mat4f mat4f_transform(vec3f position, vec3f eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }
static ALWAYS_INLINE mat4f mat4f_persp_project(float nearClipPlane, float farClipPlane, float fov, float aspectRatio)
{
	return mat4_persp_projection(float)(nearClipPlane, farClipPlane, fov, aspectRatio);
}
static ALWAYS_INLINE mat4f mat4f_ortho_project(float nearClipPlane, float farClipPlane, float height, float aspectRatio)
{
	return mat4_ortho_projection(float)(nearClipPlane, farClipPlane, height, aspectRatio);
}
static ALWAYS_INLINE mat4f mat4f_identity() { return mat4_identity(float)(); }
static ALWAYS_INLINE mat4f mat4f_inverse(mat4f m) { return mat4_inverse(float)(m); }
static ALWAYS_INLINE mat4f mat4f_transpose(mat4f m) { return mat4_transpose(float)(m); }
