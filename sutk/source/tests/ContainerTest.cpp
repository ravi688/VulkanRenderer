#include <sutk/tests/ContainerTest.hpp>

#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData ContainerTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Container Test";
		return data;
	}

	void ContainerTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());

		Container* c1 = m_uiDriver->createContainer<Container>(rootContainer);
		c1->enableDebug();
		c1->setPosition({ 1, 1 });
		Container* c2 = m_uiDriver->createContainer<Container>(com::null_pointer<Container>());
		c2->setRect({ 2, 2, 6, 6 });
		c2->setParent(c1);
		c2->getAnchorRect()->fitToParentRect();
		c2->enableDebug(true, Color4::yellow());
		c1->getAnchorRect()->fitToParentRect();

		m_inputDriver->getOnKeyEvent().subscribe([c2](IInputDriver*, KeyCode keycode, KeyEvent action, ModifierKeys)
		{
			if(action == KeyEvent::Press)
			{
				std::cout << c2->getRect() << std::endl;
			}
		});
	}

	void ContainerTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void ContainerTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ContainerTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
