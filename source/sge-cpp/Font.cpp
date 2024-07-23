#include <sge-cpp/Font.hpp>
#include <sge-cpp/sge.hpp> /* for SGE::Driver::getHandle() */

namespace SGE
{
	Font::Font(Driver& driver, const std::string& filePath) noexcept
	{
		m_handle = font_load_and_create(driver.getHandle(), filePath.c_str());
	}

	Font::Font(Driver& driver, const void* bytes, u64 length) noexcept
	{
		m_handle = font_create(driver.getHandle(), const_cast<void*>(bytes), length);
	}
}
