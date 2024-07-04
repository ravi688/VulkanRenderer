#include <sge-cpp/Material.hpp>

namespace SGE
{
	template<>
	void Material::set<float>(const std::string& name, float value)
	{
		material_set_float(m_handle, name.c_str(), value);
	}
}
