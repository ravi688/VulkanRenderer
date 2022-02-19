#pragma once

#include <hpml/vec2.h>			// C header

namespace Math
{
	struct Vec2
	{
		union
		{
			vec2_t(float) v;
			struct
			{
				float x, y;
			};
			struct
			{
				float s, t;
			};
		};

		inline Vec2(float x, float y) { v = { x, y }; };
		inline Vec2(float x) { v = { x, x }; }
		inline Vec2() { v = { 0, 0}; }
		inline Vec2(const vec2_t(float)& v) { this->v = v; }


		inline operator vec2_t(float)() { return v; }
		// inline Vec2 operator+ (const Vec2& other) { return vec2_add(float)(2, v, other.v); }
		// inline Vec2 operator* (const Vec2& other) { return vec2_mul(float)(2, v, other.v); }
		// inline Vec2 operator* (float scalar) { return vec2_scale(float)(v, scalar); }
		// inline Vec2 operator- (const Vec2& other) { return vec2_sub(float)(2, v, other.v); }
		// inline Vec2 operator/ (const Vec2& other) { return vec2_div(float)(2, v, other.v); }
		// inline bool operator== (const Vec2& other) { return vec2_is_equal(float)(v, other.v); }
		// inline bool operator!= (const Vec2& other) { return !operator==(other); }

		// inline void print() { vec2_print(float)(v); }
		// inline float magnitude() { return vec2_magnitude(float)(v); }
		// inline float sqrMagnitude() { return vec2_sqrmagnitude(float)(v); }
		// inline Vec2 normalize() { return vec2_normalize(float)(v); }
		// inline Vec2 negate() { return vec2_negate(float)(v); }
		// inline Vec2 rotate(float angle) { return vec2_rotate(float)(v, angle); }
		// inline Vec2 project(const Vec2& to) { return vec2_project(float)(v, to.v); }
		// inline Vec2 unitProject(const Vec2& to) { return vec2_unit_project(float)(v, to.v); }

		// static inline Vec2 lerp(const Vec2& from, const Vec2& to, float t) { return vec2_lerp(float)(from.v, to.v, t); }
		// static inline Vec2 slerp(const Vec2& from, const Vec2& to, float t) { return vec2_slerp(float)(from.v, to.v, t); }
		// static inline float angle(const Vec2& from, const Vec2& to) { return vec2_angle(float)(from.v, to.v); }
		// static inline float unit_angle(const Vec2& from, const Vec2& to) { return vec2_unit_angle(float)(from.v, to.v); }
		// static inline float dot(const Vec2& v1, const Vec2& v2) { return vec2_dot(float)(v1.v, v2.v); }

		// static inline Vec2 one() { return vec2_one(float)(); }
		// static inline Vec2 zero() { return vec2_zero(float)(); }
		// static inline Vec2 up() { return vec2_up(float)(); }
		// static inline Vec2 down() { return vec2_down(float)(); }
		// static inline Vec2 left() { return vec2_left(float)(); }
		// static inline Vec2 right() { return vec2_right(float)(); }

	};
}