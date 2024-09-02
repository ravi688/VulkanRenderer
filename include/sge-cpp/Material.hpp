#pragma once

#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference<>
#include <sge/material.h> /* for material_t* */

#include <string> /* for std::string */

namespace SGE
{
	class Material : public PtrReference<material_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Material)

		friend class MaterialLibrary;
		friend class RenderObject;

	public:
		template<typename T>
		void set(const std::string& name, T value)
		{
			debug_log_error("Setter for this type is not defined");
		}
	};

	template<>
	void Material::set<float>(const std::string& name, float value);

	template<>
	void Material::set<vec2_t>(const std::string& name, vec2_t value);

	template<>
	void Material::set<vec4_t>(const std::string& name, vec4_t value);

	template<>
	void Material::set<s32>(const std::string& name, s32 value);
}
