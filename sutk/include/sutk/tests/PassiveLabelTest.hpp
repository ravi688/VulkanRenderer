#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;
	class PassiveLabel;
	class TextGroupContainer;

	class PassiveLabelTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		FullWindowContainer* m_rootContainer { };
		Container* m_auxContainer { };
		TextGroupContainer* m_txtGrpContainer1 { };
		TextGroupContainer* m_txtGrpContainer2 { };
		PassiveLabel* m_label { };
		PassiveLabel* m_label2 { };
		PassiveLabel* m_label3 { };
		UIDriver::FontReference m_font { };

	public:
		PassiveLabelTest() : m_uiDriver(NULL), m_gfxDriver(NULL), m_inputDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
