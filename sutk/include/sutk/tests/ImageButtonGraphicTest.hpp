#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;
	class IInputDriver;
	class Button;
	class ImageOverDefaultButtonGraphicProxy;
	class ImageButtonGraphic;

	class TestContainer;

	class ImageButtonGraphicTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		Button* m_button;
		ImageOverDefaultButtonGraphicProxy* m_buttonGraphic;
		Button* m_button2;
		ImageButtonGraphic* m_buttonGraphic2;

	public:
		ImageButtonGraphicTest() : m_uiDriver(NULL), m_gfxDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
