#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sge-cpp/RenderScene.hpp>
#include <sge-cpp/Font.hpp>
#include <sge-cpp/BitmapGlyphAtlasTexture.hpp>
#include <sge-cpp/BitmapText.hpp>
#include <sge-cpp/RenderScene.hpp>
#include <sge-cpp/RenderObject.hpp>

namespace SUTK
{
	class BitmapTextTest : public ITest
	{
	private:
		SGE::RenderScene m_scene;
		SGE::Font m_font;
		SGE::BitmapGlyphAtlasTexture m_texture;
		SGE::BitmapText m_text;
		SGE::BitmapText m_text2;
		SGE::BitmapTextString m_textString;
		SGE::BitmapTextString m_anotherString;
		SGE::RenderObject m_textObject;
		SGE::RenderObject m_textObject2;

	public:
		BitmapTextTest() = default;

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
