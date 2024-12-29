#include <sutk/tests/DrawOrderTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/Button.hpp>
#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp>
#include <sutk/Panel.hpp>

namespace SUTK
{
	ITest::TestInitializationData DrawOrderTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Draw Order Test";
		return data;
	}

	void DrawOrderTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		m_panel1 = m_uiDriver->createContainer<Panel>(rootContainer);
		m_panel1->setRect({ 5, 5, 3, 1 });
		m_panel1->setColor(Color4::blue());

		m_panel2 = m_uiDriver->createContainer<Panel>(rootContainer);
		m_panel2->setRect({ 5, 6.1f, 3, 1 });
		m_panel2->setColor(Color4::yellow());

		m_panel = m_uiDriver->createContainer<Panel>(rootContainer);
		m_panel->setColor(Color4::green());
		m_panel->setRect({ 4, 4, 10, 10 });
		// EXPECT: This Panel must drawn on top of the Pane1 and Panel2 (previous two)
		m_panel->setLayer(m_panel1->getMaxChildDistance() + m_panel1->getDepth() + 1);


		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver*, KeyCode key, KeyEvent action, ModifierKeys modifiers)
		{
			static com::Bool isSwap = com::True;
			if(action == KeyEvent::Press)
			{
				if(isSwap)
				 	m_panel->setLayer(0);
				else
					m_panel->setLayer(m_panel1->getMaxChildDistance() + m_panel1->getDepth() + 1);
				isSwap = !isSwap;
				std::cout << "Changing layer of m_panel" << std::endl;
			}
		});
	}

	void DrawOrderTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyContainer<Panel>(m_panel2);
		m_uiDriver->destroyContainer<Panel>(m_panel1);
		m_uiDriver->destroyContainer<Panel>(m_panel);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_rootContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void DrawOrderTest::render(SGE::Driver& driver)
	{
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void DrawOrderTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
