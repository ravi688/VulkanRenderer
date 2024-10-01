#pragma once

#include <sge/texture.h> /* for texture_t* */
#include <common/PtrReference.hpp> // for com::PtrReference

#include <string_view> // for std::string_view
#include <span> // for std::span

namespace SGE
{
	typedef texture_attributes_t TextureAttributes;
	class Driver;
	class SGE_API Texture : public com::PtrReference<texture_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Texture);

		friend class BitmapGlyphAtlasTexture;
	public:
		Texture(Driver& driver, texture_type_t type, std::string_view str) noexcept;
		Texture(Driver& driver, texture_type_t type, const std::span<const u8> data) noexcept;
		void getAttributes(texture_attributes_t& attributes) noexcept;
		void destroy() noexcept
		{
			texture_destroy(m_handle);
			texture_release_resources(m_handle);
			m_handle = NULL;
		}
	};
}
