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