#include <sutk/AutoTest.hpp>
#include <sutk/Renderable.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK::AutoTests
{
	class AnchorRectTest : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			Container* cnt1 = driver->createContainer<Container>();
			cnt1->setSize({ 10.0, 20.0f });
			Vec2Df v { 10.0f, 20.0f };
			EXPECT_EQ(cnt1->getSize(), v);
			
			Container* cnt2 = driver->createContainer<Container>(cnt1);
			// Default size of newly created Container must be { 5.0f, 5.0f }
			Vec2Df v2 { 5.0f, 5.0f };
			EXPECT_EQ(cnt2->getSize(), v2);
			// Default local position of newly created Container must be { 0.0f, 0.0f }
			EXPECT_EQ(cnt2->getPosition(), Vec2Df::zero());

			cnt2->fitInParent();
			// Now the size of cnt2 (child container) must be equal to its parent's size
			EXPECT_EQ(cnt2->getSize(), v);
			EXPECT_EQ(cnt2->getPosition(), Vec2Df::zero());

			// Use some margins
			cnt2->fitInParent({ 1.0f, 2.0f, 2.0f, 3.0f });
			Vec2Df v3 { 7.0f,  15.0f };
			EXPECT_EQ(cnt2->getSize(), v3);
			Vec2Df v4 { 1.0f, 2.0f };
			EXPECT_EQ(cnt2->getPosition(), v4);

			// Tie all corners of child container to corresponding corners of its parent container
			cnt2->getAnchorRect()->fitToParentRect();
			// v will become { 20.0f, 40.0f }
			v *= 2;
			// Now change the size of the parent container
			cnt1->setSize(v);
			EXPECT_EQ(cnt1->getSize(), v);
			Vec2Df v5 { 17.0f, 35.0f };
			// Check size and local position of the child container
			EXPECT_EQ(cnt2->getSize(), v5);
			EXPECT_EQ(cnt2->getPosition(), v4);

			// Tie only the top and bottom sides of the child container to corresponding sides (top and bottom) of its parent container
			cnt2->getAnchorRect()->setRect({ 0.0f, 0.0f, 0.0f, 1.0f });
			// Resize the parent container
			cnt1->setSize({ 40, 50 });
			// Now check the child container's size and location position
			Vec2Df v6 { 17.0f, 45.0f };
			EXPECT_EQ(cnt2->getSize(), v6);
			EXPECT_EQ(cnt2->getPosition(), v4);

			// Do it again with some different values
			cnt2->setPosition(Vec2Df::zero());
			cnt2->setSize({ 100.0f, cnt1->getSize().y });
			cnt1->setSize({ 40, 5.0f });
			Vec2Df v7 { 100.0f, 5.0f };
			EXPECT_EQ(cnt2->getSize(), v7);
			EXPECT_EQ(cnt2->getPosition(), Vec2Df::zero());

			return TestResult::passed();
		}
	};

	static AnchorRectTest instance("test.AnchorRect", com::True);
}
