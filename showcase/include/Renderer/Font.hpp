/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Font.hpp is a part of VulkanRenderer

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

#include <Showcase/Defines.hpp>
#include <Renderer/IResourceObject.hpp>
#include <renderer/font.h>			// C header, font_t
#include <string>

namespace V3D
{
	class SHOWCASE_API Font : IResourceObject
	{
	private:
		font_t* handle;

		friend class GlyphMeshPool;


	public:

		Font(const std::string& filePath);
		inline Font(font_t* font) : handle(font) { }

		// Implementation of IResourceObject interface
		void unload() const override;
		void drop() const override { unload(); }
	};
}
