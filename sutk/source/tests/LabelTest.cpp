#include <sutk/tests/LabelTest.hpp>

#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/FullWindowContainer.hpp>

#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/TextGroupContainer.hpp>

namespace SUTK
{
	ITest::TestInitializationData LabelTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Label Test";
		return data;
	}

	void LabelTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);
		FullWindowContainer* rootContainer = m_rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		// EXPECT: a new Text Group Object
		m_label = m_uiDriver->createContainer<Label>(rootContainer);
		m_label->enableDebug();
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText Object
		m_label->set("Albanian and Indian");

		// EXPECT: a new Text Group Object
		m_label2 = m_uiDriver->createContainer<Label>();
		m_label2->enableDebug();
		m_label2->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText Object
		m_label2->set("American and European");
		m_label2->setPosition({ 6, 0 });

		// EXPECT: a new Text Group Object
		m_label3 = m_uiDriver->createContainer<Label>();
		m_label3->enableDebug(true, Color4::red());
		m_label3->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
		// EXPECT: a new SGE::BitmapText Object
		m_label3->set("Japanease and Korean");
		m_label3->setPosition({ 1, 6 });
		m_label3->setSize({ 6, 6 });

		m_auxContainer = m_uiDriver->createContainer<Container>(rootContainer);
		m_auxContainer->setPosition({ 6, 6 });
		m_auxContainer->setSize({ 7, 7 });
		m_auxContainer->enableDebug(true, Color4::yellow());
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		m_label3->setParent(m_auxContainer);
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		m_label3->setParent(com::null_pointer<Container>());
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		m_label3->setParent(m_auxContainer, false);
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		m_label3->setParent(com::null_pointer<Container>());
		m_label3->setPosition({ 1, 6 });
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		// EXPECT: Now the Label Container must be on the left side touching the auxContainer
		m_label3->setParent(m_auxContainer);

		UIDriver::FontReference font = m_font = this->m_uiDriver->loadFont("../showcase/resource/fonts/Pushster-Regular.ttf");

		m_label3->getText().setFont(font);

		m_txtGrpContainer1 = m_uiDriver->createContainer<TextGroupContainer>(rootContainer);
		m_txtGrpContainer1->enableDebug(true, Color4::white());
		m_txtGrpContainer1->setPosition({ 7, 13 });
		// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
		m_label3->setParent(m_txtGrpContainer1, false);
		m_txtGrpContainer2 = m_uiDriver->createContainer<TextGroupContainer>(rootContainer);
		m_txtGrpContainer2->enableDebug(true, Color4::white());
		m_txtGrpContainer2->setPosition({ 14, 13 });

		m_inputDriver->getOnKeyEvent().subscribe([this](IInputDriver*, KeyCode key, KeyEvent action, ModifierKeys modifiers)
		{
			static com::Bool isSwap = com::True;
			if(action == KeyEvent::Press)
			{
				// EXPECT: no Text Group Object instantions or SGE::BitmapText Object instantions
				if(isSwap)
					m_label3->setParent(m_txtGrpContainer2, false);
				else m_label3->setParent(m_txtGrpContainer1, false);
				isSwap = !isSwap;
			}
		});

		// EXPECT: only 3 SGE::BitmapText objects must be created throughout the whole test
	}

	void LabelTest::terminate(SGE::Driver& driver)
	{
		m_uiDriver->destroyContainer<Label>(m_label3);
		m_uiDriver->destroyContainer<Label>(m_label2);
		m_uiDriver->destroyContainer<Label>(m_label);
		m_uiDriver->unloadFont(m_font);
		m_uiDriver->destroyContainer<TextGroupContainer>(m_txtGrpContainer2);
		m_uiDriver->destroyContainer<TextGroupContainer>(m_txtGrpContainer1);
		m_uiDriver->destroyContainer<Container>(m_auxContainer);
		m_uiDriver->destroyContainer<FullWindowContainer>(m_rootContainer);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void LabelTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		if(m_uiDriver->isDirty())
			m_uiDriver->render();
	}

	void LabelTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
