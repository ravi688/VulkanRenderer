/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Texture.cpp is a part of VulkanRenderer

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


#include <Renderer/Texture.hpp>
#include <Renderer/Renderer.hpp>
#include <stdarg.h>

namespace V3D
{
	SHOWCASE_API Texture::Texture(Renderer& renderer, TextureType type, va_list filePaths)
	{
		handle = texture_loadv(renderer.handle, static_cast<texture_type_t>(type), filePaths);
	}

	SHOWCASE_API Texture::Texture(Renderer& renderer, TextureType type, ...)
	{
		va_list args;
		va_start(args, type);
		handle = texture_loadv(renderer.handle, static_cast<texture_type_t>(type), args);
		va_end(args);
	}

	SHOWCASE_API void Texture::destroy() const
	{
		texture_destroy(handle);
	}

	SHOWCASE_API void Texture::releaseResources() const
	{
		texture_release_resources(handle);
	}

	SHOWCASE_API void Texture::drop() const
	{
		texture_destroy(handle);
		texture_release_resources(handle);
	}
}