/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: grammar.h is a part of VulkanRenderer

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

static const char* keywords[] = 
{
	"",
	"true",
	"false",
	"Shader",
	"Properties",
	"Layout",
	"RenderPass",
	"SubPass",
	"GLSL",
	"GraphicsPipeline",
	"vec4",
	"vec3",
	"vec2"
};

/* keyword enumerations */
enum
{
	KEYWORD_UNDEFINED = 0UL,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_SHADER,
	KEYWORD_PROPERTIES,
	KEYWORD_LAYOUT,
	KEYWORD_RENDERPASS,
	KEYWORD_SUBPASS,
	KEYWORD_GLSL,
	KEYWORD_GFXPIPELINE,
	KEYWORD_VEC4,
	KEYWORD_VEC3,
	KEYWORD_VEC2,
	KEYWORD_MAX
};

/* note: the name of the blocks are also treated as qualifiers */
enum
{
	QUALIFIER_SHADER = KEYWORD_SHADER,
	QUALIFIER_PROPERTIES = KEYWORD_PROPERTIES,
	QUALIFIER_LAYOUT = KEYWORD_LAYOUT,
	QUALIFIER_RENDERPASS = KEYWORD_RENDERPASS,
	QUALIFIER_SUBPASS = KEYWORD_SUBPASS,
	QUALIFIER_GLSL = KEYWORD_GLSL,
	QUALIFIER_GFXPIPELINE = KEYWORD_GFXPIPELINE,
	QUALIFIER_VEC4 = KEYWORD_VEC4,
	QUALIFIER_VEC3 = KEYWORD_VEC3,
	QUALIFIER_VEC2 = KEYWORD_VEC2
};

/* attributes */
CAN_BE_UNUSED_VARIABLE static const char* attribute_keywords[] = 
{
	"Name",
	"NoParse",
	"Read",
	"Write",
	"Attribute",
	"Rate",
	"MeshLayout"
};

/* attribute enumerations */
enum
{
	ATTRIBUTE_NAME,
	ATTRIBUTE_NOPARSE,
	ATTRIBUTE_READ,
	ATTRIBUTE_WRITE,
	ATTRIBUTE_ATTRIBUTE, /* vertex attribute */
	ATTRIBUTE_RATE,
	ATTRIBUTE_MESH_LAYOUT,
	ATTRIBUTE_MAX
};
