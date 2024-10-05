#pragma once

#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;
	class ToggleButtonListView;
	class ToggleButtonGroupView;

	class ToggleButtonGroupTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		ToggleButtonListView* m_toggleListView;
		ToggleButtonGroupView* m_toggleGroupView;

	public:
		ToggleButtonGroupTest() : m_uiDriver(NULL), m_gfxDriver(NULL) { }

		DriverInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
