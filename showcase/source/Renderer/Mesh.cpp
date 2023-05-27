/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Mesh.cpp is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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


#include <Renderer/Mesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Mesh3D.hpp>

namespace V3D
{
	SHOWCASE_API Mesh::Mesh(Renderer& renderer, Mesh3D& mesh_data)
	{
		handle = mesh_create(renderer.handle, mesh_data.handle);
	}

	SHOWCASE_API void Mesh::destroy() const
	{
		mesh_destroy(handle);
	}

	SHOWCASE_API void Mesh::releaseResources() const
	{
		mesh_release_resources(handle);
	}

	SHOWCASE_API void Mesh::drop() const
	{
		mesh_destroy(handle);
		mesh_release_resources(handle);
	}

	SHOWCASE_API void Mesh::drawIndexed() const
	{
		mesh_draw_indexed(handle);
	}

	SHOWCASE_API void Mesh::drawInstanced(u32 instanceCount) const
	{
		mesh_draw_instanced(handle, instanceCount);
	}

	SHOWCASE_API void Mesh::drawNonIndexed() const
	{
		mesh_draw(handle);
	}

	SHOWCASE_API void Mesh::drawIndexedInstanced(u32 instanceCount) const
	{
		mesh_draw_indexed_instanced(handle, instanceCount);
	}
}