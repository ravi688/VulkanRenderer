#include <sutk/tests/PanelTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/Panel.hpp>

namespace SUTK
{
	ITest::TestInitializationData PanelTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Panel Test";
		return data;
	}

	void PanelTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		m_fullWindowContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());

		// Panel should be drawn before the RenderRectFill
		m_panel = m_uiDriver->createContainer<Panel>(m_fullWindowContainer);
		m_panel->setRect({ 5.0f, 10.0f, 5.0f, 5.0f });
		m_panel->setColor(Color4::white());

		// This will be draw over the Panel (green)
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(m_panel);
		m_renderRect = m_uiDriver->createRenderable<RenderRectFill>(m_renderRectContainer);
		m_renderRectContainer->setSize({ 2.5f, 2.5f });
		m_renderRect->setColor(Color4::green());
	}

	void PanelTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyRenderable<RenderRectFill>(m_renderRect);
		m_uiDriver->destroyContainer<RenderableContainer>(m_renderRectContainer);
		m_uiDriver->destroyContainer<Panel>(m_panel);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_fullWindowContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void PanelTest::render(SGE::Driver& driver)
	{
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void PanelTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
