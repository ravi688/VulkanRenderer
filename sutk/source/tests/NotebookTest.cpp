#include <sutk/tests/NotebookTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/NotebookView.hpp>

namespace SUTK
{
	ITest::TestInitializationData NotebookTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Notebook Test";
		return data;
	}

	void NotebookTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_notebookView = m_uiDriver->createContainer<NotebookView>(rootContainer);
		m_notebookView->alwaysFitInParent();

		m_notebookView->getOnPageSelectEvent().subscribe([](NotebookView* notebook, NotebookPage* page) noexcept
		{
			std::cout << "Selected: " << page->getLabel() << std::endl;
		});

		for(u32 i = 0; i < 3; ++i)
		{
			Container* container = m_notebookView->createPage(std::format("My Page {}", i))->getContainer();
			Label* label = m_uiDriver->createContainer<Label>();
			label->enableDebug(true, Color4::red());
			label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
			label->set(std::format("This is Page {}", i));
			m_labels.push_back(label);
		}

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
					auto* container = this->m_notebookView->createPage(buffer)->getContainer();
					Label* label = m_uiDriver->createContainer<Label>();
					label->enableDebug(true, Color4::red());
					label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
					label->set(std::format("This is Page {}", counter + 3));
					m_labels.push_back(label);
				}
				else if(keyCode == KeyCode::D)
					this->m_notebookView->removePage(this->m_notebookView->getCurrentPage());
				this->m_notebookView->dump();
			}
		});
	}

	void NotebookTest::terminate(SGE::Driver& driver)
	{
		for(auto* &value : m_labels)
			m_uiDriver->destroyContainer<Label>(value);
		m_uiDriver->destroyContainer<NotebookView>(m_notebookView);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_rootContainer);
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
