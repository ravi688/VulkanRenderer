#include <sutk/tests/DarkLightThemeTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/BuiltinThemeManager.hpp>
#include <sutk/Label.hpp>

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

		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());

		// Create a Button
		// Create a Toggle Button
		// Create a Button List
		// Create a Toggle Button List
		// Create a Notebook
		// Create a Label
		// Create a Panel
		// Create a RenderRect
		// Create a RenderRoundFill

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
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void DarkLightThemeTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void DarkLightThemeTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
