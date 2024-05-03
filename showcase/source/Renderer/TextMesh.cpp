/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TextMesh.cpp is a part of VulkanRenderer

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


#include <Renderer/TextMesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/GlyphMeshPool.hpp>

#include <iostream>

namespace V3D
{
	SHOWCASE_API TextMesh::TextMesh(Renderer& renderer, GlyphMeshPool& pool)
	{
		handle = text_mesh_create(renderer.handle, pool.handle);
	}

	SHOWCASE_API void TextMesh::destroy() const
	{
		text_mesh_destroy(handle);
	}

	SHOWCASE_API void TextMesh::releaseResources() const
	{
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API void TextMesh::drop() const
	{
		text_mesh_destroy(handle);
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API TextMesh::StringHandle TextMesh::createString() const
	{
		return text_mesh_string_create(handle);
	}

	SHOWCASE_API void TextMesh::destroyString(StringHandle shandle) const
	{
		text_mesh_string_destroyH(handle, shandle);
	}

	SHOWCASE_API void TextMesh::draw() const
	{
		text_mesh_draw(handle);
	}

	SHOWCASE_API void TextMesh::setString(StringHandle shandle, const char* ptr) const
	{
		text_mesh_string_setH(handle, shandle, ptr);
	}

	SHOWCASE_API void TextMesh::setStringPosition(StringHandle shandle, const Math::Vec3& position) const
	{
		text_mesh_string_set_positionH(handle, shandle, const_cast<Math::Vec3&>(position));
	}

	SHOWCASE_API void TextMesh::setStringRotation(StringHandle shandle, const Math::Vec3& euler) const
	{
		text_mesh_string_set_rotationH(handle, shandle, const_cast<Math::Vec3&>(euler));
	}

	SHOWCASE_API void TextMesh::setStringScale(StringHandle shandle, const Math::Vec3& scaleVector) const
	{
		text_mesh_string_set_scaleH(handle, shandle, const_cast<Math::Vec3&>(scaleVector));
	}
}