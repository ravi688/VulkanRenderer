#pragma once

#include <common/PtrReference.hpp> // for com::PtrReference<>
#include <sge/material.h> /* for material_t* */
#include <sge-cpp/Texture.hpp> // for SGE::Texture reference type

#include <string_view> // std::string_view

namespace SGE
{
	class Material : public com::PtrReference<material_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Material)

		friend class MaterialLibrary;
		friend class RenderObject;

	public:
		template<typename T>
		void set(const std::string_view str, T value) noexcept
		{
			static_assert(false, "Please specialize set<> for this type");
		}
		template<typename T>
		T get(const std::string_view str) noexcept
		{
			static_assert(false, "Please specialize set<> for this type");
		}
	};


	// Specializations for Setters

	template<>
	void Material::set<float>(const std::string_view str, float value) noexcept;

	template<>
	void Material::set<vec2_t>(const std::string_view str, vec2_t value) noexcept;

	template<>
	void Material::set<vec4_t>(const std::string_view str, vec4_t value) noexcept;

	template<>
	void Material::set<s32>(const std::string_view str, s32 value) noexcept;

	template<>
	void Material::set<u32>(const std::string_view str, u32 value) noexcept;

	template<>
	void Material::set<Texture>(const std::string_view str, Texture texture) noexcept;

	// Specializations for Getters

	template<>
	Texture Material::get<Texture>(const std::string_view str) noexcept;
}
