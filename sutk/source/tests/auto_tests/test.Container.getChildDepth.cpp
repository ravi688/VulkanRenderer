#include <sutk/AutoTest.hpp>
#include <sutk/Container.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK::AutoTests
{
	class ContainerGetChildTest : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			Container* cnt1 = driver->createContainer<Container>();
			Container* cnt2 = driver->createContainer<Container>(cnt1);
			Container* cnt3 = driver->createContainer<Container>(cnt1);
			Container* cnt4 = driver->createContainer<Container>(cnt3);
			Container* cnt5 = driver->createContainer<Container>(cnt3);
			Container* cnt6 = driver->createContainer<Container>(cnt5);

			EXPECT_EQ(cnt1->getMaxChildDistance(), 3);
			EXPECT_EQ(cnt2->getMaxChildDistance(), 0);
			EXPECT_EQ(cnt3->getMaxChildDistance(), 2);
			EXPECT_EQ(cnt4->getMaxChildDistance(), 0);
			EXPECT_EQ(cnt5->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt6->getMaxChildDistance(), 0);

			Container* cnt7 = driver->createContainer<Container>();
			cnt7->setParent(cnt4);
			EXPECT_EQ(cnt4->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt7->getMaxChildDistance(), 0);

			cnt3->setParent(com::null_pointer<Container>());
			EXPECT_EQ(cnt1->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt2->getMaxChildDistance(), 0);
			EXPECT_EQ(cnt3->getMaxChildDistance(), 2);
			EXPECT_EQ(cnt4->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt5->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt6->getMaxChildDistance(), 0);

			cnt3->setParent(cnt2);
			EXPECT_EQ(cnt1->getMaxChildDistance(), 4);
			EXPECT_EQ(cnt2->getMaxChildDistance(), 3);
			EXPECT_EQ(cnt3->getMaxChildDistance(), 2);
			EXPECT_EQ(cnt4->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt5->getMaxChildDistance(), 1);
			EXPECT_EQ(cnt6->getMaxChildDistance(), 0);

			driver->destroyContainer(cnt7);
			driver->destroyContainer(cnt6);
			driver->destroyContainer(cnt5);
			driver->destroyContainer(cnt4);
			driver->destroyContainer(cnt3);
			driver->destroyContainer(cnt2);
			driver->destroyContainer(cnt1);
			return TestResult::passed();
		}
	};

	static ContainerGetChildTest instance("test.Container.getChildTest()", com::True);
}
