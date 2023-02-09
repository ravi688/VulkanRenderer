
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

#include <renderer/defines.h>

#define DEFAULT_TEST_NAME "SPOT_LIGHT"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define GPU_TYPE RENDERER_GPU_TYPE_DISCRETE
#define FULL_SCREEN 0
#define RESIZABLE 1
#define WINDOW_NAME "3D Renderer"

#if (FULL_SCREEN == 1)
#	undef WINDOW_WIDTH
#	define WINDOW_WIDTH 1920
#	undef WINDOW_HEIGHT
#	define WINDOW_HEIGHT 1080
#endif

typedef struct renderer_t renderer_t;

typedef struct test_t
{
	void* user_data;
	void (*initialize)(renderer_t* driver, void* user_data);
	void (*render)(renderer_t* driver, void* user_data);
	void (*update)(renderer_t* driver, float deltaTime, void* user_data);
	void (*terminate)(renderer_t* driver, void* user_data);
} test_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API test_t* test_new();
RENDERER_API test_t* test_create(const char* test_name);
RENDERER_API void test_destroy(test_t* test);

END_CPP_COMPATIBLE


#include <renderer/alloc.h>

#define TEST_DATA(NAME) struct NAME##_t
#define TEST_NAME(NAME) NAME##_name
#define TEST_DECLARE(NAME)\
extern const char* NAME##_name;\
RENDERER_API test_t* NAME##_get_callbacks(test_t* test)

#define SETUP_TEST(NAME) \
TEST_DATA(NAME);\
const char* TEST_NAME(NAME) = #NAME;\
static void initialize(renderer_t* renderer, TEST_DATA(NAME)* this);\
static void terminate(renderer_t* renderer, TEST_DATA(NAME)* this);\
static void update(renderer_t* renderer, float deltaTime, TEST_DATA(NAME)* this);\
static void render(renderer_t* renderer, TEST_DATA(NAME)* this);\
RENDERER_API test_t* NAME##_get_callbacks(test_t* test)\
{\
	test->initialize = (void*)initialize;\
	test->terminate = (void*)terminate;\
	test->update = (void*)update;\
	test->render = (void*)render;\
	test->user_data = heap_alloc(sizeof(TEST_DATA(NAME)));\
}


#define TEST_ON_INITIALIZE(NAME) static void initialize(renderer_t* renderer, TEST_DATA(NAME)* this)
#define TEST_ON_TERMINATE(NAME) static void terminate(renderer_t* renderer, TEST_DATA(NAME)* this)
#define TEST_ON_UPDATE(NAME) static void update(renderer_t* renderer, float deltaTime, TEST_DATA(NAME)* this)
#define TEST_ON_RENDER(NAME) static void render(renderer_t* renderer, TEST_DATA(NAME)* this)
