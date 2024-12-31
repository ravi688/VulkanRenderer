#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;

	class ScrollContainer;

	class FullWindowContainer;
	class Button;
	class ToggleButton;
	class ButtonListView;
	class ToggleButtonListView;
	class ScrollButtonList;
	class ScrollToggleButtonGroupView;
	class NotebookView;

	class DarkLightThemeTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		FullWindowContainer* m_fullWindowContainer;
		Button* m_button;
		ToggleButton* m_toggleButton;
		ButtonListView* m_buttonListView;
		ToggleButtonListView* m_toggleButtonListView;
		NotebookView* m_notebookView;
		ScrollContainer* m_scrollContainer1;
		ScrollContainer* m_scrollContainer2;
		ScrollButtonList* m_scrollButtonList;
		ScrollToggleButtonGroupView* m_scrollToggleButtonGroupView;

	public:
		DarkLightThemeTest() : m_uiDriver(NULL), m_gfxDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
