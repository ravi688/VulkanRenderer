#pragma once

#include <sge/material_library.h>
#include <sge-cpp/Material.hpp> /* for Material */
#include <sge-cpp/Shader.hpp> /* for Shader */
#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference
#include <string> /* for std::string */

namespace SGE
{
	class SGE_API MaterialLibrary : public PtrReference<material_library_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(MaterialLibrary);

		friend class Driver;

	public:
		Material createMaterial(Shader shader, const std::string& name) const noexcept
		{
			return Material(material_library_getH(m_handle, material_library_create_material(m_handle, shader.getHandle(), name.c_str())));
		}
	};
}
