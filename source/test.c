
#include <renderer/test.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>
#include <string.h>

#include <renderer/tests/depth_render_texture.h>
#include <renderer/tests/depth_render_texture.load.h>
#include <renderer/tests/environment_reflections.h>
#include <renderer/tests/environment_reflections.load.h>
#include <renderer/tests/depth_cube_render_texture.h>
#include <renderer/tests/depth_cube_render_texture.load.h>
#include <renderer/tests/point_light_shadows.h>
#include <renderer/tests/point_light_shadows.load.h>
#include <renderer/tests/spot_light.h>
#include <renderer/tests/spot_light.load.h>

#include <renderer/tests/cube.h>

RENDERER_API test_t* test_new()
{
	test_t* test = heap_new(test_t);
	memzero(test, test_t);
	return test;
}

#define IF(NAME) if(strcmp(name, TEST_NAME(NAME)) == 0) NAME##_get_callbacks(test)
#define ELSE_IF(NAME) else IF(NAME)

RENDERER_API test_t* test_create(const char* name)
{
	test_t* test = test_new();
	IF(DEPTH_RENDER_TEXTURE);
	ELSE_IF(DEPTH_RENDER_TEXTURE_LOAD);
	ELSE_IF(ENVIRONMENT_REFLECTIONS);
	ELSE_IF(ENVIRONMENT_REFLECTIONS_LOAD);
	ELSE_IF(DEPTH_CUBE_RENDER_TEXTURE);
	ELSE_IF(DEPTH_CUBE_RENDER_TEXTURE_LOAD);
	ELSE_IF(POINT_LIGHT_SHADOWS);
	ELSE_IF(POINT_LIGHT_SHADOWS_LOAD);
	ELSE_IF(SPOT_LIGHT);
	ELSE_IF(SPOT_LIGHT_LOAD);
	ELSE_IF(CUBE);
	else LOG_FETAL_ERR("Unrecognized test: %s\n", name);
	return test;
}

RENDERER_API void test_destroy(test_t* test)
{
	heap_free(test);
}
