#pragma once

#include <sge/bitmap_glyph_atlas_texture.h> /* for bitmap_glyph_atlas_texture_t* */
#include <sge-cpp/Font.hpp> /* for SGE::Font */

#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference

namespace SGE
{
	/* forward declaration of SGE::Driver */
	class Driver;

	class SGE_API BitmapGlyphAtlasTexture : public PtrReference<bitmap_glyph_atlas_texture_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(BitmapGlyphAtlasTexture)
		
	public:
		struct CreateInfo
		{
			u32 width;
			u32 height;
			Font font;
		};

		BitmapGlyphAtlasTexture(Driver& driver, const CreateInfo& createInfo) noexcept;

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

		#ifdef GLOBAL_DEBUG
		void dump(const std::string& filePath) const noexcept
		{
			bitmap_glyph_atlas_texture_dump(m_handle, filePath.c_str());
		}
		#endif
	};
}
