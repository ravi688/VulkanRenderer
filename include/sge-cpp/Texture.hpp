#pragma once

#include <sge/texture.h> /* for texture_t* */
#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference

namespace SGE
{
	class SGE_API Texture : public PtrReference<texture_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Texture);

		friend class BitmapGlyphAtlasTexture;
	public:
		void destroy() noexcept
		{
			texture_destroy(m_handle);
			texture_release_resources(m_handle);
			m_handle = NULL;
		}
	};
}
