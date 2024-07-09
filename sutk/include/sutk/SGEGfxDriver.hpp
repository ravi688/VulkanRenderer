#pragma once

#include <sutk/defines.hpp> /* for Vec2D, etc. */
#include <sutk/IGfxDriver.hpp> /* for IGfxDriver interface class */

#include <sge-cpp/sge.hpp>

#include <unordered_map> /* for std::unordered_map */

#include <common/id_generator.h> /* for id_generator */

namespace SUTK
{	
	class SGEGfxDriver : public IGfxDriver
	{
	private:
		SGE::Driver& m_driver;
		SGE::BitmapGlyphAtlasTexture m_bgaTexture;
		SGE::Font m_font;
		SGE::RenderScene m_scene;
		SGE::Material m_material;
		id_generator_t m_id_generator;
		std::unordered_map<GfxDriverObjectHandleType, std::pair<SGE::BitmapText, SGE::RenderObject>> m_bitmapTextMappings;
		std::unordered_map<GfxDriverObjectHandleType, SGE::BitmapTextString> m_bitmapTextStringMappings;
	public:
		SGEGfxDriver(SGE::Driver& driver);
		~SGEGfxDriver();
		virtual Vec2D<DisplaySizeType> getSize() override;
		virtual void render(UIDriver& driver) override;

		virtual GfxDriverObjectHandleType createText() override;

		std::unordered_map<GfxDriverObjectHandleType, std::pair<SGE::BitmapText, SGE::RenderObject>>::iterator
		getBitmapTextIterator(GfxDriverObjectHandleType handle);

		virtual void destroyText(GfxDriverObjectHandleType handle) override;

		virtual GfxDriverObjectHandleType createSubtext(GfxDriverObjectHandleType textHandle) override;

		std::unordered_map<GfxDriverObjectHandleType, SGE::BitmapTextString>::iterator
		getSubTextIterator(GfxDriverObjectHandleType handle);

		virtual void destroySubText(GfxDriverObjectHandleType subTextHandle) override;
	};
}
