#pragma once

#include <sge/bitmap_glyph_atlas_texture.h> /* for bitmap_glyph_atlas_texture_t* */
#include <sge-cpp/Font.hpp> /* for SGE::Font */

namespace SGE
{
	/* forward declaration of SGE::Driver */
	class Driver;

	class SGE_API BitmapGlyphAtlasTexture
	{
	private:
		bitmap_glyph_atlas_texture_t* m_handle;
		BitmapGlyphAtlasTexture(bitmap_glyph_atlas_texture_t* handle) noexcept : m_handle(handle) { }
	public:
		BitmapGlyphAtlasTexture() : m_handle(NULL) { }
		struct CreateInfo
		{
			u32 width;
			u32 height;
			Font font;
		};

		BitmapGlyphAtlasTexture(Driver& driver, const CreateInfo& createInfo) noexcept;
		bitmap_glyph_atlas_texture_t* getHandle() const noexcept { return m_handle; }

		void destroy() noexcept 
		{
			bitmap_glyph_atlas_texture_destroy(m_handle);
			bitmap_glyph_atlas_texture_release_resources(m_handle);
			m_handle = NULL;
		}

		void commit(bool* const isResized) const noexcept
		{
			bitmap_glyph_atlas_texture_commit(m_handle, isResized);
		}
	};
}
