#pragma once

#include <sge/material.h> /* for material_t* */

#include <string> /* for std::string */

namespace SGE
{
	class SGE_API Material
	{
	private:
		material_t* m_handle;
		Material(material_t* handle) : m_handle(handle) { }

		friend class MaterialLibrary;
		friend class RenderObject;
		
	public:
		Material() : m_handle(NULL) { }
		material_t* getHandle() const noexcept { return m_handle; }

		template<typename T>
		void set(const std::string& name, T value)
		{
			debug_log_error("Setter for this type is not defined");
		}
	};

	template<>
	void Material::set<float>(const std::string& name, float value);

	template<>
	void Material::set<vec4_t>(const std::string& name, vec4_t value);
}
