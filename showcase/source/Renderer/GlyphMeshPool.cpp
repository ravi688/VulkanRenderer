/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: GlyphMeshPool.cpp is a part of VulkanRenderer

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


#include <Renderer/GlyphMeshPool.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Font.hpp>
#include <renderer/glyph_mesh_pool.h> 

namespace V3D
{
	SHOWCASE_API GlyphMeshPool::GlyphMeshPool(Renderer& renderer, Font& font)
	{
		handle = glyph_mesh_pool_create(renderer.handle, font.handle);
	}

	SHOWCASE_API void GlyphMeshPool::destroy() const
	{
		glyph_mesh_pool_destroy(handle);
	}

	SHOWCASE_API void GlyphMeshPool::releaseResources() const
	{
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API void GlyphMeshPool::drop() const
	{
		glyph_mesh_pool_destroy(handle);
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API Mesh GlyphMeshPool::getMesh(u16 glyph) const
	{
		return glyph_mesh_pool_get_mesh(handle, glyph);
	}
}
