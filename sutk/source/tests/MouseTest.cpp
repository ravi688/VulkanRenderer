#include <sutk/tests/MouseTest.hpp>

#include <sutk/RenderRect.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData MouseTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Mouse Test";
		return data;
	}

	void MouseTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 1.0f, 1.0f });
		AnchorRect* anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 0, 0 });
		m_renderRect = m_uiDriver->createRenderRect<RenderRectFill>(m_renderRectContainer);
		m_renderRect->setColor(Color3::green());
	}

	void MouseTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_inputDriver;
		delete m_gfxDriver;
	}

	void MouseTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void MouseTest::update(SGE::Driver& driver, float deltaTime)
	{
		Vec2Df position = m_inputDriver->getMousePosition();
		m_renderRectContainer->setPosition(position - m_renderRectContainer->getSize() * 0.5f);
	}
}
