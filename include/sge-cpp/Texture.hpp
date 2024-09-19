#pragma once

#include <sge/texture.h> /* for texture_t* */
#include <common/PtrReference.hpp> // for com::PtrReference

#include <string> // for std::string

namespace SGE
{
	class Driver;
	class SGE_API Texture : public com::PtrReference<texture_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Texture);

		friend class BitmapGlyphAtlasTexture;
	public:
		Texture(Driver& driver, texture_type_t type, const std::string& str) noexcept;
		void destroy() noexcept
		{
			texture_destroy(m_handle);
			texture_release_resources(m_handle);
			m_handle = NULL;
		}
	};
}
