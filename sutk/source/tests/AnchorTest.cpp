#include <sutk/tests/AnchorTest.hpp>

#include <sutk/RenderRect.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderRectContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData AnchorTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Anchor Test";
		return data;
	}

	void AnchorTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_renderRectContainer = m_uiDriver->createContainer<RenderRectContainer>(rootContainer);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 7, 7 });
		AnchorRect* anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_renderRect = m_uiDriver->createRenderRect<RenderRectFill>(m_renderRectContainer);
		m_renderRect->setColor(Color3::green());
	}

	void AnchorTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void AnchorTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void AnchorTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
