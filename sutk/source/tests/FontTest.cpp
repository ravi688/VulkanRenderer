#include <sutk/tests/FontTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>

namespace SUTK
{
	DriverInitializationData FontTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Font Test";
		return data;
	}

	void FontTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_label = m_uiDriver->createContainer<Label>(rootContainer);
		m_label->setRect({ 5, 5, 3, 1 });
		m_label->set("This is Font Test, Press any key to switch fonts");

		UIDriver::FontReference font1 = this->m_uiDriver->loadFont("../showcase/resource/fonts/Pushster-Regular.ttf");
		UIDriver::FontReference font2 = this->m_uiDriver->loadFont("../showcase/resource/fonts/arial.ttf");
		m_inputDriver->getOnKeyEvent().subscribe([this, font1, font2](IInputDriver*, KeyCode key, KeyEvent action, ModifierKeys modifiers)
		{
			if(action == KeyEvent::Press)
			{
				std::cout << "Key Pressed" << std::endl;
				this->m_label->getText().setFont(font1);
			}
			else
			{
				std::cout << "Key Released" << std::endl;
				this->m_label->getText().setFont(font2);
			}
		});
	}

	void FontTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void FontTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void FontTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
