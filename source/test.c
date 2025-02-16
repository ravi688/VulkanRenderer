/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: test.c is a part of VulkanRenderer

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


#include <sge/test.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h>
#include <sge/assert.h>
#include <string.h>
#include <stdio.h>

#include <sge/tests/depth_render_texture.h>
#include <sge/tests/depth_render_texture.load.h>
#include <sge/tests/environment_reflections.h>
#include <sge/tests/environment_reflections.load.h>
#include <sge/tests/depth_cube_render_texture.h>
#include <sge/tests/depth_cube_render_texture.load.h>
#include <sge/tests/point_light_shadows.h>
#include <sge/tests/point_light_shadows.load.h>
#include <sge/tests/spot_light.h>
#include <sge/tests/spot_light.load.h>
#include <sge/tests/multiple_lights.load.h>
#include <sge/tests/multiple_spot_lights.load.h>

#include <sge/tests/cube.h>
#include <sge/tests/texture_sampling.h>
#include <sge/tests/texture_sampling_alpha_channel.h>
#include <sge/tests/bitmap_glyph_atlas_texture.h>
#include <sge/tests/text_mesh.h>
#include <sge/tests/text_mesh.load.h>
#include <sge/tests/bitmap_text.h>
#include <sge/tests/bitmap_text.load.h>

#include <sge/tests/sdf_rounded_rect.h>

#include <sge/tests/TID-14.case1.h>
#include <sge/tests/TID-14.case2.h>
#include <sge/tests/TID-14.case3.h>

#include <sge/tests/TID-28.case1.h>
#include <sge/tests/TID-28.case2.h>
#include <sge/tests/TID-28.case3.h>
#include <sge/tests/TID-28.case4.h>

#include <sge/tests/TID-42.case1.h>

#include <sge/tests/TID-43.case1.h>
#include <sge/tests/TID-43.case2.h>
#include <sge/tests/TID-43.case3.h>
#include <sge/tests/TID-43.case4.h>

#include <sge/tests/TID-48.case1.h>
#include <sge/tests/TID-48.case2.h>
#include <sge/tests/TID-48.case3.h>
#include <sge/tests/TID-48.case4.h>
#include <sge/tests/TID-48.case5.h>

#include <sge/tests/transparent_queue.h>
#include <sge/tests/shader_cache.h>

#include <sge/tests/material_get_set.h>

SGE_API test_t* test_new(memory_allocator_t* allocator)
{
	test_t* test = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST, test_t);
	memzero(test, test_t);
	return test;
}

#define IF(NAME) if(strcmp(name, TEST_NAME(NAME)) == 0) NAME##_setup_callbacks(test)
#define ELSE_IF(NAME) else IF(NAME)

SGE_API test_t* test_create(memory_allocator_t* allocator, const char* name)
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
	ELSE_IF(MULTIPLE_LIGHTS_LOAD);
	ELSE_IF(MULTIPLE_SPOT_LIGHTS_LOAD);
	ELSE_IF(CUBE);
	ELSE_IF(TEXTURE_SAMPLING);
	ELSE_IF(TEXTURE_SAMPLING_ALPHA_CHANNEL);
	ELSE_IF(BITMAP_GLYPH_ATLAS_TEXTURE);
	ELSE_IF(TEXT_MESH);
	ELSE_IF(TEXT_MESH_LOAD);
	ELSE_IF(BITMAP_TEXT);
	ELSE_IF(BITMAP_TEXT_LOAD);
	ELSE_IF(SDF_ROUNDED_RECT);
	ELSE_IF(TID_14_CASE_1);
	ELSE_IF(TID_14_CASE_2);
	ELSE_IF(TID_14_CASE_3);
	ELSE_IF(TID_28_CASE_1);
	ELSE_IF(TID_28_CASE_2);
	ELSE_IF(TID_28_CASE_3);
	ELSE_IF(TID_28_CASE_4);
	ELSE_IF(TID_42_CASE_1);
	ELSE_IF(TID_43_CASE_1);
	ELSE_IF(TID_43_CASE_2);
	ELSE_IF(TID_43_CASE_3);
	ELSE_IF(TID_43_CASE_4);
	ELSE_IF(TID_48_CASE_1);
	ELSE_IF(TID_48_CASE_2);
	ELSE_IF(TID_48_CASE_3);
	ELSE_IF(TID_48_CASE_4);
	ELSE_IF(TID_48_CASE_5);
	ELSE_IF(TRANSPARENT_QUEUE);
	ELSE_IF(SHADER_CACHE);
	ELSE_IF(MATERIAL_GET_SET);
	else
	{
		printf 	(
					"supported tests: \n"
					"\tDEPTH_RENDER_TEXTURE\n"
					"\tDEPTH_RENDER_TEXTURE_LOAD\n"
					"\tENVIRONMENT_REFLECTIONS\n"
					"\tENVIRONMENT_REFLECTIONS_LOAD\n"
					"\tDEPTH_CUBE_RENDER_TEXTURE\n"
					"\tDEPTH_CUBE_RENDER_TEXTURE_LOAD\n"
					"\tPOINT_LIGHT_SHADOWS\n"
					"\tPOINT_LIGHT_SHADOWS_LOAD\n"
					"\tSPOT_LIGHT\n"
					"\tSPOT_LIGHT_LOAD\n"
					"\tMULTIPLE_LIGHTS_LOAD\n"
					"\tMULTIPLE_SPOT_LIGHTS_LOAD\n"
					"\tCUBE\n"
					"\tTEXTURE_SAMPLING\n"
					"\tTEXTURE_SAMPLING_ALPHA_CHANNEL\n"
					"\tTEXT_MESH\n"
					"\tTEXT_MESH_LOAD\n"
					"\tBITMAP_TEXT\n"
					"\tBITMAP_TEXT_LOAD\n"
					"\tSDF_ROUNDED_RECT\n"
					"\tTID_14_CASE_1\n"
					"\tTID_14_CASE_2\n"
					"\tTID_14_CASE_3\n"
					"\tTID_28_CASE_1\n"	
					"\tTID_28_CASE_2\n"	
					"\tTID_28_CASE_3\n"	
					"\tTID_28_CASE_4\n"	
					"\tTID_42_CASE_1\n"	
					"\tTID_43_CASE_1\n"	
					"\tTID_43_CASE_2\n"	
					"\tTID_43_CASE_3\n"	
					"\tTID_43_CASE_4\n"
					"\tTID_48_CASE_1\n"
					"\tTID_48_CASE_2\n"
					"\tTID_48_CASE_3\n"
					"\tTID_48_CASE_4\n"
					"\tTID_48_CASE_5\n"
					"\tTRANSPARENT_QUEUE\n"
					"\tSHADER_CACHE\n"
					"\tMATERIAL_GET_SET\n"
				);
		exit(0);
	}
	return test;
}

SGE_API void test_destroy(test_t* test)
{
	heap_free(test->user_data);
	memory_allocator_dealloc(test->allocator, test);
}
