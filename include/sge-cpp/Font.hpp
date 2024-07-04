#pragma once

#include <sge/font.h> /* font_t* */

#include <string> /* for std::string */

namespace SGE
{
	/* forward declaration for SGE::Driver */
	class Driver;

	class SGE_API Font
	{
	private:
		font_t* m_handle;
		Font(font_t* handle) noexcept : m_handle(handle) { }

	public:
		Font() : m_handle(NULL) { }
		Font(Driver& driver, const std::string& filePath) noexcept;

		font_t* getHandle() const noexcept { return m_handle; }

		void setCharSize(u32 pointSize) const noexcept 
		{
			font_set_char_size(m_handle, pointSize);
		}

		void destroy() noexcept
		{
			font_destroy(m_handle);
			font_release_resources(m_handle);
			m_handle = NULL;
		}
	};
};
