#include <sge-cpp/Texture.hpp>

#include <sge-cpp/sge.hpp>

namespace SGE
{
	Texture::Texture(Driver& driver, texture_type_t type, const std::string& str) noexcept 
	{ 
		m_handle = texture_load(driver.getHandle(), type, str.c_str());
	}
}