#include <sutk/tests/HBoxTest.hpp>

#include <sutk/RenderRectFillRound.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/HBoxContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData HBoxTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Horizontal Box Container Test";
		return data;
	}

	void HBoxTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_hBoxContainer = m_uiDriver->createContainer<HBoxContainer>(rootContainer);
		m_hBoxContainer->setRect({ 0, 0, 5, 3 });
		for(u32 i = 0; i < 6; i++)
		{
			RenderableContainer* renderableContainer = m_uiDriver->createContainer<RenderableContainer>(m_hBoxContainer);
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.minSize.width = 0.0f;
				attrs.prefSize.width = 2.0f;
				renderableContainer->setLayoutAttributes(attrs);
			if(i == 1)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.minSize.width = 1.0f;
				renderableContainer->setLayoutAttributes(attrs);
			}
			else if(i == 3)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.prefSize.width = 5.0f;
				attrs.minSize.width = 2;
				renderableContainer->setLayoutAttributes(attrs);
			}
			else if(i == 4)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.prefSize.width = attrs.maxSize.width;
				attrs.minSize.width = 0.5f;
				renderableContainer->setLayoutAttributes(attrs);
			}
			RenderRectFillRound* renderRect = m_uiDriver->createRenderRect<RenderRectFillRound>(renderableContainer);
			if(i == 4)
				renderRect->setColor(Color4::red());
			else
				renderRect->setColor(Color4::green(com::Lerp(0.5f, 1.0f, com::InverseLerp<f32>(static_cast<s32>(i), 0, 5))));
			std::cout << renderableContainer->getRect() << std::endl;
		}
		m_hBoxContainer->setSize({ 10.0f, m_hBoxContainer->getSize().height });
		m_hBoxContainer->getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
		m_hBoxContainer->enableDebug();
	}

	void HBoxTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void HBoxTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void HBoxTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
