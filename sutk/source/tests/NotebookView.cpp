#include <sutk/tests/NotebookTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/NotebookView.hpp>

namespace SUTK
{
	DriverInitializationData NotebookTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Notebook Test";
		return data;
	}

	void NotebookTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_notebookView = m_uiDriver->createContainer<NotebookView>(rootContainer);
		m_notebookView->alwaysFitInParent();

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver*, KeyCode keyCode, KeyEvent keyEvent, ModifierKeys modifierKeys) noexcept
		{
			static u32 counter = 0;
			if(keyEvent == KeyEvent::Press)
			{
				if(keyCode == KeyCode::A)
				{
					++counter;
					char buffer[128];
					sprintf(buffer, "New Page (%u)", counter);
					this->m_notebookView->createPage(buffer);
				}
				else if(keyCode == KeyCode::D)
					this->m_notebookView->removePage(this->m_notebookView->getCurrentPage());
			}
		});
	}

	void NotebookTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_inputDriver;
		delete m_gfxDriver;
	}

	void NotebookTest::render(SGE::Driver& driver)
	{
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void NotebookTest::update(SGE::Driver& driver, float deltaTime)
	{

	}
}