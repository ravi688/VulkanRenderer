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

		u32 getBaselineSpace() const noexcept
		{
			return static_cast<u32>(font_get_baseline_space(m_handle));
		}

		s32 getAscender() const noexcept
		{
			return static_cast<s32>(font_get_ascender(m_handle));
		}

		s32 getDescender() const noexcept
		{
			return static_cast<s32>(font_get_descender(m_handle));
		}

		void setCharSize(u32 pointSize) const noexcept 
		{
			font_set_char_size(m_handle, pointSize);
		}

		u32 getCharSize() const noexcept
		{
			return font_get_char_size(m_handle);
		}

		void destroy() noexcept
		{
			font_destroy(m_handle);
			font_release_resources(m_handle);
			m_handle = NULL;
		}

		u32 getFontUnitsToPixels(u32 fontUnits, f32 dpi) { return font_get_font_units_to_pixels(m_handle, fontUnits, dpi); }
	};
};
