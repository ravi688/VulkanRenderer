#include <sutk/tests/VBoxTest.hpp>

#include <sutk/RenderRectFillRound.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/VBoxContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData VBoxTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Vertical Box Container Test";
		return data;
	}

	void VBoxTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_vBoxContainer = m_uiDriver->createContainer<VBoxContainer>(rootContainer);
		m_vBoxContainer->setRect({ 0, 0, 3, 5 });
		for(u32 i = 0; i < 6; i++)
		{
			RenderableContainer* renderableContainer = m_uiDriver->createContainer<RenderableContainer>(m_vBoxContainer);
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.minSize.height = 0.0f;
				attrs.prefSize.height = 1.0f;
				renderableContainer->setLayoutAttributes(attrs);
			if(i == 1)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.minSize.height = 1.0f;
				renderableContainer->setLayoutAttributes(attrs);
			}
			else if(i == 3)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.prefSize.height = 4.0f;
				attrs.minSize.height = 3;
				renderableContainer->setLayoutAttributes(attrs);
			}
			else if(i == 4)
			{
				LayoutAttributes attrs = renderableContainer->getLayoutAttributes();
				attrs.prefSize.height = attrs.maxSize.height;
				attrs.minSize.height = 0.5f;
				renderableContainer->setLayoutAttributes(attrs);
			}
			RenderRectFillRound* renderRect = m_uiDriver->createRenderable<RenderRectFillRound>(renderableContainer);
			if(i == 4)
				renderRect->setColor(Color4::red());
			else
				renderRect->setColor(Color4::green(com::Lerp(0.5f, 1.0f, com::InverseLerp<f32>(static_cast<s32>(i), 0, 5))));
			std::cout << renderableContainer->getRect() << std::endl;
		}
		m_vBoxContainer->setSize({ m_vBoxContainer->getSize().width, 10.0f });
		m_vBoxContainer->getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
		m_vBoxContainer->enableDebug();
	}

	void VBoxTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void VBoxTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void VBoxTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
