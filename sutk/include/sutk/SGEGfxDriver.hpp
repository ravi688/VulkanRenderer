#pragma once

#include <sutk/defines.hpp> /* for Vec2D, etc. */
#include <sutk/IGfxDriver.hpp> /* for IGfxDriver interface class */

#include <sge-cpp/sge.hpp>

#include <unordered_map> /* for std::unordered_map */

#include <common/id_generator.h> /* for id_generator */

namespace SUTK
{	
	struct SGEBitmapTextData
	{
		SGE::BitmapText text;
		SGE::RenderObject object;
		// holds total aggregate number of characters in 'text' (BitmapText), i.e. sum over all text strings's sizes created out of this 'text'
		u32 charCount;
	};
	struct SGEBitmapTextStringData
	{
		SGE::BitmapTextString textString;
		// handle to SGE::BitmapText out of which this text string has been created
		// this is used to hash into the 'm_bitmapTextMappings' unordered map and update the 'charCount'
		GfxDriverObjectHandleType textHandle;
	};
	class SGEGfxDriver : public IGfxDriver
	{
	private:
		SGE::Driver& m_driver;
		SGE::BitmapGlyphAtlasTexture m_bgaTexture;
		SGE::Font m_font;
		SGE::RenderScene m_scene;
		SGE::Shader m_shader;
		id_generator_t m_id_generator;
		GfxDriverObjectHandleType m_currentBitmapTextHandle;
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextData> m_bitmapTextMappings;
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData> m_bitmapTextStringMappings;
		struct OnResizeCallbackHandlerData
		{
			OnResizeCallbackHandler handler;
			SGE::Event::SubscriptionHandle handle;
		};
		std::unordered_map<id_generator_id_type_t, OnResizeCallbackHandlerData> m_onResizeHandlers;

		// We're caching current window size into this member variable to avoid calling internal function calls everytime getSize() is called
		// The value of this variable will be updated whenever window is resized.
		Vec2D<DisplaySizeType> m_size;

	private:
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextData>::iterator
		getBitmapTextIterator(GfxDriverObjectHandleType handle);
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData>::iterator
		getSubTextIterator(GfxDriverObjectHandleType handle);
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> getOrCreateBitmapText();
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> createBitmapText();
		void destroyBitmapText(GfxDriverObjectHandleType handle);
		SGE::BitmapTextString getText(GfxDriverObjectHandleType handle);
	public:

		// Constructors
		SGEGfxDriver(SGE::Driver& driver);
		// Destructor
		~SGEGfxDriver();


		// IGfxDriver INTERFACE IMPLEMENTATION

		virtual Vec2D<DisplaySizeType> getSize() override { return m_size; }
		virtual void render(UIDriver& driver) override;

		virtual GfxDriverObjectHandleType createText() override;
		virtual void destroyText(GfxDriverObjectHandleType handle) override;

		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec2D<DisplaySizeType> position) override;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) override;

		virtual u32 getBaselineHeightInPixels() override;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) override;
		virtual void removeOnResizeHandler(u32 id) override;
	};
}
