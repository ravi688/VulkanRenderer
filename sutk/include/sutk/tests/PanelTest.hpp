#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;

	class Panel;

	class PanelTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		FullWindowContainer* m_fullWindowContainer { };
		RenderableContainer* m_renderRectContainer { };
		RenderRectFill* m_renderRect { };
		Panel* m_panel { };

	public:
		PanelTest() : m_uiDriver(NULL), m_gfxDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
