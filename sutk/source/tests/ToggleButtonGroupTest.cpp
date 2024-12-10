#include <sutk/tests/ToggleButtonGroupTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/ToggleButtonListView.hpp>
#include <sutk/ToggleButtonGroupView.hpp>
#include <sutk/Label.hpp>

namespace SUTK
{
	ITest::TestInitializationData ToggleButtonGroupTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Toggle Button Group Test";
		return data;
	}

	void ToggleButtonGroupTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_toggleListView = m_uiDriver->createContainer<ToggleButtonListView>(rootContainer);
		m_toggleListView->setRect({ 5, 5, 3, 5 });
		m_toggleListView->addButton("Toggle Button 1");
		m_toggleListView->addButton("Toggle Button 2");
		m_toggleListView->addButton("Toggle Button 3");
		m_toggleListView->addButton("Toggle Button 4");
		m_toggleListView->addButton("Toggle Button 5");
		m_toggleListView->addButton("Toggle Button 6");
		m_toggleListView->fit();
		m_toggleListView->getAnchorRect()->setRect({ 0.0f, 0.5f, 1.0f, 0.0f });

		m_toggleGroupView = m_uiDriver->createContainer<ToggleButtonGroupView>(rootContainer);
		m_toggleGroupView->setRect({ 5, 11, 3, 5 });
		m_toggleGroupView->addButton("Toggle Button 1");
		m_toggleGroupView->addButton("Toggle Button 2");
		m_toggleGroupView->addButton("Toggle Button 3");
		m_toggleGroupView->addButton("Toggle Button 4");
		m_toggleGroupView->addButton("Toggle Button 5");
		m_toggleGroupView->addButton("Toggle Button 6");
		m_toggleGroupView->fit();
		m_toggleGroupView->getAnchorRect()->setRect({ 0.0f, 0.5f, 1.0f, 0.0f });

		m_toggleGroupView->getOnSelectEvent().subscribe([](u32 index)
		{
			std::cout << "Selected: " << index << std::endl;
		});
	}

	void ToggleButtonGroupTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void ToggleButtonGroupTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void ToggleButtonGroupTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
