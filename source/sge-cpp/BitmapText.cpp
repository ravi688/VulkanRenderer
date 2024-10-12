#include <sge-cpp/BitmapText.hpp>
#include <sge-cpp/sge.hpp> /* for SGE::Driver */
#include <sge/debug.h> /* for debug_log_error */

namespace SGE
{
	BitmapText::BitmapText(Driver& driver, BitmapGlyphAtlasTexture texture, Font font) noexcept
	{
		m_handle = bitmap_text_create(driver.getHandle(), texture.getHandle(), static_cast<font_t*>(font));
	}

	bitmap_text_render_space_type_t BitmapText::to_bitmap_text_render_space_type(RenderSpaceType type)
	{
		switch(type)
		{
			case RenderSpaceType::Plain: return BITMAP_TEXT_RENDER_SPACE_TYPE_2D;
			case RenderSpaceType::Volume: return BITMAP_TEXT_RENDER_SPACE_TYPE_3D;
			default: debug_log_error("Unrecognized RenderSpaceType");
		}
		return BITMAP_TEXT_RENDER_SPACE_TYPE_2D;
	}

	bitmap_text_render_surface_type_t BitmapText::to_bitmap_text_render_surface_type(RenderSurfaceType type)
	{
		switch(type)
		{
			case RenderSurfaceType::Camera: return BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA;
			case RenderSurfaceType::Screen: return BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN;
			default: debug_log_error("Unrecognized RenderSurfaceType");
		}
		return BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN;
	}
}
