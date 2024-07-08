#include <sutk/tests/TextTest.hpp>

#include <sutk/Text.hpp>

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
		UIRendererCallbacks callbacks = { };
		callbacks.userData = reinterpret_cast<void*>(&driver);
		m_uiDriver = new UIDriver(callbacks);
		Container* rootContainer = m_uiDriver->createContainer<Container>(NULL);
		TextContainer* textContainer = m_uiDriver->createContainer<TextContainer>(rootContainer);
		m_text = m_uiDriver->createText(textContainer);
		m_text->set("Hello World");
	}

	void TextTest::terminate(SGE::Driver& driver)
	{
		delete m_uiDriver;
	}

	void TextTest::render(SGE::Driver& driver)
	{
		m_uiDriver->render();
	}

	void TextTest::update(SGE::Driver& driver, float deltaTime)
	{
		static float t = 0;
		t += deltaTime;
		if((static_cast<int>(deltaTime) % 500) == 0)
		{
			static u64 counter = 0;
			++counter;
			char buffer[128];
			sprintf(buffer, "%llu", counter);
			m_text->set(buffer);
		}
	}
}
