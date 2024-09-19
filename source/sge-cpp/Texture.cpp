#include <sge-cpp/Texture.hpp>

#include <sge-cpp/sge.hpp>

namespace SGE
{
	Texture::Texture(Driver& driver, texture_type_t type, const std::string& str) noexcept 
	{ 
		m_handle = texture_load(driver.getHandle(), type, str.c_str());
	}
	Texture::Texture(Driver& driver, texture_type_t type, const std::span<const u8> data) noexcept
	{
		m_handle = texture_load_mem(driver.getHandle(), type, com_immutable_data_t { data.data(), static_cast<u32>(data.size()) });
	}
}