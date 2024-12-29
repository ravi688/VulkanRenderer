#include <sutk/tests/PassiveLabelTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/PassiveLabel.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/TextGroupContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData PassiveLabelTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Passive Label Test";
		return data;
	}

	void PassiveLabelTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		// EXPECT: a new Text Group object creation
		m_label = m_uiDriver->createContainer<PassiveLabel>(rootContainer);
		m_label->enableDebug();
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText object creation
		m_label->set("Albanian and Indian");

		// EXPECT: a new Text Group object creation
		m_label2 = m_uiDriver->createContainer<PassiveLabel>();
		m_label2->enableDebug();
		m_label2->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText object creation
		m_label2->set("American and European");
		m_label2->setPosition({ 6, 0 });

		// EXPECT: a new Text Group object creation
		m_label3 = m_uiDriver->createContainer<PassiveLabel>();
		m_label3->enableDebug(true, Color4::red());
		m_label3->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText object creation
		m_label3->set("Japanease and Korean");
		m_label3->setPosition({ 1, 6 });
		m_label3->setSize({ 6, 6 });

		m_auxContainer = m_uiDriver->createContainer<Container>(rootContainer);
		m_auxContainer->setPosition({ 6, 6 });
		m_auxContainer->setSize({ 7, 7 });
		m_auxContainer->enableDebug(true, Color4::yellow());
		// EXPECT: No instantion of Text Group object or SGE::BitmapText object
		m_label3->setParent(m_auxContainer);
		// EXPECT: No instantion of Text Group object or SGE::BitmapText object
		m_label3->setParent(com::null_pointer<Container>());
		// EXPECT: No instantion of Text Group object or SGE::BitmapText object
		m_label3->setParent(m_auxContainer, false);
		// EXPECT: No instantion of Text Group object or SGE::BitmapText object
		m_label3->setParent(com::null_pointer<Container>());
		m_label3->setPosition({ 1, 6 });
		// EXPECT: No instantion of Text Group object or SGE::BitmapText object
		// EXPECT: Now the Label Container must be on the left side touching the auxContainer
		m_label3->setParent(m_auxContainer);

		UIDriver::FontReference font = m_font = this->m_uiDriver->loadFont("../showcase/resource/fonts/Pushster-Regular.ttf");

		m_label3->setFont(font);

		m_txtGrpContainer1 = m_uiDriver->createContainer<TextGroupContainer>(rootContainer);
		m_txtGrpContainer1->enableDebug(true, Color4::white());
		m_txtGrpContainer1->setPosition({ 7, 13 });
		// EXPECT: No instantion of Text Group object
		// EXPECT: a new SGE::BitmapText object for the m_txtGrpContainer1
		m_label3->setParent(m_txtGrpContainer1, false);
		m_txtGrpContainer2 = m_uiDriver->createContainer<TextGroupContainer>(rootContainer);
		m_txtGrpContainer2->enableDebug(true, Color4::white());
		m_txtGrpContainer2->setPosition({ 14, 13 });

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver*, KeyCode key, KeyEvent action, ModifierKeys modifiers)
		{
			static com::Bool isSwap = com::True;
			if(action == KeyEvent::Press)
			{
				if(isSwap)
					// EXPECT: No instantion of Text Group object
					// EXPECT: (for the first hit) a new SGE::BitmapText object for the m_txtGrpContainer1
					m_label3->setParent(m_txtGrpContainer2, false);
				else m_label3->setParent(m_txtGrpContainer1, false);
				isSwap = !isSwap;
			}
		});

		// EXPECT: almost 5 SGE::BitmapText objects must be created throughout the whole test
	}

	void PassiveLabelTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyContainer<PassiveLabel>(m_label3);
		m_uiDriver->destroyContainer<PassiveLabel>(m_label2);
		m_uiDriver->destroyContainer<PassiveLabel>(m_label);
		m_uiDriver->unloadFont(m_font);
		m_uiDriver->destroyContainer<TextGroupContainer>(m_txtGrpContainer2);
		m_uiDriver->destroyContainer<TextGroupContainer>(m_txtGrpContainer1);
		m_uiDriver->destroyContainer<Container>(m_auxContainer);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_rootContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void PassiveLabelTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void PassiveLabelTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
