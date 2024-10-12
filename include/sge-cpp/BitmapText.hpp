#pragma once

#include <sge/bitmap_text.h> /* for bitmap_text_t* */
#include <sge-cpp/BitmapGlyphAtlasTexture.hpp> /* for SGE::BitmapGlyphAtlasTexture */

#include <string> /* for std::string */

#include <hpml/affine_transformation.h> /* for mat4_translation */

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
		void setFont(Font font) noexcept
		{
			bitmap_text_string_set_fontH(m_bitmapTextHandle, m_handle, static_cast<font_t*>(font));
		}
		Font getFont() noexcept
		{
			return Font(bitmap_text_string_get_fontH(m_bitmapTextHandle, m_handle));
		}
		u32 getLength() const noexcept
		{
			return bitmap_text_string_get_lengthH(m_bitmapTextHandle, m_handle);
		}
		f32 getZCoordFromGlyphIndex(u32 glyphIndex) const noexcept
		{
			return bitmap_text_string_get_zcoord_from_glyph_index(m_bitmapTextHandle, m_handle, glyphIndex);
		}
		u32 getGlyphIndexFromZCoord(f32 zCoord) const noexcept
		{
			return bitmap_text_string_get_glyph_index_from_zcoord(m_bitmapTextHandle, m_handle, zCoord);
		}
		void setTransform(mat4_t transform) const noexcept
		{
			bitmap_text_string_set_transformH(m_bitmapTextHandle, m_handle, transform);
		}
		mat4_t getTransform() const noexcept
		{
			return bitmap_text_string_get_transformH(m_bitmapTextHandle, m_handle);
		}
		void setActive(bool isActive) noexcept
		{
			bitmap_text_string_set_activeH(m_bitmapTextHandle, m_handle, isActive);
		}
		bool isActive() const noexcept
		{
			return bitmap_text_string_is_activeH(m_bitmapTextHandle, m_handle);
		}
		void setColor(color_t color) noexcept
		{
			bitmap_text_string_set_color(m_bitmapTextHandle, m_handle, color);
		}
		void setCharAttrColor(const char_attr_color_range_t* ranges, u32 rangeCount) noexcept
		{
			bitmap_text_string_set_char_attr_color(m_bitmapTextHandle, m_handle, ranges, rangeCount);
		}
		void setPosition(vec3_t position) const noexcept
		{
			setTransform(mat4_translation(position.x, position.y, position.z));
		}
		vec3_t getPosition() const noexcept
		{
			mat4_t m = getTransform();
			return vec3(m.m03, m.m13, m.m23);
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

		BitmapText(Driver& driver, BitmapGlyphAtlasTexture texture, Font font = { }) noexcept;

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