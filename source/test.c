
#include <renderer/test.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>
#include <string.h>
#include <stdio.h>

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

#include <renderer/tests/TID-14.case1.h>
#include <renderer/tests/TID-14.case2.h>
#include <renderer/tests/TID-14.case3.h>

#include <renderer/tests/TID-28.case1.h>
#include <renderer/tests/TID-28.case2.h>
#include <renderer/tests/TID-28.case3.h>
#include <renderer/tests/TID-28.case4.h>

#include <renderer/tests/TID-42.case1.h>

RENDERER_API test_t* test_new(memory_allocator_t* allocator)
{
	test_t* test = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST, test_t);
	memzero(test, test_t);
	return test;
}

#define IF(NAME) if(strcmp(name, TEST_NAME(NAME)) == 0) NAME##_get_callbacks(test)
#define ELSE_IF(NAME) else IF(NAME)

RENDERER_API test_t* test_create(memory_allocator_t* allocator, const char* name)
{
	test_t* test = test_new(allocator);
	test->allocator = allocator;
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
	ELSE_IF(TID_14_CASE_1);
	ELSE_IF(TID_14_CASE_2);
	ELSE_IF(TID_14_CASE_3);
	ELSE_IF(TID_28_CASE_1);
	ELSE_IF(TID_28_CASE_2);
	ELSE_IF(TID_28_CASE_3);
	ELSE_IF(TID_28_CASE_4);
	ELSE_IF(TID_42_CASE_1);
	else
	{
		printf 	(
					"supported tests: \n"
					"\tDEPTH_RENDER_TEXTURE\n"
					"\tENVIRONMENT_REFLECTIONS\n"
					"\tENVIRONMENT_REFLECTIONS_LOAD\n"
					"\tDEPTH_CUBE_RENDER_TEXTURE\n"
					"\tDEPTH_CUBE_RENDER_TEXTURE_LOAD\n"
					"\tPOINT_LIGHT_SHADOWS\n"
					"\tPOINT_LIGHT_SHADOWS_LOAD\n"
					"\tSPOT_LIGHT\n"
					"\tSPOT_LIGHT_LOAD\n"
					"\tCUBE\n"
					"\tTID_14_CASE_1\n"
					"\tTID_14_CASE_2\n"
					"\tTID_14_CASE_3\n"
					"\tTID_28_CASE_1\n"	
					"\tTID_28_CASE_2\n"	
					"\tTID_28_CASE_3\n"	
					"\tTID_28_CASE_4\n"	
					"\tTID_42_CASE_1\n"	
				);
		exit(0);
	}
	return test;
}

RENDERER_API void test_destroy(test_t* test)
{
	memory_allocator_dealloc(test->allocator, test);
}
