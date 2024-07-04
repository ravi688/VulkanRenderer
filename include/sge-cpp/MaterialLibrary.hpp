#pragma once

#include <sge/material_library.h>
#include <sge-cpp/Material.hpp> /* for Material */
#include <sge-cpp/Shader.hpp> /* for Shader */
#include <string> /* for std::string */

namespace SGE
{
	class SGE_API MaterialLibrary
	{
	private:
		material_library_t* m_handle;
		MaterialLibrary(material_library_t* handle) noexcept : m_handle(handle) { }

		friend class Driver;

	public:
		material_library_t* getHandle() const noexcept { return m_handle; }

		Material createMaterial(Shader shader, const std::string& name) const noexcept
		{
			return Material(material_library_getH(m_handle, material_library_create_material(m_handle, shader.getHandle(), name.c_str())));
		}
	};
}
