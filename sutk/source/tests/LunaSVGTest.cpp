#include <sutk/tests/LunaSVGTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData LunaSVGTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "LunaSVG Test (press B to increase size, press C to decrease size)";
		return data;
	}

	void LunaSVGTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		UIDriver::ImageReference image = m_uiDriver->loadImage("svg_files/close-cross-symbol-in-a-circle-svgrepo-com.svg");
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		Container* emptyContainer = m_uiDriver->createContainer<Container>(rootContainer);
		emptyContainer->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor = emptyContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(emptyContainer);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 5, 5 });
		anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 1, 1 });
		anchor->setBottomRight({ 1, 1 });
		emptyContainer->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });
		m_renderRect = m_uiDriver->createRenderable<RenderImage>(m_renderRectContainer);
		m_renderRect->setColor(Color4::green());
		m_renderRect->setImage(image);

		RenderableContainer* renderRectContainer2 = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		RenderRectFill* renderRect2 = m_uiDriver->createRenderable<RenderRectFill>(renderRectContainer2);
		renderRectContainer2->setRect({ 5.0f, 10.0f, 5.0f, 5.0f });
		AnchorRect* anchor2 = renderRectContainer2->getAnchorRect();
		anchor2->setTopLeft({ 0.0f, 0.0f });
		anchor2->setBottomRight({ 1.0f, 1.0f });
		renderRect2->setColor(Color4::red());

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			if((keycode == KeyCode::B) && (event == KeyEvent::Press))
			{
				auto size = m_renderRectContainer->getSize();
				size *= 1.2f;
				m_renderRectContainer->setSize(size);
			}
			else if((keycode == KeyCode::C) && (event == KeyEvent::Press))
			{
				auto size = m_renderRectContainer->getSize();
				size /= 1.2f;
				m_renderRectContainer->setSize(size);
			}
		});
	}

	void LunaSVGTest::terminate(SGE::Driver& driver)
	{
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void LunaSVGTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void LunaSVGTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
