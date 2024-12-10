#include <sutk/tests/TransientContTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderImage.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/TransientContainer.hpp>
#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/Button.hpp>

namespace SUTK
{
	class TestContainer : public TransientContainer<RenderableContainer>
	{
	private:
		RenderImage* m_image;
	public:
		TestContainer(UIDriver& driver, Container* parent) : TransientContainer(driver, parent, false)
		{
			UIDriver::ImageReference image = driver.loadImage("../textures/Smile.bmp");
			m_image = driver.createRenderable<RenderImage>(this);
			m_image->setColor(Color4::green());
			m_image->setImage(image);

			getController().getAutomaton().setTransitionDelay(0.5f);

			// Mandatory to be called here
			restoreState();
		}
	};
}

namespace SUTK
{
	ITest::TestInitializationData TransientContTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Transient Container Test";
		return data;
	}

	void TransientContTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		Container* emptyContainer = m_uiDriver->createContainer<Container>(rootContainer);
		emptyContainer->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor = emptyContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_testContainer = m_uiDriver->createContainer<TestContainer>(emptyContainer);
		m_testContainer->setRect({ 1.0f, 1.0f, 5, 5 });
		anchor = m_testContainer->getAnchorRect();
		anchor->setTopLeft({ 1, 1 });
		anchor->setBottomRight({ 1, 1 });
		emptyContainer->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });

		m_button = m_uiDriver->createContainer<Button>(rootContainer);
		m_button->setRect({ 1, 1, 4, 2 });
		m_button->getOnPressEvent().subscribe([this](Button*)
		{
			if(this->m_testContainer->isHidden())
			{
				std::cout << "Showing..." << std::endl;
				this->m_testContainer->show();
			}
			else
			{
				std::cout << "Hiding..." << std::endl;
				this->m_testContainer->hide();
			}
		});
		auto& label =  m_button->getGraphicAs<DefaultButtonGraphic>()->getLabel();
		label.getText().setColor(Color4::black());
		label.set("Toggle");
	}

	void TransientContTest::terminate(SGE::Driver& driver)
	{
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
		delete m_inputDriver;
	}

	void TransientContTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void TransientContTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
