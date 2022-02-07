#pragma once

#include <hpml/mat4.h>
#include <hpml/affine_transformation.h>


namespace Math
{
	struct Mat4
	{
		union
		{
			mat4_t(float) m;
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
		};

		inline Mat4()
		{
			mat4_move(float)(&m, mat4_identity(float)());
		}
		inline Mat4(const float (&values)[4][4])
		{
			float** data = (float**)mat4_data(float)(&m);
			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					data[i][j] = values[i][j];
		}
		inline Mat4(const mat4_t(float)& value)
		{
			mat4_move(float)(&m, value);
		}
		inline Mat4(	float v00, float v01, float v02, float v03,
				float v10, float v11, float v12, float v13,
				float v20, float v21, float v22, float v23,
				float v30, float v31, float v32, float v33)
		{
			mat4_move(float)(&m, mat4(float)(v00, v01, v02, v03, v10, v11, v12, v13, v20, v21, v22, v23, v30, v31, v32, v33));
		}
		inline Mat4(float x, float y, float z, float w) { mat4_move(float)(&m, mat4_diagonal(float)(x, y, z, w));}
		inline Mat4(float x)
		{
			for(int i = 0; i < 16; i++)
				m.values[i] = x;
		}

		inline operator mat4_t(float)() { return m; }
		inline Mat4 operator+ (const Mat4& other) { return mat4_add(float)(m, other.m); }
		inline Mat4 operator- (const Mat4& other)  { return mat4_sub(float)(m, other.m); }
		inline Mat4 operator* (const Mat4& other) { return mat4_mul(float)(2, m, other.m); }
		inline Mat4 operator* (float scalar) { return mat4_mul_scalar(float)(m, scalar); }
		inline Mat4 operator/ (const Mat4 other) { return mat4_div(float)(m, other.m); }
		inline bool operator== (const Mat4& other) { return mat4_is_equal(float)(m, other.m); }
		inline bool operator!= (const Mat4& other) { return !operator==(other); }
		inline Mat4 operator=(const Mat4& other)
		{
			mat4_move(float)(&m, other.m);
			return *this;
		}
		inline Mat4 operator=(const mat4_t(float)& _m)
		{
			mat4_move(float)(&m, _m);
			return *this;
		}

		inline void buildCofactor(float* const* const cofactorMatrix, u32 row, u32 column) { mat4_build_cofactor(float)(m, cofactorMatrix, row, column); }
		inline bool isNull() { return mat4_is_null(float)(m); }
		inline Mat4 inverse() { return mat4_inverse(float)(m); }
		inline Mat4 transpose() { return mat4_transpose(float)(m); }
		inline Mat4 negate() { return mat4_negate(float)(m); }
		inline float trace() { return mat4_trace(float)(m); }
		inline float determinant() { return mat4_det(float)(m); }
		inline const float* const* getData() { return mat4_data(float)(&m); }
		inline void print() { mat4_print(float)(m); }

		static inline Mat4 identity() { return mat4_identity(float)(); }
		static inline Mat4 lerp(const Mat4& from, const Mat4& to, float t ) { return mat4_lerp(float)(from.m, to.m, t); }
		static inline Mat4 diagonal(float x, float y, float z, float w) { return mat4_diagonal(float)(x, y, z, w); }
	
		static inline Mat4 translation(float x, float y, float z) { return mat4_translation(float)(x, y, z); }
		static inline Mat4 rotation(float x, float y, float z) { return mat4_rotation(float)(x, y, z); }
		static inline Mat4 scale(float x, float y, float z) { return mat4_scale(float)(x, y, z); }
		static inline Mat4 perspectiveProjection(float nearClipPlane, float farClipPlane, float fov, float aspectRatio)
		{
			return mat4_persp_projection(float)(nearClipPlane, farClipPlane, fov, aspectRatio);
		}
		static inline Mat4 orthographicProjection(float nearClipPlane, float farClipPlane, float height, float aspectRatio)
		{
			return mat4_ortho_projection(float)(nearClipPlane, farClipPlane, height, aspectRatio);
		}
	};
}