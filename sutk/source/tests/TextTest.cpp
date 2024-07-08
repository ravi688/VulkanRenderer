#include <sutk/tests/TextTest.hpp>

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
		// SUTK::UIRendererCallbacks callbacks = { };
		// callbacks.userData = reinterpret_cast<void*>(&driver);
		// SUTK::UIDriver uiDriver(callbacks);
		// SUTK::Container container = uiDriver.createContainer();
		// SUTK::Text text = uiDriver.createText();
		// container.add(text);
		// text.set("Hello World");
		// text.append("Hello Another World");
		// text.appendLine("Hello Yet Another World");
		// text.insert(34, "Hello Yet Yet Another World");
		// text.appendChar('A');
		// text.insertChar(45, 'S');
		// std::size_t lineCount = text.getLineCount();
		// text.appendLine();
	}

	void TextTest::terminate(SGE::Driver& driver)
	{

	}

	void TextTest::render(SGE::Driver& driver)
	{
		// uiDriver.render();
	}

	void TextTest::update(SGE::Driver& driver, float deltaTime)
	{

	}
}
