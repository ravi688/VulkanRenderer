#include <sutk/tests/RoundRectTest.hpp>

#include <sutk/RenderRectFillRound.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData RoundRectTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Round Rect Test (SDF)";
		return data;
	}

	void RoundRectTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		Container* emptyContainer = m_uiDriver->createContainer<Container>(rootContainer, com::Bool::False(), 100);
		emptyContainer->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor = emptyContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(emptyContainer, com::Bool::False(), 10000);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 5, 5 });
		anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 1, 1 });
		anchor->setBottomRight({ 1, 1 });
		emptyContainer->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });
		m_renderRect = m_uiDriver->createRenderable<RenderRectFillRound>(m_renderRectContainer);
		m_renderRect->setColor(Color4::green());

		Container* emptyContainer1 = m_uiDriver->createContainer<Container>(emptyContainer);
		emptyContainer1->alwaysFitInParent();
		Container* emptyContainer2 = m_uiDriver->createContainer<Container>(emptyContainer1);
		emptyContainer2->alwaysFitInParent();
		Container* emptyContainer3 = m_uiDriver->createContainer<Container>(emptyContainer2);
		emptyContainer3->alwaysFitInParent();
		RenderableContainer* renderRectContainer2 = m_uiDriver->createContainer<RenderableContainer>(emptyContainer3);
		RenderRectFill* renderRect2 = m_uiDriver->createRenderable<RenderRectFill>(renderRectContainer2);
		renderRectContainer2->setRect({ 5.0f, 10.0f, 5.0f, 5.0f });
		AnchorRect* anchor2 = renderRectContainer2->getAnchorRect();
		anchor2->setTopLeft({ 0.0f, 0.0f });
		anchor2->setBottomRight({ 1.0f, 1.0f });
		renderRect2->setColor(Color4::red());
	}

	void RoundRectTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void RoundRectTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void RoundRectTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
