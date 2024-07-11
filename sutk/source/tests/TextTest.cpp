#include <sutk/tests/TextTest.hpp>

#include <sutk/Text.hpp>
#include <sutk/SGEGfxDriver.hpp>

#include <common/id_generator.h>

namespace SUTK
{
	DriverInitializationData TextTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Text Test";
		return data;
	}

	void TextTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver);
		Container* rootContainer = m_uiDriver->createContainer<Container>(NULL);
		TextContainer* textContainer = m_uiDriver->createContainer<TextContainer>(rootContainer);
		m_text = m_uiDriver->createText(textContainer);
		m_text->set("Hello World");
	}

	void TextTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void TextTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void TextTest::update(SGE::Driver& driver, float deltaTime)
	{
		static float t = 0;
		t += deltaTime;
		if((static_cast<int>(deltaTime) % 1000) == 0)
		{
			static u64 counter = 0;
			++counter;
			char buffer[128];
			sprintf(buffer, " %llu", counter);
			m_text->append(buffer);
		}
	}
}
