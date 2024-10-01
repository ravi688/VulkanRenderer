#include <sge-cpp/Texture.hpp>

#include <sge-cpp/sge.hpp>

namespace SGE
{
	Texture::Texture(Driver& driver, texture_type_t type, std::string_view str) noexcept 
	{ 
		std::string _str(str);
		m_handle = texture_load(driver.getHandle(), type, _str.c_str());
	}
	Texture::Texture(Driver& driver, texture_type_t type, const std::span<const u8> data) noexcept
	{
		m_handle = texture_load_mem(driver.getHandle(), type, com_immutable_data_t { data.data(), static_cast<u32>(data.size()) });
	}
	void Texture::getAttributes(texture_attributes_t& attributes) noexcept
	{
		const texture_attributes_t* pAttributes = texture_get_attributes(m_handle);
		attributes.width = pAttributes->width;
		attributes.height = pAttributes->height;
		attributes.depth = pAttributes->depth;
		attributes.channel_count = pAttributes->channel_count;
	}
}