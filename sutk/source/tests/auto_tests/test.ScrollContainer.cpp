#include <sutk/AutoTest.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/ScrollContainer.hpp>
#include <sutk/ScrollableContainer.hpp>

namespace SUTK::AutoTests
{
	class ScrollContainerTest : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			auto* scrlCnt = driver->createContainer<ScrollContainer>();
			
			// By default container size must be { 5.0f, 5.0f }
			Vec2Df size { 5.0f, 5.0f };
			EXPECT_EQ(scrlCnt->getSize(), size);

			// By default scroll delta must be zero
			EXPECT_EQ(scrlCnt->getScrollDelta(), Vec2Df::zero());

			scrlCnt->setSize({ 40.0f, 10.0f });
			auto* scrlbCnt = driver->createContainer<ScrollableContainer<>>(scrlCnt);
			// The anchors must automatically be set to Rect2Df::zero for any Container derived from Scrollable class
			EXPECT_EQ(scrlbCnt->getAnchorRect()->getRect(), Rect2Df::zero());
			// By default scroll margins must be zero
			EXPECT_EQ(scrlbCnt->getScrollMargins(), RectMarginsf::zero());

			// Since the width and height of the Scrollable ({ 5, 5 }) is smaller than { 40.0f, 10.0f }
			// isScrollable() must return com::False
			EXPECT_EQ(scrlCnt->isScrollable(), com::False);

			// Test: Full Margin for RectEdge::Left
			scrlbCnt->setFullScrollMargins(RectEdgeBits::Left);
			EXPECT_EQ(scrlbCnt->getScrollMargins().left, scrlCnt->getSize().width);

			EXPECT_EQ(scrlCnt->isScrollableRight(), com::True);
			scrlCnt->addScrollDelta(Vec2Df::right() * 6.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::right() * 6.0f);
			EXPECT_EQ(scrlCnt->isScrollableRight(), com::True);
			
			scrlCnt->addScrollDelta(Vec2Df::right() * 100.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::right() * scrlCnt->getSize().width);
			EXPECT_EQ(scrlCnt->isScrollableRight(), com::False);
			
			scrlCnt->addScrollDelta(Vec2Df::left() * 100.0f);
			EXPECT_EQ(scrlCnt->isScrollableLeft(), com::False);
			EXPECT_EQ(scrlbCnt->getRect().getRight(), scrlCnt->getSize().width);
			EXPECT_EQ(scrlCnt->isScrollableRight(), com::True);

			// Test: Full Margin for RectEdge::Right
			scrlbCnt->setFullScrollMargins(RectEdgeBits::Right);
			scrlCnt->resetScrollDelta();
			EXPECT_EQ(scrlCnt->getScrollDelta(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getScrollMargins().right, scrlCnt->getSize().width);

			EXPECT_EQ(scrlCnt->isScrollableLeft(), com::True);
			scrlCnt->addScrollDelta(Vec2Df::left() * 7.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::left() * 5.0f);
			EXPECT_EQ(scrlCnt->isScrollableLeft(), com::False);
			EXPECT_EQ(scrlCnt->isScrollableRight(), com::True);

			scrlCnt->addScrollDelta(Vec2Df::right() * 200.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::zero());
			EXPECT_EQ(scrlCnt->isScrollableRight(), com::False);

			scrlCnt->addScrollDelta(Vec2Df::left() * 5.0f);
			EXPECT_EQ(scrlbCnt->getRect().getLeft(), -5.0f);
			EXPECT_EQ(scrlCnt->isScrollableLeft(), com::False);

			// Test: Full Margin for RectEdge::Top
			scrlbCnt->setFullScrollMargins(RectEdgeBits::Top);
			EXPECT_EQ(scrlbCnt->getScrollMargins().top, scrlCnt->getSize().height);
			scrlCnt->resetScrollDelta();
			EXPECT_EQ(scrlCnt->getScrollDelta(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getScrollMargins().top, scrlCnt->getSize().height);

			EXPECT_EQ(scrlCnt->isScrollableDown(), com::True);
			scrlCnt->addScrollDelta(Vec2Df::down() * 6.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::down() * 6.0f);
			EXPECT_EQ(scrlCnt->isScrollableDown(), com::True);

			scrlCnt->addScrollDelta(Vec2Df::down() * 100.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::down() * scrlCnt->getSize().height);
			EXPECT_EQ(scrlCnt->isScrollableDown(), com::False);

			scrlCnt->addScrollDelta(Vec2Df::up() * 100.0f);
			EXPECT_EQ(scrlbCnt->getRect().getBottom(), scrlCnt->getSize().height);
			EXPECT_EQ(scrlCnt->isScrollableUp(), com::False);
			EXPECT_EQ(scrlCnt->isScrollableDown(), com::True);

			// Test: Full Margin for RectEdge::Bottom
			scrlbCnt->setFullScrollMargins(RectEdgeBits::Bottom);
			EXPECT_EQ(scrlbCnt->getScrollMargins().bottom, scrlCnt->getSize().height);
			scrlCnt->resetScrollDelta();
			EXPECT_EQ(scrlCnt->getScrollDelta(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::zero());
			EXPECT_EQ(scrlbCnt->getScrollMargins().bottom, scrlCnt->getSize().height);

			EXPECT_EQ(scrlCnt->isScrollableUp(), com::True);
			scrlCnt->addScrollDelta(Vec2Df::up() * 6.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::up() * 5.0f);
			EXPECT_EQ(scrlCnt->isScrollableUp(), com::False);

			scrlCnt->addScrollDelta(Vec2Df::up() * 100.0f);
			EXPECT_EQ(scrlbCnt->getPosition(), Vec2Df::up() * 5.0f);
			EXPECT_EQ(scrlCnt->isScrollableUp(), com::False);

			scrlCnt->addScrollDelta(Vec2Df::down() * 100.0f);
			EXPECT_EQ(scrlbCnt->getRect().getBottom(), 5.0f);
			EXPECT_EQ(scrlbCnt->getRect().getTop(), 0.0f);
			EXPECT_EQ(scrlCnt->isScrollableUp(), com::True);
			EXPECT_EQ(scrlCnt->isScrollableDown(), com::False);

			driver->destroyContainer<ScrollableContainer<>>(scrlbCnt);
			driver->destroyContainer<ScrollContainer>(scrlCnt);
			return TestResult::passed();
		}
	};

	static ScrollContainerTest instance("test.ScrollContainer", com::True);
}
