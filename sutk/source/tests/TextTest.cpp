#include <sutk/tests/TextTest.hpp>

#include <sutk/Text.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/TextContainer.hpp>
#include <sutk/FullWindowContainer.hpp>

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
		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(NULL);
		m_textContainer = m_uiDriver->createContainer<TextContainer>(rootContainer);
		m_textContainer->setRect({ 1.0f, 0, 7, 7 });
		m_text = m_uiDriver->createText(m_textContainer);
		m_text->append("#include <iostream>\n");
		m_text->append("int main()\n");
		m_text->append("{\n");
		m_text->append("\tuint32_t a = 232442; \n");
		m_text->append("\tuint32_t b = 2343244; \n");
		m_text->append("\n");
		m_text->append("\tuint32_t c = a + b; \n");
		m_text->append("\treturn 0; \n");
		m_text->append("}");

		m_text->enableClipping();
		// m_text->createNewLine(Text::Flags::After, 3);
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
		static u64 counter = 0;
		++counter;
		m_textContainer->setRect({ 1.0f + (counter / 2000.0f) * 1.0f , (counter / 2000.0f) * 2.0f, 7.0f, 7.0f });
		if(counter >= 1000)
		{
			counter = 0;
			static u64 counter2 = 0;
			++counter2;
			char buffer[128];
			sprintf(buffer, " %llu", counter2);
			m_text->append(buffer);
			counter = 0;

			static u64 counter3 = 0;
			++counter3;
			if(counter3 == 15)
			{
				counter3 = 0;
				m_text->insert(CursorPosition<LineCountType>::EndOfText(), "\nH");
				// m_text->append("\n");
			}
			if(counter3 == 14)
			{
				m_text->insert({ 3, 0 }, "#$");
				m_text->insert(CursorPosition<LineCountType>::EndOfLine(4), "\nHello World");
			}
		}
	}
}
