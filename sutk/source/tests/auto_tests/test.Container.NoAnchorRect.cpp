#include <sutk/AutoTest.hpp>
#include <sutk/Renderable.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK::AutoTests
{
	class ContainerNoAnchorRect : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			Container* cnt1 = driver->createContainer<Container>();
			Vec2Df size { 5.0f, 5.0f };
			// Default Size must be 5, 5 centimeters
			EXPECT_EQ(cnt1->getSize(), size);
			// Default Position must be 0, 0 centimeters
			Vec2Df position { 0.0f, 0.0f };
			EXPECT_EQ(cnt1->getPosition(), position);

			auto* cnt2 = driver->createContainer<Container>(cnt1);
			// By default local position of child container must be zero
			EXPECT_EQ(cnt2->getPosition(), Vec2Df::zero());
			// If the parent container's global position is zero and child container's local position is zero
			// Then the child container's global position must also be zero
			EXPECT_EQ(cnt2->getGlobalPosition(), Vec2Df::zero());

			// Move the parent container (which is also in global space) towards right by 1 centimeter
			cnt1->moveRight(1.0f);
			
			// The child container's global position's x component should increase by 1 centimeter
			// While the local position should remain same.
			position = { 1.0f, 0.0f };
			EXPECT_EQ(cnt2->getGlobalPosition(), position);
			EXPECT_EQ(cnt2->getPosition(), Vec2Df::zero());

			// Move the child container towards right by 2.5 centimeters
			cnt2->setPosition(Vec2Df::right() * 2.5f);
			// Now the child container's global position's x component should be 3.5f centimeters
			position = { 3.5f, 0.0f };
			EXPECT_EQ(cnt2->getGlobalPosition(), position);
			// The local position of the child containeir should be { 2.5f, 0 }
			position = { 2.5f, 0 };
			EXPECT_EQ(cnt2->getPosition(), position);

			// Move the parent container towards right by 7.5 centimeters 
			cnt1->moveRight(7.5f);
			// Now the global position of the child container should be { 1 + 7.5 + 2.5, 0 } = { 11, 0 }
			position = { 11.0f, 0 };
			EXPECT_EQ(cnt2->getGlobalPosition(), position);
			// The local position of the child container should remain same as { 2.5f, 0 }
			position = { 2.5f , 0 };
			EXPECT_EQ(cnt2->getPosition(), position);
			return TestResult::passed();
		}
	};

	static ContainerNoAnchorRect instance("test.Container.NoAnchorRect", com::True);
}
