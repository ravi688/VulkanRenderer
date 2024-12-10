#include <sutk/tests/ImageButtonGraphicTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/Button.hpp>
#include <sutk/RenderImage.hpp>

namespace SUTK
{
	ITest::TestInitializationData ImageButtonGraphicTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Image Button Graphic Test";
		return data;
	}

	void ImageButtonGraphicTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());

		m_button = m_uiDriver->createContainer<Button>(rootContainer, false);
		m_button->setRect({ 1, 1, 4, 2 });
		m_buttonGraphic = m_uiDriver->createObject<ImageOverDefaultButtonGraphicProxy>(m_button);
		m_button->setGraphic(m_buttonGraphic);
		ImageButtonGraphic* imageGraphic = m_buttonGraphic->getImageGraphic();
		UIDriver::ImageReference imageRef = m_uiDriver->loadImage("../textures/Rider.png");
		imageGraphic->setImage(imageRef);
		imageGraphic->setHoverColor(Color4::grey(0.7f));
		imageGraphic->setPressColor(Color4::grey(0.5f));
		imageGraphic->getRenderImage().preserveImageAspectRatio();

		m_button2 = m_uiDriver->createContainer<Button>(rootContainer, false);
		m_button2->setRect({ 6, 1, 4, 2 });
		m_buttonGraphic2 = m_uiDriver->createContainer<ImageButtonGraphic>(m_button2);
		m_button2->setGraphic(m_buttonGraphic2);
		m_buttonGraphic2->setImage(imageRef);
		m_buttonGraphic2->setHoverColor(Color4::grey(0.7f));
		m_buttonGraphic2->setPressColor(Color4::grey(0.5f));
		m_buttonGraphic2->getRenderImage().preserveImageAspectRatio();
	}

	void ImageButtonGraphicTest::terminate(SGE::Driver& driver)
	{
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
		delete m_inputDriver;
	}

	void ImageButtonGraphicTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ImageButtonGraphicTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
