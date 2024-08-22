#include <sutk/tests/ButtonTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/Button.hpp>

namespace SUTK
{
	DriverInitializationData ButtonTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Button Test";
		return data;
	}

	void ButtonTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_button = m_uiDriver->createContainer<Button>(rootContainer);
		m_button->setRect({ 5, 5, 3, 1 });
		auto& visualRect = m_button->getGraphicAs<DefaultButtonGraphic>()->getVisualButtonRect();
		auto& label = m_button->getGraphicAs<DefaultButtonGraphic>()->getLabel();
		visualRect.setIdleColor(Color4::red());
		visualRect.setHoverColor(Color4::red(0.6f));
		visualRect.setPressColor(Color4::red(0.3f));
		visualRect.setTransitionDelay(0.2f);
		label.set("Close");

		m_button = m_uiDriver->createContainer<Button>(rootContainer);
		m_button->setRect({ 5, 6.1f, 3, 1 });
		auto& visualRect2 = m_button->getGraphicAs<DefaultButtonGraphic>()->getVisualButtonRect();
		auto& label2 = m_button->getGraphicAs<DefaultButtonGraphic>()->getLabel();
		visualRect2.setIdleColor(Color4::yellow());
		visualRect2.setHoverColor(Color4::yellow(0.6f));
		visualRect2.setPressColor(Color4::yellow(0.3f));
		visualRect2.setTransitionDelay(0.2f);
		label2.set("OK");
	}

	void ButtonTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void ButtonTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ButtonTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
