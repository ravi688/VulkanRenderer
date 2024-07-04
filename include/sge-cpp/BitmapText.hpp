#pragma once

#include <sge/bitmap_text.h> /* for bitmap_text_t* */
#include <sge-cpp/BitmapGlyphAtlasTexture.hpp> /* for SGE::BitmapGlyphAtlasTexture */

#include <string> /* for std::string */

namespace SGE
{
	class SGE_API BitmapTextString
	{
	private:
		bitmap_text_t* m_bitmapTextHandle;
		bitmap_text_string_handle_t m_handle;
		BitmapTextString(bitmap_text_t* bitmapText, bitmap_text_string_handle_t handle) : m_bitmapTextHandle(bitmapText),  m_handle(handle) { }
		friend class BitmapText;
	public:
		BitmapTextString() : m_bitmapTextHandle(NULL), m_handle(0) { }
		bitmap_text_string_handle_t getHandle() const noexcept { return m_handle; }
		void set(const std::string& str) const noexcept
		{
			bitmap_text_string_setH(m_bitmapTextHandle, m_handle, str.c_str());
		}
		void setPointSize(u32 pointSize) const noexcept
		{
			bitmap_text_string_set_point_sizeH(m_bitmapTextHandle, m_handle, pointSize);
		}
		u32 getPointSize() const noexcept
		{
			return bitmap_text_string_get_point_sizeH(m_bitmapTextHandle, m_handle);
		}
		void setTransform(mat4_t transform) const noexcept
		{
			bitmap_text_string_set_transformH(m_bitmapTextHandle, m_handle, transform);
		}

		void destroy() const noexcept
		{
			bitmap_text_string_destroyH(m_bitmapTextHandle, m_handle);
		}
	};

	/* forward declaration of SGE::Driver */
	class Driver;

	class SGE_API BitmapText
	{
	private:
		bitmap_text_t* m_handle;
	public:

		BitmapText() : m_handle(NULL) { }

		enum class RenderSpaceType
		{
			Plain,
			Volume
		};

		enum class RenderSurfaceType
		{
			Camera,
			Screen
		};

		static bitmap_text_render_space_type_t to_bitmap_text_render_space_type(RenderSpaceType type);

		static bitmap_text_render_surface_type_t to_bitmap_text_render_surface_type(RenderSurfaceType type);

		BitmapText(Driver& driver, BitmapGlyphAtlasTexture texture) noexcept;

		bitmap_text_t* getHandle() const noexcept { return m_handle; }
		
		void destroy() noexcept
		{
			bitmap_text_destroy(m_handle);
			bitmap_text_release_resources(m_handle);
			m_handle = NULL;
		}

		BitmapTextString createString() const noexcept
		{
			auto handle = bitmap_text_string_create(m_handle);
			return BitmapTextString(m_handle, handle);
		}

		void destroyString(BitmapTextString& str) const noexcept
		{
			bitmap_text_string_destroyH(m_handle, str.getHandle());
		}

		void setRenderSpaceType(RenderSpaceType spaceType) const noexcept
		{
			bitmap_text_set_render_space_type(m_handle, to_bitmap_text_render_space_type(spaceType));
		}

		void setRenderSurfaceType(RenderSurfaceType surfaceType) const noexcept
		{
			bitmap_text_set_render_surface_type(m_handle, to_bitmap_text_render_surface_type(surfaceType));
		}
	};
}