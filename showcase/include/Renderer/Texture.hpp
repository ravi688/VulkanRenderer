/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Texture.hpp is a part of VulkanRenderer

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


#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>
#include <renderer/texture.h>

#include <stdarg.h>		// va_list

namespace V3D
{
	enum class TextureType
	{
		NORMAL = TEXTURE_TYPE_NORMAL,
		ALBEDO = TEXTURE_TYPE_ALBEDO,
		CUBE_COMBINED = TEXTURE_TYPE_CUBE_COMBINED,
		CUBE_SEPARATED = TEXTURE_TYPE_CUBE_SEPARATED,
		CUBE = TEXTURE_TYPE_CUBE
	};

	class Renderer;

	class SHOWCASE_API Texture : IRenderObject
	{
	private:
		texture_t* handle;

		friend Renderer;
		friend class Material;

		// variable number of file paths (const char*)
		Texture(Renderer& renderer, TextureType type, va_list filePaths);
		Texture(Renderer& renderer, TextureType type, ...);
		//Texture(Renderer* renderer, /*Create info*/)

	public:

		inline Texture(texture_t* texture) : handle(texture) { }

		// Implementation of IRenderObject
		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;
	};
}