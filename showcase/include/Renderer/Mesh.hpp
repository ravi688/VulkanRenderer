/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Mesh.hpp is a part of VulkanRenderer

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

#include <Showcase/Defines.hpp>		// SHOWCASE_API
#include <Renderer/IRenderObject.hpp>
#include <renderer/mesh.h>			// C header

namespace V3D
{
	class Renderer;
	class Mesh3D;
	
	class SHOWCASE_API Mesh : IRenderObject
	{
	private:
		mesh_t* handle;

		friend Renderer;

		Mesh(Renderer& renderer, Mesh3D& mesh_data);
	
	public:
		inline Mesh(mesh_t* mesh) : handle(mesh) { }

		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;
		
		inline void draw() const { drawIndexed(); }
		void drawIndexed() const;
		void drawIndexedInstanced(u32 instanceCount) const;
		void drawInstanced(u32 instanceCount) const;
		void drawNonIndexed() const;
	};
}
