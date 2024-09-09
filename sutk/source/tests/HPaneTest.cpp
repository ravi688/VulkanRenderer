#include <sutk/tests/HPaneTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/HPaneContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData HPaneTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Horizontal Pane Container Test";
		return data;
	}

	void HPaneTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_hPaneContainer = m_uiDriver->createContainer<HPaneContainer>(rootContainer);
		m_hPaneContainer->alwaysFitInParent();
		m_hPaneContainer->enableDebug();
	}

	void HPaneTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void HPaneTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void HPaneTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
