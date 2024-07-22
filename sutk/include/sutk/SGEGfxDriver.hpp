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
		bool m_autoCmdRecordAndExecute;
		SGE::BitmapGlyphAtlasTexture m_bgaTexture;
		SGE::Font m_font;
		SGE::RenderScene m_scene;
		SGE::Shader m_shader;
		id_generator_t m_id_generator;
		GfxDriverObjectHandleType m_currentBitmapTextHandle;
		std::unordered_map<id_generator_id_type_t, SGEBitmapTextData> m_bitmapTextMappings;
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData> m_bitmapTextStringMappings;
		std::unordered_map<id_generator_id_type_t, SGE::RenderObject> m_renderObjectMappings;
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
		std::unordered_map<id_generator_id_type_t, SGEBitmapTextData>::iterator
		getBitmapTextIterator(GfxDriverObjectHandleType handle);
		std::unordered_map<id_generator_id_type_t, SGE::RenderObject>::iterator
		getRenderObjectIterator(GfxDriverObjectHandleType handle);
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData>::iterator
		getSubTextIterator(GfxDriverObjectHandleType handle);
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> getOrCreateBitmapText();
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> createBitmapText();
		void destroyBitmapText(GfxDriverObjectHandleType handle);
		SGE::BitmapTextString getText(GfxDriverObjectHandleType handle);
	public:

		// Constructors
		SGEGfxDriver(SGE::Driver& driver, bool autoCmdRecordAndExecute = false);
		// Destructor
		~SGEGfxDriver();


		// IGfxDriver INTERFACE IMPLEMENTATION

		virtual Vec2D<DisplaySizeType> getSize() override { return m_size; }
		virtual void render(UIDriver& driver) override;

		virtual GfxDriverObjectHandleType createText() override;
		virtual void destroyText(GfxDriverObjectHandleType handle) override;

		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec2D<DisplaySizeType> position) override;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) override;

		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) override;
		virtual void setObjectScissor(GfxDriverObjectHandleType handle, const Rect2D<DisplaySizeType> rect) override;

		virtual u32 getBaselineHeightInPixels() override;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) override;
		virtual void removeOnResizeHandler(u32 id) override;
	};
}
