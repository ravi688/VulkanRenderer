#pragma once

#include <sge/texture.h> /* for texture_t* */

namespace SGE
{
	class SGE_API Texture
	{
	private:
		texture_t* m_handle;
		Texture(texture_t* handle) : m_handle(handle) { }
		friend class BitmapGlyphAtlasTexture;
	public:
		texture_t* getHandle() const noexcept { return m_handle; }

		void destroy() noexcept
		{
			texture_destroy(m_handle);
			texture_release_resources(m_handle);
			m_handle = NULL;
		}
	};
}
