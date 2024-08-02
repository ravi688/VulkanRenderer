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
		Container* emptyContainer = m_uiDriver->createContainer<Container>(rootContainer);
		emptyContainer->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor = emptyContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_renderRectContainer = m_uiDriver->createContainer<RenderRectContainer>(emptyContainer);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 5, 5 });
		anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 1, 1 });
		anchor->setBottomRight({ 1, 1 });
		emptyContainer->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });
		m_renderRect = m_uiDriver->createRenderRect<RenderRectFill>(m_renderRectContainer);
		m_renderRect->setColor(Color3::green());

		RenderRectContainer* renderRectContainer2 = m_uiDriver->createContainer<RenderRectContainer>(rootContainer);
		RenderRectFill* renderRect2 = m_uiDriver->createRenderRect<RenderRectFill>(renderRectContainer2);
		renderRectContainer2->setRect({ 5.0f, 10.0f, 5.0f, 5.0f });
		AnchorRect* anchor2 = renderRectContainer2->getAnchorRect();
		anchor2->setTopLeft({ 0.0f, 0.0f });
		anchor2->setBottomRight({ 1.0f, 1.0f });
		renderRect2->setColor(Color3::red());
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
