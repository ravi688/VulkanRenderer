/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Material.cpp is a part of VulkanRenderer

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


#include <Renderer/Material.hpp> // Material
#include <Renderer/Shader.hpp>	 // Shader
#include <Renderer/Texture.hpp>	 // Texture

#include <Renderer/Renderer.hpp> // Renderer

namespace V3D
{
	SHOWCASE_API Material::Material(Renderer& renderer, Shader& shader)
	{
		material_create_info_t create_info { };
		create_info.is_vertex_attrib_from_file = true;
		create_info.shader = shader.handle;
		handle = material_create(renderer.handle, &create_info);
	}

	SHOWCASE_API void Material::destroy() const
	{
		material_destroy(handle);
	}

	SHOWCASE_API void Material::releaseResources() const
	{
		material_release_resources(handle);
	}

	SHOWCASE_API void Material::drop() const
	{
		material_destroy(handle);
		material_release_resources(handle);
	}

	SHOWCASE_API void Material::bind() const
	{
		material_bind(handle);
	}


	template<>
	SHOWCASE_API void Material::setPush<Math::Mat4>(const char* name, const Math::Mat4& value) const
	{
		material_set_push_mat4(handle, name, const_cast<Math::Mat4&>(value));
	}
	template SHOWCASE_API void Material::setPush<Math::Mat4>(const char* name, const Math::Mat4& value) const;


	template<>
	SHOWCASE_API void Material::setPush<Math::Vec4>(const char* name, const Math::Vec4& value) const
	{
		material_set_push_vec4(handle, name, const_cast<Math::Vec4&>(value));
	}
	template SHOWCASE_API void Material::setPush<Math::Vec4>(const char* name, const Math::Vec4& value) const;


	template<>
	SHOWCASE_API void Material::setPush<Math::Vec3>(const char* name, const Math::Vec3& value) const
	{
		material_set_push_vec3(handle, name, const_cast<Math::Vec3&>(value));
	}
	template SHOWCASE_API void Material::setPush<Math::Vec3>(const char* name, const Math::Vec3& value) const;


	template<>
	SHOWCASE_API void Material::setPush<Math::Vec2>(const char* name, const Math::Vec2& value) const
	{
		material_set_push_vec2(handle, name, const_cast<Math::Vec2&>(value));
	}
	template SHOWCASE_API void Material::setPush<Math::Vec2>(const char* name, const Math::Vec2& value) const;


	template<>
	SHOWCASE_API void Material::setPush<float>(const char* name, const float& value) const
	{
		material_set_push_float(handle, name, value);
	}
	template SHOWCASE_API void Material::setPush<float>(const char* name, const float& value) const;


	template<>
	SHOWCASE_API void Material::set<Math::Mat4>(const char* name, const Math::Mat4& value) const
	{
		material_set_mat4(handle, name, const_cast<Math::Mat4&>(value));
	}
	template SHOWCASE_API void Material::set<Math::Mat4>(const char* name, const Math::Mat4& value) const;


	template<>
	SHOWCASE_API void Material::set<Math::Vec4>(const char* name, const Math::Vec4& value) const
	{
		material_set_vec4(handle, name, const_cast<Math::Vec4&>(value));
	}
	template SHOWCASE_API void Material::set<Math::Vec4>(const char* name, const Math::Vec4& value) const;


	template<>
	SHOWCASE_API void Material::set<Math::Vec3>(const char* name, const Math::Vec3& value) const
	{
		material_set_vec3(handle, name, const_cast<Math::Vec3&>(value));
	}
	template SHOWCASE_API void Material::set<Math::Vec3>(const char* name, const Math::Vec3& value) const;

	template<>
	SHOWCASE_API void Material::set<Texture>(const char* name, const Texture& value) const
	{
		material_set_texture(handle, name, value.handle);
	}

	template<>
	SHOWCASE_API void Material::set<float>(const char* name, const float& value) const
	{
		material_set_float(handle, name, value);
	}
	template SHOWCASE_API void Material::set<float>(const char* name, const float& value) const;
}