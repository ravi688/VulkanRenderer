#include <sge-cpp/Material.hpp>

namespace SGE
{
	template<>
	void Material::set<float>(const std::string_view name, float value) noexcept
	{
		material_set_float(m_handle, name.data(), value);
	}

	template<>
	void Material::set<vec2_t>(const std::string_view name, vec2_t value) noexcept
	{
		material_set_vec2(m_handle, name.data(), value);
	}

	template<>
	void Material::set<vec4_t>(const std::string_view name, vec4_t value) noexcept
	{
		material_set_vec4(m_handle, name.data(), value);
	}

	template<>
	void Material::set<s32>(const std::string_view name, s32 value) noexcept
	{
		material_set_int(m_handle, name.data(), value);
	}

	template<>
	void Material::set<u32>(const std::string_view name, u32 value) noexcept
	{
		material_set_uint(m_handle, name.data(), value);
	}

	template<>
	void Material::set<Texture>(const std::string_view name, Texture texture) noexcept
	{
		material_set_texture(m_handle, name.data(), static_cast<texture_t*>(texture));
	}

	template<>
	Texture Material::get<Texture>(const std::string_view name) noexcept
	{
		return Texture { material_get_texture(m_handle, name.data()) };
	}
}
