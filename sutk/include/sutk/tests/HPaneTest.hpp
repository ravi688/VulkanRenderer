#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;

	class HPaneContainer;

	class HPaneTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IInputDriver* m_inputDriver;
		IGfxDriver* m_gfxDriver;
		HPaneContainer* m_hPaneContainer;

	public:
		HPaneTest() : m_uiDriver(NULL), m_inputDriver(NULL), m_gfxDriver(NULL) { }

		DriverInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
