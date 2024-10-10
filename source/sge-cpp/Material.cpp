#include <sge-cpp/Material.hpp>

namespace SGE
{
	template<>
	void Material::set<float>(const std::string& name, float value)
	{
		material_set_float(m_handle, name.c_str(), value);
	}

	template<>
	void Material::set<vec2_t>(const std::string& name, vec2_t value)
	{
		material_set_vec2(m_handle, name.c_str(), value);
	}

	template<>
	void Material::set<vec4_t>(const std::string& name, vec4_t value)
	{
		material_set_vec4(m_handle, name.c_str(), value);
	}

	template<>
	void Material::set<s32>(const std::string& name, s32 value)
	{
		material_set_int(m_handle, name.c_str(), value);
	}

	template<>
	void Material::set<Texture>(const std::string& name, Texture texture)
	{
		material_set_texture(m_handle, name.c_str(), static_cast<texture_t*>(texture));
	}
}
