
typedef struct 
{
	float r0[4]; 
	float r1[4]; 
	float r2[4]; 
	float r3[4];

	float* const data[4]; 
} mat4_t;

#define IGNORE_CONST(type, value) *(type*)(&value)

#define mat4_data(m) __mat4_data(&m)
float* const* const __mat4_data(mat4_t* m)
{
	IGNORE_CONST(float*, m->data[0]) = &m->r0[0]; 
	IGNORE_CONST(float*, m->data[1]) = &m->r1[0]; 
	IGNORE_CONST(float*, m->data[2]) = &m->r2[0]; 
	IGNORE_CONST(float*, m->data[3]) = &m->r3[0]; 
	return m->data;
}

#define mat4_copy(dst, src) __mat4_copy(&dst, &src)
void __mat4_copy(mat4_t* dst, mat4_t* src)  { memcpy(dst, src, sizeof(float) * 16); }


#define mat4_move(dst, src) __mat4_move(&dst, src)
void __mat4_move(mat4_t* dst, mat4_t src) { __mat4_copy(dst, &src); }

typedef struct 
{
	union 
	{
		struct 
		{
			float x; 
			float y; 
			float z;
		};
		vec3f_t v;
	};

	float w;
} quat_t;


// Vector mathematics functions
static vec3f_t vec3_scale(vec3f_t v, float r) { return (vec3f_t) { v.x * r, v.y * r, v.z * r }; }
static float vec3_dot(vec3f_t v1, vec3f_t v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
static vec3f_t vec3_cross(vec3f_t from, vec3f_t to) { return (vec3f_t) { from.y * to.z - from.z * to.y, from.x * to.z - from.z * to.x, from.x * to.y - from.y * to.x }; }
static vec3f_t __vec3_add(vec3f_t v1, vec3f_t v2) { return (vec3f_t) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }
static vec3f_t vec3_add(u32 count, ...)
{
	va_list args; 
	vec3f_t result = { };
	va_start(args, count);
	while(count > 0)
	{
		vec3f_t v = va_arg(args, vec3f_t);
		v = __vec3_add(result, v);
		--count;
	}
	va_end(args);
	return result;
}
static vec3f_t vec3_sub(vec3f_t v1, vec3f_t v2) { return (vec3f_t) { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; }

//Quaternion mathematics functions
static quat_t quat_conjugate(quat_t q) { return (quat_t) { -q.x, -q.y, -q.z, q.w }; }
static float quat_sqrmagnitude(quat_t q) { return q.x * q.x + q.y * q.y + q.z * q.z; }
static quat_t quat_inverse(quat_t q) { float m = 1 / quat_sqrmagnitude(q); return (quat_t) { q.x * m, q.y * m, q.z * m, q.w * m }; }
static quat_t quat_mul(quat_t q1, quat_t q2) { return (quat_t) { .w = q1.w * q2.w - vec3_dot(q1.v, q2.v), .v =  vec3_add(3, vec3_scale(q1.v, q2.w), vec3_scale(q2.v, q1.w), vec3_cross(q1.v, q2.v)) }; }
static quat_t quat_angle_axis(float angle, vec3f_t axis) { return (quat_t) { .v = vec3_scale(axis, sin(angle * 0.5f)), .w = cos(angle * 0.5f) }; }
static vec3f_t quat_mul_vec3(quat_t q, vec3f_t v) { return quat_mul(q, quat_mul((quat_t) { .v = v, .w = 0 }, quat_inverse(q))).v; }


//Matrix 4x4 mathematics functions
static mat4_t __mat4_mul(mat4_t m1, mat4_t m2)
{
	return (mat4_t)
	{
		m1.r0[0] * m2.r0[0] + m1.r0[1] * m2.r1[0] + m1.r0[2] * m2.r2[0] + m1.r0[3] * m2.r3[0],
		m1.r0[0] * m2.r0[1] + m1.r0[1] * m2.r1[1] + m1.r0[2] * m2.r2[1] + m1.r0[3] * m2.r3[1],
		m1.r0[0] * m2.r0[2] + m1.r0[1] * m2.r1[2] + m1.r0[2] * m2.r2[2] + m1.r0[3] * m2.r3[2],
		m1.r0[0] * m2.r0[3] + m1.r0[1] * m2.r1[3] + m1.r0[2] * m2.r2[3] + m1.r0[3] * m2.r3[3],

		m1.r1[0] * m2.r0[0] + m1.r1[1] * m2.r1[0] + m1.r1[2] * m2.r2[0] + m1.r1[3] * m2.r3[0],
		m1.r1[0] * m2.r0[1] + m1.r1[1] * m2.r1[1] + m1.r1[2] * m2.r2[1] + m1.r1[3] * m2.r3[1],
		m1.r1[0] * m2.r0[2] + m1.r1[1] * m2.r1[2] + m1.r1[2] * m2.r2[2] + m1.r1[3] * m2.r3[2],
		m1.r1[0] * m2.r0[3] + m1.r1[1] * m2.r1[3] + m1.r1[2] * m2.r2[3] + m1.r1[3] * m2.r3[3],

		m1.r2[0] * m2.r0[0] + m1.r2[1] * m2.r1[0] + m1.r2[2] * m2.r2[0] + m1.r2[3] * m2.r3[0],
		m1.r2[0] * m2.r0[1] + m1.r2[1] * m2.r1[1] + m1.r2[2] * m2.r2[1] + m1.r2[3] * m2.r3[1],
		m1.r2[0] * m2.r0[2] + m1.r2[1] * m2.r1[2] + m1.r2[2] * m2.r2[2] + m1.r2[3] * m2.r3[2],
		m1.r2[0] * m2.r0[3] + m1.r2[1] * m2.r1[3] + m1.r2[2] * m2.r2[3] + m1.r2[3] * m2.r3[3],

		m1.r3[0] * m2.r0[0] + m1.r3[1] * m2.r1[0] + m1.r3[2] * m2.r2[0] + m1.r3[3] * m2.r3[0],
		m1.r3[0] * m2.r0[1] + m1.r3[1] * m2.r1[1] + m1.r3[2] * m2.r2[1] + m1.r3[3] * m2.r3[1],
		m1.r3[0] * m2.r0[2] + m1.r3[1] * m2.r1[2] + m1.r3[2] * m2.r2[2] + m1.r3[3] * m2.r3[2],
		m1.r3[0] * m2.r0[3] + m1.r3[1] * m2.r1[3] + m1.r3[2] * m2.r2[3] + m1.r3[3] * m2.r3[3],
	};
}; 
static mat4_t mat4_identity() { return (mat4_t) { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } }; }
static mat4_t mat4_mul(u32 count, ...)
{
	mat4_t result = mat4_identity();
	va_list args; 
	va_start(args, count); 
	while(count > 0)
	{
		//TODO: this should be like: move(,)
		mat4_move(result, __mat4_mul(result, va_arg(args, mat4_t)));
		--count;
	}
	va_end(args);
	return result;
}
static vec4f_t mat4_mul_vec4(mat4_t m, float x, float y, float z, float w)
{
	vec4f_t vector = 
	{
		m.r0[0] * x + m.r0[1] * y + m.r0[2] * z + m.r0[3] * w, 
		m.r1[0] * x + m.r1[1] * y + m.r1[2] * z + m.r1[3] * w, 
		m.r2[0] * x + m.r2[1] * y + m.r2[2] * z + m.r2[3] * w, 
		m.r3[0] * x + m.r3[1] * y + m.r3[2] * z + m.r3[3] * w
	};
	return vector;
}


static mat4_t mat4_transpose(mat4_t m)
{
	float* const* const v = mat4_data(m);
	return (mat4_t)
	{
		v[0][0], v[1][0], v[2][0], v[3][0],
		v[0][1], v[1][1], v[2][1], v[3][1],
		v[0][2], v[1][2], v[2][2], v[3][2], 
		v[0][3], v[1][3], v[2][3], v[3][3] 
	};
}

#define mat4_build_cofactor(m, out_mptr, row, column) __mat4_build_cofactor(mat4_data(m), __mat4_data(out_mptr), 4, row, column)
void __mat4_build_cofactor(float* const* const baseMatrix, float* const* const cofactorMatrix,  u32 n, u32 row, u32 column)
{
	for(u32 i = 0, g = 0; i < (n - 1); i++, g++)
	{
		if(g == row) g++;
		for(u32 j = 0, h = 0; j < (n - 1); j++, h++)
		{
			if(h == column) h++;
			cofactorMatrix[i][j] = baseMatrix[g][h];
		}
	}
}

#define mat4_determinant(m) __mat4_determinant(mat4_data(m), 4)
static float __mat4_determinant(float* const* const mat, u32 n)
{
	if(n <= 1)
		return 0;
	if(n == 2)
		return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

	float result = 0;
	signed char sign = 1;
	for(u32 i = 0; i < n; i++, sign *= -1)
	{
		float element = mat[0][i];
		if(element == 0) continue;
		//Construct a matrix with order n -1
		float _mat[n - 1][n - 1];
		float* const data[n - 1];
		for(u32 l = 0; l < (n - 1); l++)
			IGNORE_CONST(float*, data[l]) = &_mat[l][0];
		__mat4_build_cofactor(mat, (float* const* const)data, n, 0, i);
		result += element * sign * __mat4_determinant((float* const* const)data, n - 1);
	}
	return result;
}

#define mat4_cofactor(m, row, column) __mat4_cofactor(mat4_data(m), 4, row, column)
static float __mat4_cofactor(float* const* const m, u32 n, u32 row, u32 column)
{
	float _m[n - 1][n - 1]; 
	float* const data[n - 1]; 
	for(u32 i = 0; i < (n - 1); i++)
		IGNORE_CONST(float*, data[i]) = &_m[i][0]; 
	__mat4_build_cofactor(m, (float* const* const)data, n, row, column); 
	return __mat4_determinant((float* const* const)data, n - 1) * ((((row + column) % 2 )== 0) ? 1 : -1);
}

static mat4_t mat4_cofactor_matrix(mat4_t m)
{
	mat4_t cofactorMatrix;
	float* const* const data = mat4_data(cofactorMatrix);
	for(u32 i = 0; i < 4; i++)
		for(u32 j = 0; j < 4; j++)
			//TODO: optimize this, we can get the mat4_data out of this loop, see the macro definition of mat4_cofactor
			data[i][j] = mat4_cofactor(m, i, j); 
	return cofactorMatrix;
}

static mat4_t mat4_mul_with_scalar(mat4_t m, float x)
{
	float* const* const v = mat4_data(m);
	return (mat4_t)
	{
		v[0][0] * x, v[0][1] * x, v[0][2] * x, v[0][3] * x,
		v[1][0] * x, v[1][1] * x, v[1][2] * x, v[1][3] * x,
		v[2][0] * x, v[2][1] * x, v[2][2] * x, v[2][3] * x,
		v[3][0] * x, v[3][1] * x, v[3][2] * x, v[3][3] * x,
	};
}

static mat4_t mat4_inverse(mat4_t m)
{
	/*
	Matrix inverse: 
	 = transpose(Confactor matrix (M)) / Det (M) or Adjoint(m) / Det(m)
	*/
	float det = mat4_determinant(m);
EXCEPTION_BLOCK
(
	if(det == 0)
		throw_exception(DIVIDE_BY_ZERO);
)
	mat4_move(m, mat4_transpose(mat4_cofactor_matrix(m))); 
	return mat4_mul_with_scalar(m, 1 / det);
}
