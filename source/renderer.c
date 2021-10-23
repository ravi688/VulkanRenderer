//TODO: Write a numeric type conversion warning systme; because of this printf("%u", <float> value) => outputs zero if <float> value < 1

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <garbage_collector/garbage_collector.h>
#include <vulkan/vulkan_wrapper.h>

#include <exception/exception.h>
#include <debug.h>

#include <array/header_config.h>
#include <array/array.h>

#include <defines.h>

#include <math.h>

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

typedef struct 
{
	float position[2]; 
	float color[3];
} vertex2d_t;

typedef struct  
{
	float position[3]; 
	float color[3];
} vertex3d_t;

typedef vertex2d_t* pvertex_t;
instantiate_tuple_t(uint32_t, pvertex_t);

typedef struct renderer_t
{
	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_graphics_queue; 
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;
	uint32_t vk_graphics_queue_index;
	tuple_t(uint32_t, pVkImage_t) vk_images;
	tuple_t(uint32_t, pVkImageView_t) vk_image_views;
	tuple_t(uint32_t, pVkFramebuffer_t) vk_framebuffers;
	tuple_t(uint32_t, pVkCommandBuffer_t) vk_command_buffers;
	VkFormat vk_format; 
	VkExtent2D vk_extent;
	VkPipelineLayout vk_pipeline_layout;
	VkRenderPass vk_render_pass;
	VkPipeline vk_pipeline;
	VkCommandPool vk_command_pool;
	VkSemaphore vk_image_available_semaphore;
	VkSemaphore vk_render_finished_semaphore;

	//Vertex Buffer
	VkBuffer vk_vertex_buffer;
	VkDeviceMemory vk_vertex_memory;

	//Vertex Staging Buffer
	VkBuffer vk_staging_buffer; 
	VkDeviceMemory vk_staging_memory;

	//Index Buffer
	VkBuffer vk_index_buffer; 
	VkDeviceMemory vk_index_memory;

	tuple_t(uint32_t, pvertex_t) vertices;
} renderer_t;


instantiate_array_struct(VkFormat); 
instantiate_declaration_array(VkFormat);
instantiate_implementation_array(VkFormat);

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }


typedef struct { float x, y, z; } vec3_t;


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
		vec3_t v;
	};

	float w;
} quat_t;


