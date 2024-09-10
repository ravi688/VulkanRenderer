#include <sutk/tests/HPaneTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/HPaneContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/RenderableContainer.hpp>

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
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_hPaneContainer = m_uiDriver->createContainer<HPaneContainer>(rootContainer);
		m_hPaneContainer->alwaysFitInParent();
		// m_hPaneContainer->enableDebug();

		RenderableContainer* c1 = m_uiDriver->createContainer<RenderableContainer>(m_hPaneContainer);
		RenderRectFill* r1 = m_uiDriver->createRenderable<RenderRectFill>(c1);
		r1->setColor(Color4::red());
		RenderableContainer* c2 = m_uiDriver->createContainer<RenderableContainer>(m_hPaneContainer);
		RenderRectFill* r2 = m_uiDriver->createRenderable<RenderRectFill>(c2);
		r2->setColor(Color4::green());
		RenderableContainer* c3 = m_uiDriver->createContainer<RenderableContainer>(m_hPaneContainer);
		RenderRectFill* r3 = m_uiDriver->createRenderable<RenderRectFill>(c3);
		r3->setColor(Color4::blue());
		c1->setLayoutExpand();
		c2->setLayoutExpand();
		c3->setLayoutExpand();
	}

	void HPaneTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_inputDriver;
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
