#pragma once

#include <hpml/vec4.h>			// C header

namespace Math
{
	struct Vec4
	{
		union
		{
			vec4_t(float) v;
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
		};

		inline Vec4(float x, float y, float z, float w) { v = { x, y, z, w }; };
		inline Vec4(float x) { v = { x, x, x, x }; }
		inline Vec4() { v = { 0, 0, 0, 0 }; }
		inline Vec4(const vec4_t(float)& v) { this->v = v; }


		inline operator vec4_t(float)() { return v; }
		inline Vec4 operator+ (const Vec4& other) { return vec4_add(float)(2, v, other.v); }
		inline Vec4 operator* (const Vec4& other) { return vec4_mul(float)(2, v, other.v); }
		inline Vec4 operator* (float scalar) { return vec4_scale(float)(v, scalar); }
		inline Vec4 operator- (const Vec4& other) { return vec4_sub(float)(2, v, other.v); }
		inline Vec4 operator/ (const Vec4& other) { return vec4_div(float)(2, v, other.v); }
		inline bool operator== (const Vec4& other) { return vec4_is_equal(float)(v, other.v); }
		inline bool operator!= (const Vec4& other) { return !operator==(other); }

		inline void print() { vec4_print(float)(v); }
		inline float magnitude() { return vec4_magnitude(float)(v); }
		inline float sqrMagnitude() { return vec4_sqrmagnitude(float)(v); }
		inline Vec4 normalize() { return vec4_normalize(float)(v); }
		inline Vec4 negate() { return vec4_negate(float)(v); }
		inline Vec4 rotateX(float angle) { return vec4_rotate_x(float)(v, angle); }
		inline Vec4 rotateY(float angle) { return vec4_rotate_y(float)(v, angle); }
		inline Vec4 rotateZ(float angle) { return vec4_rotate_z(float)(v, angle); }
		inline Vec4 rotate(float xAngle, float yAngle, float zAngle) { return vec4_rotate(float)(v, xAngle, yAngle, zAngle); }
		inline Vec4 axisRotate(const Vec4& axis, float angle) { return vec4_axis_rotate(float)(v, axis.x, axis.y, axis.z, angle); }
		inline Vec4 axisRotate(float xAxis, float yAxis, float zAxis, float angle) { return vec4_axis_rotate(float)(v, xAxis, yAxis, zAxis, angle); }
		inline Vec4 project(const Vec4& to) { return vec4_project(float)(v, to.v); }
		inline Vec4 unitProject(const Vec4& to) { return vec4_unit_project(float)(v, to.v); }

		static inline Vec4 lerp(const Vec4& from, const Vec4& to, float t) { return vec4_lerp(float)(from.v, to.v, t); }
		static inline Vec4 slerp(const Vec4& from, const Vec4& to, float t) { return vec4_slerp(float)(from.v, to.v, t); }
		static inline float angle(const Vec4& from, const Vec4& to) { return vec4_angle(float)(from.v, to.v); }
		static inline float unit_angle(const Vec4& from, const Vec4& to) { return vec4_unit_angle(float)(from.v, to.v); }
		static inline float box(const Vec4& from, const Vec4& to, const Vec4& height) { return vec4_box(float)(from.v, to.v, height.v); }
		static inline float dot(const Vec4& v1, const Vec4& v2) { return vec4_dot(float)(v1.v, v2.v); }

		static inline Vec4 one() { return vec4_one(float)(); }
		static inline Vec4 zero() { return vec4_zero(float)(); }
		static inline Vec4 up() { return vec4_up(float)(); }
		static inline Vec4 down() { return vec4_down(float)(); }
		static inline Vec4 left() { return vec4_left(float)(); }
		static inline Vec4 right() { return vec4_right(float)(); }
		static inline Vec4 forward() { return vec4_forward(float)(); }
		static inline Vec4 back() { return vec4_back(float)(); }

	};
}