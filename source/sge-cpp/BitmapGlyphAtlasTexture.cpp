#include <sge-cpp/BitmapGlyphAtlasTexture.hpp>
#include <sge-cpp/sge.hpp> /* for SGE::Driver */

namespace SGE
{
	BitmapGlyphAtlasTexture::BitmapGlyphAtlasTexture (Driver& driver, const CreateInfo& createInfo) noexcept
	{
		bitmap_glyph_atlas_texture_create_info_t _createInfo = { };
		_createInfo.width = createInfo.width;
		_createInfo.height = createInfo.height;
		_createInfo.font = createInfo.font.getHandle();
		m_handle = bitmap_glyph_atlas_texture_create(driver.getHandle(), &_createInfo);
	}
}
