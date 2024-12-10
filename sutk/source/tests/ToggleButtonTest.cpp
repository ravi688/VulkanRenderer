#include <sutk/tests/ToggleButtonTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/ToggleButton.hpp>
#include <sutk/ToggleButtonGraphic.hpp>
#include <sutk/Label.hpp>

namespace SUTK
{
	ITest::TestInitializationData ToggleButtonTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Toggle Button Test";
		return data;
	}

	void ToggleButtonTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_button = m_uiDriver->createContainer<ToggleButton>(rootContainer);
		m_button->setRect({ 5, 5, 3, 1 });
		DefaultToggleButtonGraphic* graphic = m_button->getGraphicAs<DefaultToggleButtonGraphic>();
		auto& label = graphic->getLabel();
		graphic->setIdleColor(Color4::red());
		graphic->setHoverColor(Color4::red(0.6f));
		graphic->setPressColor(Color4::red(0.3f));
		graphic->setTransitionDelay(0.2f);
		label.set("Close");

		m_button = m_uiDriver->createContainer<ToggleButton>(rootContainer);
		m_button->setRect({ 5, 6.1f, 3, 1 });
		graphic = m_button->getGraphicAs<DefaultToggleButtonGraphic>();
		auto& label2 = graphic->getLabel();
		graphic->setIdleColor(Color4::yellow());
		graphic->setHoverColor(Color4::yellow(0.6f));
		graphic->setPressColor(Color4::yellow(0.3f));
		graphic->setTransitionDelay(0.2f);
		label2.set("OK");
	}

	void ToggleButtonTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void ToggleButtonTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ToggleButtonTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
