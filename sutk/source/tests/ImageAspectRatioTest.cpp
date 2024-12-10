#include <sutk/tests/ImageAspectRatioTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData ImageAspectRatioTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Image Aspect Ratio Test (Press B to toggle Aspect Ratio Preserve)";
		return data;
	}

	void ImageAspectRatioTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		UIDriver::ImageReference image = m_uiDriver->loadImage("../textures/Rider.png");
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		m_renderRectContainer->alwaysFitInParent();
		m_renderRect = m_uiDriver->createRenderable<RenderImage>(m_renderRectContainer);
		m_renderRect->setColor(Color4::white());
		m_renderRect->setImage(image);
		auto attr = m_uiDriver->getImageAttributes(image);
		m_renderRect->setPreserveAspectRatio(true);
		m_renderRect->setAspectRatio({ attr.width, attr.height });

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			static bool isSwap = true;
			if((keycode == KeyCode::B) && (event == KeyEvent::Press))
			{
				if(isSwap)
					this->m_renderRect->setPreserveAspectRatio(false);
				else
					this->m_renderRect->setPreserveAspectRatio(true);
				isSwap = !isSwap;
			}
		});
	}

	void ImageAspectRatioTest::terminate(SGE::Driver& driver)
	{
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void ImageAspectRatioTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ImageAspectRatioTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
