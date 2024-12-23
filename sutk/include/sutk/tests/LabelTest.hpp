#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;
	class Label;
	class TextGroupContainer;

	class LabelTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		FullWindowContainer* m_rootContainer { };
		Container* m_auxContainer { };
		TextGroupContainer* m_txtGrpContainer1 { };
		TextGroupContainer* m_txtGrpContainer2 { };
		Label* m_label { };
		Label* m_label2 { };
		Label* m_label3 { };
		UIDriver::FontReference m_font { };

	public:
		LabelTest() : m_uiDriver(NULL), m_gfxDriver(NULL), m_inputDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