//Vector mathematics functions
static vec3_t vec3_scale(vec3_t v, float r) { return (vec3_t) { v.x * r, v.y * r, v.z * r }; }
static float vec3_dot(vec3_t v1, vec3_t v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
static vec3_t vec3_cross(vec3_t from, vec3_t to) { return (vec3_t) { from.y * to.z - from.z * to.y, from.x * to.z - from.z * to.x, from.x * to.y - from.y * to.x }; }
static vec3_t __vec3_add(vec3_t v1, vec3_t v2) { return (vec3_t) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }
static vec3_t vec3_add(u32 count, ...)
{
	va_list args; 
	vec3_t result = { };
	va_start(args, count);
	while(count > 0)
	{
		vec3_t v = va_arg(args, vec3_t);
		v = __vec3_add(result, v);
		--count;
	}
	va_end(args);
	return result;
}
static vec3_t vec3_sub(vec3_t v1, vec3_t v2) { return (vec3_t) { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; }

//Quaternion mathematics functions
static quat_t quat_conjugate(quat_t q) { return (quat_t) { -q.x, -q.y, -q.z, q.w }; }
static float quat_sqrmagnitude(quat_t q) { return q.x * q.x + q.y * q.y + q.z * q.z; }
static quat_t quat_inverse(quat_t q) { float m = 1 / quat_sqrmagnitude(q); return (quat_t) { q.x * m, q.y * m, q.z * m, q.w * m }; }
static quat_t quat_mul(quat_t q1, quat_t q2) { return (quat_t) { .w = q1.w * q2.w - vec3_dot(q1.v, q2.v), .v =  vec3_add(3, vec3_scale(q1.v, q2.w), vec3_scale(q2.v, q1.w), vec3_cross(q1.v, q2.v)) }; }
static quat_t quat_angle_axis(float angle, vec3_t axis) { return (quat_t) { .v = vec3_scale(axis, sin(angle * 0.5f)), .w = cos(angle * 0.5f) }; }
static vec3_t quat_mul_vec3(quat_t q, vec3_t v) { return quat_mul(q, quat_mul((quat_t) { .v = v, .w = 0 }, quat_inverse(q))).v; }


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
static vec3_t mat4_mul_vec4(mat4_t m, float x, float y, float z, float w)
{
	vec3_t vector = 
	{
		m.r0[0] * x + m.r0[1] * y + m.r0[2] * z + m.r0[3] * w, 
		m.r1[0] * x + m.r1[1] * y + m.r1[2] * z + m.r1[3] * w, 
		m.r2[0] * x + m.r2[1] * y + m.r2[2] * z + m.r2[3] * w
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
static mat4_t mat4_translation(vec3_t v)
{
	return (mat4_t)
	{
		1, 0, 0, v.x, 
		0, 1, 0, v.y,
		0, 0, 1, v.z, 
		0, 0, 0, 1
	}; 
}
static mat4_t mat4_rotationX(float angle)
{
	float s = sin(angle); 
	float c = cos(angle);
	return (mat4_t) 
	{
		1,  0, 0, 0,
		 0, c, -s, 0,
		0, s, c, 0, 
		 0, 0, 0, 1
	};
}
static mat4_t mat4_rotationY(float angle)
{
	float s = sin(angle); 
	float c = cos(angle);	
	return (mat4_t) 
	{
		 c, 0, -s, 0, 
		 0, 1, 0, 0, 
		s, 0, c, 0, 
		 0, 0, 0, 1
	};
}
static mat4_t mat4_rotationZ(float angle)
{
	float s = sin(angle); 
	float c = cos(angle);
	return (mat4_t)
	{
		 c, -s, 0, 0, 
		s, c, 0, 0, 
		 0, 0, 1, 0, 
		 0, 0, 0, 1
	};
}
static mat4_t mat4_rotation(vec3_t eulerRotation) { return mat4_mul(3, mat4_rotationX(eulerRotation.x), mat4_rotationY(eulerRotation.y), mat4_rotationZ(eulerRotation.z)); }
static mat4_t mat4_transform(vec3_t position, vec3_t eulerRotation) { return mat4_mul(2, mat4_translation(position), mat4_rotation(eulerRotation)); }

static mat4_t mat4_ortho_projection(vec3_t cameraPosition, vec3_t cameraRotation)
{
	mat4_t cameraTransform = mat4_transform(cameraPosition, cameraRotation); 
	
}

static mat4_t mat4_persp_projection(vec3_t cameraPosition, vec3_t cameraRotation)
{
	return (mat4_t) { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
}

static void mat4_dump(mat4_t m)
{
	float* const* const v = mat4_data(m);
	log_msg("Row[0] => { %.2f, %.2f, %.2f, %.2f }", v[0][0], v[0][1], v[0][2], v[0][3]);
	log_msg("Row[1] => { %.2f, %.2f, %.2f, %.2f }", v[1][0], v[1][1], v[1][2], v[1][3]);
	log_msg("Row[2] => { %.2f, %.2f, %.2f, %.2f }", v[2][0], v[2][1], v[2][2], v[2][3]);
	log_msg("Row[3] => { %.2f, %.2f, %.2f, %.2f }", v[3][0], v[3][1], v[3][2], v[3][3]);
}

static void convert_to_vulkan_clipspace(vertex2d_t *  const vertices, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vertices[i].position[1] = -vertices[i].position[1];
}

static vertex2d_t* foreach_vertex3d(u32 count, vertex3d_t* vertices, mat4_t m)
{
	vertex2d_t* vertices2d = GC_NEWV(vertex2d_t, count);
	for(u32 i = 0; i < count; i++)
	{
		vec3_t v = mat4_mul_vec4(m, vertices[i].position[0], vertices[i].position[1], vertices[i].position[2], 1);
		vertices2d[i].position[0] = v.z; 
		vertices2d[i].position[1] = v.y;

		//TODO: this should be like vertices2d[i].color = vertices[i].color
		//but compiler denies because of the array type; write a function array_copy(T, count)(vertices2d[i].color, vertices[i].color)
		vertices2d[i].color[0] = vertices[i].color[0];
		vertices2d[i].color[1] = vertices[i].color[1];
		vertices2d[i].color[2] = vertices[i].color[2];
	}
	return vertices2d;
}

//TODO: Wrapp this physical device selection & creation of logical device into a single function
renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	renderer->vk_instance = vk_create_instance(); 
	renderer->vk_physical_device = vk_get_suitable_physical_device(vk_get_physical_devices(renderer->vk_instance));
	renderer->vk_device = vk_get_device(renderer->vk_physical_device);
	return renderer;
}

void renderer_init_surface(renderer_t* renderer, void* surface)
{
	//Surfaces and Swapchains
	renderer->vk_surface = *((VkSurfaceKHR*)surface);
	VkSwapchainCreateInfoKHR swapchainCreateInfo  = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = 3, 
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { 600, 500 }, 
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1, 
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, 
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, 
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR, 
		.clipped = VK_TRUE,
		.surface = renderer->vk_surface,
		.oldSwapchain = VK_NULL_HANDLE
	};
	renderer->vk_swapchain = vk_get_swapchain(renderer->vk_device, &swapchainCreateInfo);
	renderer->vk_render_pass = vk_get_render_pass(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB);
	renderer->vk_images = vk_get_images(renderer->vk_device, renderer->vk_swapchain);
	renderer->vk_image_views = vk_get_image_views(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, renderer->vk_images.value1, renderer->vk_images.value2);
	renderer->vk_framebuffers = vk_get_framebuffers(renderer->vk_device, 3, renderer->vk_render_pass, (VkExtent2D) { 600, 500 }, 1, renderer->vk_image_views.value2);
	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device)); 
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, 3);
	renderer->vk_image_available_semaphore = vk_get_semaphore(renderer->vk_device); 
	renderer->vk_render_finished_semaphore = vk_get_semaphore(renderer->vk_device);
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device), 0);

	//Graphics Pipeline
	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer->vk_device);

	VkPipelineShaderStageCreateInfo shaderStages[2] = 
	{
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/vertexShader.vert"), VERTEX_SHADER, "main"),
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/fragmentShader.frag"), FRAGMENT_SHADER, "main")
	};

	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };
	uint32_t offsets[2] = { offsetof(vertex2d_t, position), offsetof(vertex2d_t, color) };
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(	3, 
																											sizeof(vertex2d_t), 
																											VK_VERTEX_INPUT_RATE_VERTEX, 
																											formats, 
																											offsets);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(600, 500);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	renderer->vk_pipeline = vk_get_graphics_pipeline(	renderer->vk_device, renderer->vk_pipeline_layout, renderer->vk_render_pass,
														shaderStages,
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	//Geometry Data
	vertex3d_t geometry3D[4] = 
	{
		{ { 0.5f, 0,  0.5f }, { 1, 1, 1 } }, 
	 	{ { 0.5f, 0,  -0.5f }, { 1, 0, 0} },
	 	{ { -0.5f, 0,  -0.5f }, { 0, 0, 0 } },
	 	{ { -0.5f, 0, 0.5f }, { 0, 0,0 } }
	}; 

	u32 vertexCount = 4;
	mat4_t cameraTransform = mat4_transform((vec3_t) { -1, 1, 0 }, (vec3_t) { 0, 0, -30 * DEG2RAD } );
	vertex2d_t* vertices = foreach_vertex3d(vertexCount, geometry3D, mat4_inverse(cameraTransform));

	convert_to_vulkan_clipspace(vertices, vertexCount); 
	u32 indices[] = 
	{
		//clockwise order
		2, 1, 0, 3, 2, 0
	}; 
	u32 indexCount = sizeof(indices) / sizeof(u32);

	//Staging buffer preparation
	renderer->vk_staging_buffer = vk_get_buffer(renderer->vk_device, vertexCount * sizeof(vertex2d_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_staging_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_staging_buffer, vertexCount * sizeof(vertex2d_t), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(renderer->vk_device, renderer->vk_staging_memory, 0, vertexCount * sizeof(vertex2d_t), 0, &data);
	memcpy(data, vertices, sizeof(vertex2d_t) * vertexCount);
	vkUnmapMemory(renderer->vk_device, renderer->vk_staging_memory);


	//Vertex Buffer
	renderer->vk_vertex_buffer = vk_get_buffer(renderer->vk_device, vertexCount * sizeof(vertex2d_t), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE);
	renderer->vk_vertex_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_vertex_buffer, vertexCount * sizeof(vertex2d_t), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	//Recording commands to copy the staging buffer to vertex buffer
	VkCommandBufferAllocateInfo allocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 
		.commandPool = renderer->vk_command_pool, 
		.commandBufferCount = 1, 
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
	}; 
	VkCommandBuffer transferCommandBuffer;
	vkCall(vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, &transferCommandBuffer));

	VkCommandBufferBeginInfo beginInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 
	}; 

	vkCall(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo));
	VkBufferCopy copyRegion = 
	{
		.srcOffset = 0, 
		.dstOffset = 0, 
		.size = vertexCount * sizeof(vertex2d_t)
	}; 

	vkCmdCopyBuffer(transferCommandBuffer, renderer->vk_staging_buffer, renderer->vk_vertex_buffer, 1, &copyRegion); 
	vkCall(vkEndCommandBuffer(transferCommandBuffer));

	VkSubmitInfo submitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, 
		.commandBufferCount = 1, 
		.pCommandBuffers = &transferCommandBuffer,
	}; 
	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(renderer->vk_graphics_queue);
	//Destroy Staging Buffer
	vkDestroyBuffer(renderer->vk_device, renderer->vk_staging_buffer, NULL); 
	//Free the staging buffer memory 
	vkFreeMemory(renderer->vk_device, renderer->vk_staging_memory, NULL); 
	//Destroy the transfer Command buffer
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, 1, &transferCommandBuffer);


	//TODO: We can record the copy buffer commands of vertex and index buffers both in a record pass ^^ above
	renderer->vk_staging_buffer = vk_get_buffer(renderer->vk_device, sizeof(indices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_staging_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_staging_buffer, sizeof(indices), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); 

	vkMapMemory(renderer->vk_device, renderer->vk_staging_memory, 0, sizeof(indices), 0, &data);
	memcpy(data, indices, sizeof(indices)); 
	vkUnmapMemory(renderer->vk_device, renderer->vk_staging_memory);

	renderer->vk_index_buffer = vk_get_buffer(renderer->vk_device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_staging_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_index_buffer, sizeof(indices), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	vkCall(vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, &transferCommandBuffer));
	vkCall(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo));
	copyRegion.size = sizeof(indices); 
	vkCmdCopyBuffer(transferCommandBuffer, renderer->vk_staging_buffer, renderer->vk_index_buffer, 1, &copyRegion);
	vkCall(vkEndCommandBuffer(transferCommandBuffer)); 
	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE); 
	vkQueueWaitIdle(renderer->vk_graphics_queue); 
	vkDestroyBuffer(renderer->vk_device, renderer->vk_staging_buffer, NULL); 
	vkFreeMemory(renderer->vk_device, renderer->vk_staging_memory, NULL); 
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, 1, &transferCommandBuffer);


	//Recording commands
	for(uint32_t i = 0; i < 3; i++)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; 
		vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[i], &beginInfo));
		VkClearValue clearValue;
		clearValue.color.float32[0] = 0;
		clearValue.color.float32[1] = 0.3f;
		clearValue.color.float32[2] = 1;
		clearValue.color.float32[3] = 1;
		VkRenderPassBeginInfo renderPassBeginInfo =  
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.renderArea.offset = (VkOffset2D) { 0, 0 }, 
			.renderArea.extent = (VkExtent2D) { 600, 500 }, 
			.framebuffer = renderer->vk_framebuffers.value2[i], 
			.renderPass = renderer->vk_render_pass,
			.clearValueCount = 1,
			.pClearValues = &clearValue
		};
		vkCmdBeginRenderPass(renderer->vk_command_buffers.value2[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); 
		vkCmdBindPipeline(renderer->vk_command_buffers.value2[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->vk_pipeline);
		VkBuffer vertexBuffers[1] =  { renderer->vk_vertex_buffer };
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(renderer->vk_command_buffers.value2[i], renderer->vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(renderer->vk_command_buffers.value2[i], sizeof(indices), 1, 0, 0, 0);
		// vkCmdDraw(renderer->vk_command_buffers.value2[i], vertexCount, 1, 0, 0);
		vkCmdEndRenderPass(renderer->vk_command_buffers.value2[i]);
		vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[i]));
	}
}

void renderer_update(renderer_t* renderer)
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(renderer->vk_device, renderer->vk_swapchain, UINT64_MAX, renderer->vk_image_available_semaphore, VK_NULL_HANDLE, &imageIndex);
	if(result == VK_ERROR_OUT_OF_DATE_KHR)
		log_err("VK_ERROR_OUT_OF_DATE_KHR");

	uint32_t waitDstMask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1, 
		.pCommandBuffers = &(renderer->vk_command_buffers.value2[imageIndex]),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->vk_image_available_semaphore),
		.pWaitDstStageMask = &waitDstMask,
		.signalSemaphoreCount = 1, 
		.pSignalSemaphores = &(renderer->vk_render_finished_semaphore)
	}; 

	vkCall(vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE));

	VkPresentInfoKHR presentInfo =  
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1, 
		.pSwapchains = &(renderer->vk_swapchain),
		.waitSemaphoreCount = 1, 
		.pWaitSemaphores = &(renderer->vk_render_finished_semaphore),
		.pImageIndices = &imageIndex
	};

	vkCall(vkQueuePresentKHR(renderer->vk_graphics_queue, &presentInfo));
}

void renderer_terminate(renderer_t* renderer)
{

}