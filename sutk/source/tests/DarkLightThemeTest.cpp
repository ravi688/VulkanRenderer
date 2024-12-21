#include <sutk/tests/DarkLightThemeTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/BuiltinThemeManager.hpp>
#include <sutk/Label.hpp>

#include <sutk/Button.hpp>
#include <sutk/ToggleButton.hpp>
#include <sutk/ButtonListView.hpp>
#include <sutk/ToggleButtonListView.hpp>
#include <sutk/NotebookView.hpp>

namespace SUTK
{
	ITest::TestInitializationData DarkLightThemeTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Dark Light Theme Test (press B to toggle themes)";
		return data;
	}

	void DarkLightThemeTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);

		// By default built-in theme manager applied light theme
		// And only two themes exist in built-in theme manager
		// 	1. Dark Theme (by default), the name of this theme is "sutk.dark"
		// 	2. Light Theme, the name of this theme is "sutk.light"
		BuiltinThemeManager& themeManager = m_uiDriver->getBuiltinThemeManager();

		FullWindowContainer* rootContainer = m_fullWindowContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());

		f32 pos = 0.1f;
		// Button
		Button* button = m_button = m_uiDriver->createContainer<Button>(rootContainer);
		pos += button->getSize().width + 0.1f;
		// Toggle Button
		ToggleButton* togButton = m_toggleButton = m_uiDriver->createContainer<ToggleButton>(rootContainer);
		togButton->setPosition({ pos, 0.0f });
		pos += togButton->getSize().width + 0.1f;
		// Button List View
		ButtonListView* buttonList = m_buttonListView = m_uiDriver->createContainer<ButtonListView>(rootContainer);
		buttonList->setPosition({ pos, 0.0f });
		for(u32 i = 0; i < 4; ++i)
			buttonList->addButton(std::format("Button {}", i));
		pos += buttonList->getSize().width + 0.1f;
		// Toggle Button List View
		ToggleButtonListView* togButtonList = m_toggleButtonListView = m_uiDriver->createContainer<ToggleButtonListView>(rootContainer);
		togButtonList->setPosition({ pos, 0.0f });
		for(u32 i = 0; i < 4; ++i)
			togButtonList->addButton(std::format("Button {}", i));
		pos += togButtonList->getSize().width + 0.1f;
		pos = 0.1f;
		f32 posY = 5.5f;
		// Notebook
		NotebookView* notebook = m_notebookView = m_uiDriver->createContainer<NotebookView>(rootContainer);
		notebook->setRect({ { pos, posY }, { 10, 5 } });
		for(u32 i = 0; i < 3; ++i)
			notebook->createPage(std::format("Tab {}", i));
		m_inputDriver->getOnKeyEvent().subscribe([notebook](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			static auto i = 3;
			if((keycode == KeyCode::N) && (event == KeyEvent::Press))
			{
				notebook->createPage(std::format("Tab {}", i));
				i++;
			}
		});
		m_inputDriver->getOnKeyEvent().subscribe([notebook](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			static auto i = 3;
			if((keycode == KeyCode::R) && (event == KeyEvent::Press))
			{
				notebook->removePage(notebook->getCurrentPage());
				i++;
			}
		});

		m_inputDriver->getOnKeyEvent().subscribe([themeManagerPtr = &themeManager](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			static bool isSwap = true;
			if((keycode == KeyCode::B) && (event == KeyEvent::Press))
			{
				if(isSwap)
					themeManagerPtr->applyTheme("sutk.dark");
				else
					themeManagerPtr->applyTheme("sutk.light");
				isSwap = !isSwap;
			}
		});
	}

	void DarkLightThemeTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyContainer<Button>(m_button);
		m_uiDriver->destroyContainer<ToggleButton>(m_toggleButton);
		m_uiDriver->destroyContainer<ButtonListView>(m_buttonListView);
		m_uiDriver->destroyContainer<ToggleButtonListView>(m_toggleButtonListView);
		m_uiDriver->destroyContainer<NotebookView>(m_notebookView);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_fullWindowContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void DarkLightThemeTest::render(SGE::Driver& driver)
	{
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void DarkLightThemeTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
