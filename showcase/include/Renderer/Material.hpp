/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Material.hpp is a part of VulkanRenderer

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
#include <Renderer/IRenderObject.hpp>
#include <Math/Mat4.hpp>
#include <Math/Vec4.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>
#include <renderer/material.h>


namespace V3D
{
	class Renderer;
	class Shader;

	class SHOWCASE_API Material : IRenderObject
	{
		typedef material_field_handle_t FieldHandle;
	private:
		material_t* handle;

		friend Renderer;

		Material(Renderer& renderer, Shader& shader);

	public: 

		inline Material(material_t* material) : handle(material) { }

		// Implementation of IRenderObject interface
		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;


		void bind() const;

		template<typename T>
		void setPush(const char* name, const T& value) const;
		template<typename T>
		void setPush(const FieldHandle& handle, const T& value) const;

		template<typename T>
		T getPush(const char* name) const;
		template<typename T>
		T getPush(const FieldHandle& handle) const;
		
		template<typename T>
		void set(const  char* name, const T& value) const;
		template<typename T>
		void set(const FieldHandle& handle, const T& value) const;

		template<typename T>
		T get(const char* name) const;
		template<typename T>
		T get(const FieldHandle& handle) const;
	};
}