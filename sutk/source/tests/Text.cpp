#include <sutk/tests/Text.hpp>

namespace SUTK
{
	DriverInitializationData Text::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Text";
		return data;
	}

	void Text::initialize(SGE::Driver& driver)
	{

	}

	void Text::terminate(SGE::Driver& driver)
	{

	}

	void Text::render(SGE::Driver& driver)
	{
		
	}

	void Text::update(SGE::Driver& driver, float deltaTime)
	{

	}
}
