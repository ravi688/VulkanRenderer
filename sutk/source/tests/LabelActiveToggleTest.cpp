#include <sutk/tests/LabelActiveToggleTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/Label.hpp>

#include <sutk/ContainerUtility.hpp>

namespace SUTK
{
	ITest::TestInitializationData LabelActiveToggleTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Label Active Toggle Test (press Any key to toggle Active state)";
		return data;
	}

	void LabelActiveToggleTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_label = m_uiDriver->createContainer<Label>(rootContainer);
		m_label->enableDebug();
		// EXPECT: position is dirty
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: data is dirty
		m_label->set("Albanian and Indian");
		// EXPECT: isActive is dirty
		ContainerUtility::SetActiveAllRecursive(m_label, com::False);

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver*, KeyCode key, KeyEvent action, ModifierKeys modifiers)
		{
			static com::Bool isSwap = com::True;
			if(action == KeyEvent::Press)
			{
				if(isSwap)
					// EXPECT: The Labe must be aligned in the center/middle
					ContainerUtility::SetActiveAllRecursive(m_label, com::True);
				else
					ContainerUtility::SetActiveAllRecursive(m_label, com::False);
				isSwap = !isSwap;
			}
		});
	}

	void LabelActiveToggleTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyContainer<Label>(m_label);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_rootContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void LabelActiveToggleTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void LabelActiveToggleTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
