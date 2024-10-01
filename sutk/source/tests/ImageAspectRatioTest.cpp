#include <sutk/tests/ImageAspectRatioTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	DriverInitializationData ImageAspectRatioTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Image Aspect Ratio Test";
		return data;
	}

	void ImageAspectRatioTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		UIDriver::ImageReference image = m_uiDriver->loadImage("../textures/Rider.png");
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		m_renderRectContainer->alwaysFitInParent();
		m_renderRect = m_uiDriver->createRenderRect<RenderImage>(m_renderRectContainer);
		m_renderRect->setColor(Color4::white());
		m_renderRect->setImage(image);
		auto attr = m_uiDriver->getImageAttributes(image);
		m_renderRect->setPreserveAspectRatio(true);
		m_renderRect->setAspectRatio({ attr.width, attr.height });
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
