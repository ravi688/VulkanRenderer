#include <sutk/AutoTest.hpp>

namespace SUTK::AutoTests
{
	class ContainerGetChildTest : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			// Passed;
			return { com::True };
		}
	};

	static ContainerGetChildTest instance("test.Container.getChildTest()");
}
