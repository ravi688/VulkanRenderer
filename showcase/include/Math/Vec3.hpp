#pragma once

#include <hpml/vec3.h>			// C header

namespace Math
{
	struct Vec3
	{
		union
		{
			vec3_t(float) v;
			struct
			{
				float x, y, z;
			};
			struct
			{
				float r, g, b;
			};
		};

		inline Vec3(float x, float y, float z) { v = { x, y, z }; };
		inline Vec3(float x) { v = { x, x, x }; }
		inline Vec3() { v = { 0, 0, 0 }; }
		inline Vec3(const vec3_t(float)& v) { this->v = v; }


		inline operator vec3_t(float)() { return v; }
		inline Vec3 operator+ (const Vec3& other) { return vec3_add(float)(2, v, other.v); }
		inline Vec3 operator* (const Vec3& other) { return vec3_mul(float)(2, v, other.v); }
		inline Vec3 operator* (float scalar) { return vec3_scale(float)(v, scalar); }
		inline Vec3 operator- (const Vec3& other) { return vec3_sub(float)(2, v, other.v); }
		inline Vec3 operator/ (const Vec3& other) { return vec3_div(float)(2, v, other.v); }
		inline bool operator== (const Vec3& other) { return vec3_is_equal(float)(v, other.v); }
		inline bool operator!= (const Vec3& other) { return !operator==(other); }

		inline void print() { vec3_print(float)(v); }
		inline float magnitude() { return vec3_magnitude(float)(v); }
		inline float sqrMagnitude() { return vec3_sqrmagnitude(float)(v); }
		inline Vec3 normalize() { return vec3_normalize(float)(v); }
		inline Vec3 negate() { return vec3_negate(float)(v); }
		inline Vec3 rotateX(float angle) { return vec3_rotate_x(float)(v, angle); }
		inline Vec3 rotateY(float angle) { return vec3_rotate_y(float)(v, angle); }
		inline Vec3 rotateZ(float angle) { return vec3_rotate_z(float)(v, angle); }
		inline Vec3 rotate(float xAngle, float yAngle, float zAngle) { return vec3_rotate(float)(v, xAngle, yAngle, zAngle); }
		inline Vec3 axisRotate(const Vec3& axis, float angle) { return vec3_axis_rotate(float)(v, axis.x, axis.y, axis.z, angle); }
		inline Vec3 axisRotate(float xAxis, float yAxis, float zAxis, float angle) { return vec3_axis_rotate(float)(v, xAxis, yAxis, zAxis, angle); }
		inline Vec3 project(const Vec3& to) { return vec3_project(float)(v, to.v); }
		inline Vec3 unitProject(const Vec3& to) { return vec3_unit_project(float)(v, to.v); }

		static inline Vec3 lerp(const Vec3& from, const Vec3& to, float t) { return vec3_lerp(float)(from.v, to.v, t); }
		static inline Vec3 slerp(const Vec3& from, const Vec3& to, float t) { return vec3_slerp(float)(from.v, to.v, t); }
		static inline float angle(const Vec3& from, const Vec3& to) { return vec3_angle(float)(from.v, to.v); }
		static inline float unit_angle(const Vec3& from, const Vec3& to) { return vec3_unit_angle(float)(from.v, to.v); }
		static inline float box(const Vec3& from, const Vec3& to, const Vec3& height) { return vec3_box(float)(from.v, to.v, height.v); }
		static inline float dot(const Vec3& v1, const Vec3& v2) { return vec3_dot(float)(v1.v, v2.v); }
		static inline Vec3 cross(const Vec3& from, const Vec3& to) { return vec3_cross(float)(from.v, to.v); }

		static inline Vec3 one() { return vec3_one(float)(); }
		static inline Vec3 zero() { return vec3_zero(float)(); }
		static inline Vec3 up() { return vec3_up(float)(); }
		static inline Vec3 down() { return vec3_down(float)(); }
		static inline Vec3 left() { return vec3_left(float)(); }
		static inline Vec3 right() { return vec3_right(float)(); }
		static inline Vec3 forward() { return vec3_forward(float)(); }
		static inline Vec3 back() { return vec3_back(float)(); }

	};
}