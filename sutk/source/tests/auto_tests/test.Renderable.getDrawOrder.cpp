#include <sutk/AutoTest.hpp>
#include <sutk/Renderable.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK::AutoTests
{
	class DummyRenderable : public Renderable
	{
	public:
		using Renderable::Renderable;

		virtual bool isDirty() noexcept override { return false; }
		virtual void update() noexcept override { }
	};

	class RenderableGetDrawOrder : public AutoTest
	{
	public:
		// Inherit base constructors as it is
		using AutoTest::AutoTest;
		virtual TestResult run(SUTK::UIDriver* driver) override
		{
			// 1. Create Renderable -> it should automatically derive Draw Order from the RenderableContainer
			// 2. RenderableContainer::setRenderable() -> must be private and user code shouldn't be able to call it directly
			// 3. Renderable::setDrawOrder() -> must be private and user code shouldn't be able to call it directly
			// 4. (Any Anscestor Container)::setParent() -> All the Renderables below and including the container should be able to update their Draw Order

			Container* c1 = driver->createContainer<Container>();

			Container* c2 = driver->createContainer<Container>(c1);
			Container* c3 = driver->createContainer<Container>(c1);
			
			RenderableContainer* c4 = driver->createContainer<RenderableContainer>(c2);
			RenderableContainer* c5 = driver->createContainer<RenderableContainer>(c2);
			
			Container* c6 = driver->createContainer<Container>(c4);

			Container* c7 = driver->createContainer<Container>(c6);
			RenderableContainer* c8 = driver->createContainer<RenderableContainer>(c6);
			
			RenderableContainer* c9 = driver->createContainer<RenderableContainer>(c7);
			RenderableContainer* c10 = driver->createContainer<RenderableContainer>(c7);

			DummyRenderable* r4 = driver->createRenderable<DummyRenderable>(c4);
			EXPECT_EQ(r4->getDrawOrder(), 2);
			EXPECT_EQ(r4->isDrawOrderDirty(), true);
			DummyRenderable* r5 = driver->createRenderable<DummyRenderable>(c5);
			EXPECT_EQ(r5->getDrawOrder(), 2);
			EXPECT_EQ(r5->isDrawOrderDirty(), true);
			DummyRenderable* r8 = driver->createRenderable<DummyRenderable>(c8);
			EXPECT_EQ(r8->getDrawOrder(), 4);
			EXPECT_EQ(r8->isDrawOrderDirty(), true);
			DummyRenderable* r9 = driver->createRenderable<DummyRenderable>(c9);
			EXPECT_EQ(r9->getDrawOrder(), 5);
			EXPECT_EQ(r9->isDrawOrderDirty(), true);
			DummyRenderable* r10 = driver->createRenderable<DummyRenderable>(c10);
			EXPECT_EQ(r10->getDrawOrder(), 5);
			EXPECT_EQ(r10->isDrawOrderDirty(), true);

			EXPECT_EQ(r4->getDrawOrder(), 2);
			EXPECT_EQ(r5->getDrawOrder(), 2);
			EXPECT_EQ(r8->getDrawOrder(), 4);
			EXPECT_EQ(r9->getDrawOrder(), 5);
			EXPECT_EQ(r10->getDrawOrder(), 5);

			// Calls updateNormalizedDrawOrder()
			driver->render();

			EXPECT_EQ(r4->isDrawOrderDirty(), false);
			EXPECT_EQ(r5->isDrawOrderDirty(), false);
			EXPECT_EQ(r8->isDrawOrderDirty(), false);
			EXPECT_EQ(r9->isDrawOrderDirty(), false);
			EXPECT_EQ(r10->isDrawOrderDirty(), false);

			c1->setLayer(1000);

			EXPECT_EQ(r4->getDrawOrder(), 1002);
			EXPECT_EQ(r5->getDrawOrder(), 1002);
			EXPECT_EQ(r8->getDrawOrder(), 1004);
			EXPECT_EQ(r9->getDrawOrder(), 1005);
			EXPECT_EQ(r10->getDrawOrder(), 1005);

			EXPECT_EQ(r4->isDrawOrderDirty(), true);
			EXPECT_EQ(r5->isDrawOrderDirty(), true);
			EXPECT_EQ(r8->isDrawOrderDirty(), true);
			EXPECT_EQ(r9->isDrawOrderDirty(), true);
			EXPECT_EQ(r10->isDrawOrderDirty(), true);

			c2->setParent(com::null_pointer<Container>());

			EXPECT_EQ(r4->getDrawOrder(), 1);
			EXPECT_EQ(r5->getDrawOrder(), 1);
			EXPECT_EQ(r8->getDrawOrder(), 3);
			EXPECT_EQ(r9->getDrawOrder(), 4);
			EXPECT_EQ(r10->getDrawOrder(), 4);

			c3->setLayer(2000);

			c2->setParent(c3);

			EXPECT_EQ(r4->getDrawOrder(), 2002);
			EXPECT_EQ(r5->getDrawOrder(), 2002);
			EXPECT_EQ(r8->getDrawOrder(), 2004);
			EXPECT_EQ(r9->getDrawOrder(), 2005);
			EXPECT_EQ(r10->getDrawOrder(), 2005);

			c2->setParent(c1);

			EXPECT_EQ(r4->getDrawOrder(), 1002);
			EXPECT_EQ(r5->getDrawOrder(), 1002);
			EXPECT_EQ(r8->getDrawOrder(), 1004);
			EXPECT_EQ(r9->getDrawOrder(), 1005);
			EXPECT_EQ(r10->getDrawOrder(), 1005);

			driver->destroyRenderable(r4);
			driver->destroyRenderable(r5);
			driver->destroyRenderable(r8);
			driver->destroyRenderable(r9);
			driver->destroyRenderable(r10);

			driver->destroyContainer(c10);
			driver->destroyContainer(c9);
			driver->destroyContainer(c8);
			driver->destroyContainer(c7);
			driver->destroyContainer(c6);
			driver->destroyContainer(c5);
			driver->destroyContainer(c4);
			driver->destroyContainer(c3);
			driver->destroyContainer(c2);
			driver->destroyContainer(c1);

			return TestResult::passed();
		}
	};

	static RenderableGetDrawOrder instance("test.Renderable.getDrawOrder()", com::True);
}
