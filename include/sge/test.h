/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: test.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#pragma once

/*
	1. Default clear screen render pass after creating a camera [done]
	2. Test Shader & Material, Default Shaders [done]
	3. Rendering [done]
	4. Render Queue [done]
	5. Muliple render passes (shadow mapping) [ done ]
	6. Multiple sub passes (greyscale) [ done ]
	7. Shadow mapping improvement [ done ]
	8. Lights [ done ]
	9. Multiple light system [ in-progress ]
	10. Deferred rendering & forward rendering
	11. Camera and Light as a Render Object in the scene
	12. Depth texture sharing across multiple render passes
	13. Each shader will have prefered render pass description, if a render pass in the render pass pool
		suffice to meet the requirements of the shader then that pass should be used instead of creating
		another one.
 */

#include <sge/defines.h>

typedef struct renderer_t renderer_t;

typedef struct renderer_initialization_data_t
{
	u32 /* renderer_gpu_type_t*/ gpu_type;
	u32 window_width;
	u32 window_height;
	const char* window_name;
	bool is_fullscreen;
	bool is_resizable;
	bool require_bitmap_text;
	u32 max_point_lights;
	u32 max_spot_lights;
	u32 max_far_lights;
} renderer_initialization_data_t;

typedef struct test_t
{
	memory_allocator_t* allocator;
	void* user_data;

	renderer_initialization_data_t (*renderer_initialize)(void);
	void (*initialize)(renderer_t* driver, void* user_data);
	void (*render)(renderer_t* driver, void* user_data);
	void (*update)(renderer_t* driver, float deltaTime, void* user_data);
	void (*terminate)(renderer_t* driver, void* user_data);
} test_t;

BEGIN_CPP_COMPATIBLE

SGE_API test_t* test_new(memory_allocator_t* allocator);
SGE_API test_t* test_create(memory_allocator_t* allocator, const char* test_name);
SGE_API void test_destroy(test_t* test);

END_CPP_COMPATIBLE


#include <sge/alloc.h>

#define TEST_DATA(NAME) struct NAME##_t
#define TEST_NAME(NAME) NAME##_name
#define TEST_DECLARE(NAME)\
extern const char* NAME##_name;\
SGE_API void NAME##_setup_callbacks(test_t* test)

#define SETUP_TEST(NAME) \
TEST_DATA(NAME);\
const char* TEST_NAME(NAME) = #NAME;\
static renderer_initialization_data_t renderer_initialize();\
static void initialize(renderer_t* renderer, TEST_DATA(NAME)* this);\
static void terminate(renderer_t* renderer, TEST_DATA(NAME)* this);\
static void update(renderer_t* renderer, float deltaTime, TEST_DATA(NAME)* this);\
static void render(renderer_t* renderer, TEST_DATA(NAME)* this);\
SGE_API void NAME##_setup_callbacks(test_t* test)\
{\
	test->renderer_initialize = (void*)renderer_initialize;\
	test->initialize = (void*)initialize;\
	test->terminate = (void*)terminate;\
	test->update = (void*)update;\
	test->render = (void*)render;\
	test->user_data = heap_alloc(sizeof(TEST_DATA(NAME)));\
}

#define TEST_ON_SGE_INITIALIZE(NAME) static renderer_initialization_data_t renderer_initialize()
#define TEST_ON_INITIALIZE(NAME) static void initialize(renderer_t* renderer, TEST_DATA(NAME)* this)
#define TEST_ON_TERMINATE(NAME) static void terminate(renderer_t* renderer, TEST_DATA(NAME)* this)
#define TEST_ON_UPDATE(NAME) static void update(renderer_t* renderer, float deltaTime, TEST_DATA(NAME)* this)
#define TEST_ON_RENDER(NAME) static void render(renderer_t* renderer, TEST_DATA(NAME)* this)
