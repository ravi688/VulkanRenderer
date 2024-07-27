#include <sge-cpp/Material.hpp>

namespace SGE
{
	template<>
	void Material::set<float>(const std::string& name, float value)
	{
		material_set_float(m_handle, name.c_str(), value);
	}

	template<>
	void Material::set<vec4_t>(const std::string& name, vec4_t value)
	{
		material_set_vec4(m_handle, name.c_str(), value);
	}
}
