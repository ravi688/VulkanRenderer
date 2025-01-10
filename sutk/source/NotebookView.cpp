#include <sutk//NotebookView.hpp>
#include <sutk/TabBarView.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/TextGroupContainer.hpp>
#include <sutk/ContainerUtility.hpp>
#include <sutk/ButtonGraphic.hpp> // for SUTK::ImageButtonGraphic
#include <sutk/TabButtonGraphic.hpp> // for SUTK::TabButtonGraphic
#include <sutk/Panel.hpp> // for SUTK::Panel
#include <sutk/Constants.hpp>

namespace SUTK
{

	NotebookPage::NotebookPage(UIDriver& driver, Container* container) noexcept : UIDriverObject(driver), m_tab(com::null_pointer<Tab>()), m_container(container) { }
	NotebookPage::~NotebookPage() noexcept
	{
	}

	NotebookPage* NotebookPage::getNext() noexcept
	{
		auto next = m_tab->getNext();
		if(next)
			return next->getPage();
		return com::null_pointer<NotebookPage>();
	}

	NotebookPage* NotebookPage::getPrev() noexcept
	{
		auto prev = m_tab->getPrev();
		if(prev)
			return prev->getPage();
		return com::null_pointer<NotebookPage>();
	}

	void NotebookPage::setLabel(const std::string_view str) noexcept
	{
		m_tab->setLabel(str);
	}

	const std::string& NotebookPage::getLabel() const noexcept
	{
		return m_tab->getTabView()->getLabel();
	}

	u32 NotebookPage::getIndex() const noexcept
	{
		return m_tab->getIndex();
	}

	void NotebookPage::setActive(com::Bool isActive) noexcept
	{
		ContainerUtility::SetActiveAllRecursive(getContainer(), isActive);
		if(isActive)
			onActivate();
		else
			onDeactivate();
	}

	TabView::TabView(UIDriver& driver, Container* parent) noexcept : Button(driver, parent, /* isCreateDefaultGraphic: */ false)
	{
		m_graphic = driver.createContainer<TabButtonGraphic>(this);
		setGraphic(m_graphic);
	}

	TabView::~TabView() noexcept
	{
		getUIDriver().destroyContainer<TabButtonGraphic>(m_graphic);
	}

	void TabView::setLabel(const std::string_view str) noexcept
	{
		m_graphic->getLabel().set(str);
		setSize({ m_graphic->getMinBoundSize().width, getSize().height });
	}

	const std::string& TabView::getLabel() noexcept
	{
		return m_graphic->getLabel().get();
	}

	void TabView::unselectedState() noexcept
	{
		m_graphic->unselectedState();
	}

	void TabView::selectedState() noexcept
	{
		m_graphic->selectedState();
	}

	NotebookView::NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : VBoxContainer(driver, parent, /* isLockLayout: */ true, isLayoutIgnore, layer),
																													m_onPageSelectEvent(this)
	{
		// Create Tab Container
		// NOTE: the 'getDepth() + 10000' is needed to keep the tabs always on top of the pages (pages can be also be scrolled up and overlap with the tabs!)
		m_textGroupContainer = driver.createContainer<TextGroupContainer>(this, com::False, getDepth() + 10000);
		LayoutAttributes attr = m_textGroupContainer->getLayoutAttributes();
		attr.minSize.height = Constants::Defaults::Notebook::Tab::Height;
		attr.prefSize = attr.minSize;
		m_textGroupContainer->setLayoutAttributes(attr);
		m_tabBarBGPanel = driver.createContainer<Panel>(m_textGroupContainer);
		m_tabBarBGPanel->alwaysFitInParent();
		m_tabBarBGPanel->setColor(Color4::grey(0.8f));
		m_tabBarContainer = driver.createContainer<ScrollContainer>(m_tabBarBGPanel);
		m_tabBarContainer->alwaysFitInParent();
		m_tabBar = driver.createContainer<ScrollableTabBar>(m_tabBarContainer);
		m_tabBar->enableDebug();
		m_tabBar->fitInParent();
		// The height of the tabBar must match with that of its parent (The ScrollContainer)
		m_tabBar->getAnchorRect()->setRect({ 0, 0, 0, 1 });
		m_tabBar->getOnSelectEvent().subscribe([this](Tab* tab) noexcept
		{
			this->m_onPageSelectEvent.publish(tab->getPage());
			this->m_tabBar->scrollToTab(tab);
		});
		m_tabBar->getOnRemoveEvent().subscribe([this](Tab* tab) noexcept
		{
			auto* page = tab->getPage();
			// Save pointer to the Page's Container object before destroying the NotebookPage object
			// This makes sure any Container object instantiated inside the NotebookPage (in the derived class) is destroyed
			Container* pageContainer = page->m_container;
			getUIDriver().destroyObject<NotebookPage>(page);
			// Now destroy the Page Container
			getUIDriver().destroyContainer<Container>(pageContainer);
			tab->setPage(com::null_pointer<NotebookPage>());
		});

		// Create Container for holding page container
		m_pageContainerParent = driver.createContainer<Panel>(this);
		attr = m_pageContainerParent->getLayoutAttributes();
		attr.prefSize = attr.maxSize;
		_com_assert((attr.prefSize.height == F32_INFINITY) && (attr.prefSize.width == F32_INFINITY));
		m_pageContainerParent->setLayoutAttributes(attr);
		m_pageContainerParent->setColor(Color4::grey(0.6f));

		unlockLayout();
	}

	NotebookView::~NotebookView() noexcept
	{
		if(m_tabBar->getRootTab())
		{
			com::TraverseLinkedList(m_tabBar->getRootTab()->getPage(), [](NotebookPage* page) noexcept
			{
				page->getUIDriver().destroyObject<NotebookPage>(page);
			});
		}
		m_pageContainerParent->destroyAllChilds();
		getUIDriver().destroyContainer<Panel>(m_pageContainerParent);
		getUIDriver().destroyContainer<TabBar>(m_tabBar);
		getUIDriver().destroyContainer<ScrollContainer>(m_tabBarContainer);
		getUIDriver().destroyContainer<Panel>(m_tabBarBGPanel);
		getUIDriver().destroyContainer<TextGroupContainer>(m_textGroupContainer);
	}

	class TabInsertAnimation : public AnimationEngine::AnimContextBase
	{
		friend class TabAnimGroup;
	private:
		Tab* m_tab;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			TabView* tabView = m_tab->getTabView();
			f32 dstTabWidth = tabView->getSize().width;
			f32 offset = dstTabWidth - Constants::Defaults::Notebook::TabBar::TabViewAnimStartWidth;
			setSpeed(offset / Constants::Defaults::Notebook::TabBar::NewTabAnimDuration);
			setLength(offset);
			Container* tabContainer = tabView->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_tab->getIndex() + 1; i < childs.size(); ++i)
				childs[i]->moveLeft(offset);
			tabView->setSize({ Constants::Defaults::Notebook::TabBar::TabViewAnimStartWidth, tabView->getSize().height });
			// Insertion animation would still work fine if you remove these layout attribute updates
			// Because the layout controller is locked anyway, however, if the layout controller isn't locked
			// then we would need to update the layout attributes to agree with the Layout Controller.
			// For this reasons, it is important to write code which works in theoretical scenarios.
			LayoutAttributes attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
			tabView->setLayoutAttributes(attr);
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			TabView* tabView = m_tab->getTabView();
			Container* tabContainer = tabView->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_tab->getIndex() + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaValue);
			tabView->extendRight(deltaValue);
			LayoutAttributes attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
			tabView->setLayoutAttributes(attr);
		}
	public:
		TabInsertAnimation(UIDriver& driver, TabAnimGroup* group, Tab* tab) noexcept : AnimContextBase(driver, group), m_tab(tab) { }
	};

	class TabRemoveAnimation : public AnimationEngine::AnimContextBase
	{
		friend class TabAnimGroup;
	private:
		Tab* m_tab;
		TabBar* m_tabBar;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			f32 dstTabWidth = Constants::Defaults::Notebook::TabBar::TabViewAnimStartWidth;
			f32 offset = dstTabWidth - m_tab->getTabView()->getSize().width;
			setSpeed(offset / Constants::Defaults::Notebook::TabBar::NewTabAnimDuration);
			setLength(offset);
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			TabView* tabView = m_tab->getTabView();
			Container* tabContainer = tabView->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_tab->getIndex() + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaValue);
			tabView->extendRight(deltaValue);
			LayoutAttributes attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
			tabView->setLayoutAttributes(attr);
		}
		virtual void onEnd(com::Bool isAborted) noexcept override
		{
			m_tabBar->destroyTab(m_tab);
			AnimContextBase::onEnd(isAborted);
		}
	public:
		TabRemoveAnimation(UIDriver& driver, TabAnimGroup* group, Tab* tab, TabBar* tabBar) noexcept : AnimContextBase(driver, group), m_tab(tab), m_tabBar(tabBar) { }
	};

	class TabShiftAnimation : public AnimationEngine::AnimContextBase
	{
		friend class TabAnimGroup;
		friend class TabShiftAnimGroup;
	private:
		Tab* m_tab;
		com::Bool m_isLeft;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			f32 offset = m_tab->getPos().x - m_tab->getTabView()->getPosition().x;
			std::cout << "offset: " << offset << std::endl;
			setSpeed(offset / Constants::Defaults::Notebook::TabBar::TabShiftAnimDuration);
			setLength(offset);
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			m_tab->getTabView()->moveRight(deltaValue);
		}
		virtual void onEnd(com::Bool isAborted) noexcept override
		{
			std::cout << "Animation Ended" << std::endl;
			AnimContextBase::onEnd(isAborted);
		}
	public:
		TabShiftAnimation(UIDriver& driver, TabShiftAnimGroup* group, Tab* tab, com::Bool isLeft) noexcept : AnimContextBase(driver, group), m_tab(tab), m_isLeft(isLeft) { }
		com::Bool isLeft() const noexcept { return m_isLeft; }
		com::Bool isRight() const noexcept { return !m_isLeft; }
	};

	void TabAnimGroup::onPresent(AnimationEngine::AnimContextBase* animContext) noexcept
	{
		if(auto insertAnim = dynamic_cast<TabInsertAnimation*>(animContext))
		{
			Tab* insertTab = insertAnim->m_tab;
			for(TabRemoveAnimation* removeAnim : m_inFlightRemoves)
				if(removeAnim->m_tab->m_index >= insertTab->m_index)
				{
					removeAnim->m_tab->m_index += 1;
					break;
				}
			m_inFlightInserts.push_back(insertAnim);
		}
		else if(auto removeAnim = dynamic_cast<TabRemoveAnimation*>(animContext))
		{
			Tab* removeTab = removeAnim->m_tab;
			for(TabInsertAnimation* insertAnim : m_inFlightInserts)
				if(insertAnim->m_tab == removeTab)
				{
					insertAnim->abort();
					break;
				}
			m_inFlightRemoves.push_back(removeAnim);
		}
	}
	void TabAnimGroup::onAbsent(AnimationEngine::AnimContextBase* animContext) noexcept
	{
		if(auto insertAnim = dynamic_cast<TabInsertAnimation*>(animContext))
			com::find_erase(m_inFlightInserts, insertAnim);
		else if(auto removeAnim = dynamic_cast<TabRemoveAnimation*>(animContext))
			com::find_erase(m_inFlightRemoves, removeAnim);
	}
	void TabAnimGroup::onWhenAnyStart() noexcept
	{
		std::cout << "Any started" << std::endl;
		if(!m_tabBar->isLockedLayout())
			m_tabBar->lockLayout();
	}
	void TabAnimGroup::onWhenAllEnd() noexcept
	{
		std::cout << "All ended" << std::endl;
		m_tabBar->unlockLayout(true);
	}
	TabAnimGroup::TabAnimGroup(UIDriver& driver, TabBar* tabBar) noexcept : AnimGroup(driver), m_tabBar(tabBar)
	{

	}

	ScrollTabAnimGroup::ScrollTabAnimGroup(UIDriver& driver, ScrollableTabBar* tabBar) noexcept : TabAnimGroup(driver, tabBar), m_scrollableTabBar(tabBar)
	{

	}
	void ScrollTabAnimGroup::onStepAll() noexcept
	{
		auto* tabBar = m_scrollableTabBar;
		// onStepAll() is called even for the last (only one is left) stepped animation and that animation might destroy the only tab left in the TabBar
		// Causing tabBar->getSelectedTab() to return nullptr, so we need to check for null here.
		if(tabBar->getSelectedTab())
			tabBar->scrollToTab(tabBar->getSelectedTab());
	}

	void TabShiftAnimGroup::onPresent(AnimationEngine::AnimContextBase* animContext) noexcept
	{
		auto anim2 = com::iknow_down_cast<TabShiftAnimation*>(animContext);
		auto& anims = getAnims();
		for(auto& anim : anims)
		{
			auto anim1 = com::iknow_down_cast<TabShiftAnimation*>(anim);
			if((anim2->m_tab == anim1->m_tab) && (anim != animContext))
			{
				anim1->abort();
				break;
			}
		}
	}
	void TabShiftAnimGroup::onAbsent(AnimationEngine::AnimContextBase* animContext) noexcept
	{

	}
	TabShiftAnimGroup::TabShiftAnimGroup(UIDriver& driver, TabBar* tabBar) noexcept : AnimGroup(driver), m_tabBar(tabBar), m_isAborting(com::False)
	{

	}
	void TabShiftAnimGroup::abort() noexcept
	{
		auto& animations = getAnims();
		animations.traverse([](AnimationEngine::AnimContextBase* anim) noexcept
		{
			anim->abort();
		});
	}

	void Tab::setPage(NotebookPage* page) noexcept
	{
		m_page = page;
		if(page)
			page->m_tab = this;
	}
	void Tab::setLabel(const std::string_view str) noexcept
	{
		com_assert(COM_DESCRIPTION(!m_tabBar->isLockedLayout()), "Undefined behaviour, You can't change label of a tab while some tab animations are going on (the layout is locked)");
		// NOTE: Calling setLabel() resizes the tabView to fit the label, which results in layout recalculation
		// Since, we call setLayoutAttributes() further, it would trigger another layout recalculation.
		// Therefore, we are better off locking the layout first and then we are finished, unlock it at the end.
		m_tabBar->lockLayout();
		m_tabView->setLabel(str);
		LayoutAttributes attr = m_tabView->getLayoutAttributes();
		attr.prefSize.width = std::max(Constants::Defaults::Notebook::Tab::MinWidth, m_tabView->getSize().width);
		attr.minSize.width = attr.prefSize.width;
		m_tabView->setLayoutAttributes(attr);
		m_tabBar->unlockLayout(true);
	}

	TabBar::TabBar(UIDriver& driver, Container* parent, std::optional<TabAnimGroup*> tabAnimGroup) noexcept : HBoxContainer(driver, parent), 
																GlobalMouseMoveHandlerObject(driver), 
																m_root(com::null_pointer<Tab>()), 
																m_selectedTab(com::null_pointer<Tab>())
	{
		if(tabAnimGroup)
			m_tabAnimGroup = tabAnimGroup.value();
		else
		{
			m_isTabAnimGroupOwner = com::True;
			m_tabAnimGroup = driver.getAnimationEngine().createAnimGroup<TabAnimGroup>(this);
		}
		m_tabShiftAnimGroup = driver.getAnimationEngine().createAnimGroup<TabShiftAnimGroup>(this);
		this->GlobalMouseMoveHandlerObject::sleep();
	}

	TabBar::~TabBar() noexcept
	{
		// WARN: lockLayout() is important here!
		// Lock the layout to avoid unnecessary layout calculations
		// It also avoids accessing the already destroyed Tab objects
		lockLayout();
		com::TraverseLinkedList(getRootTab(), [](Tab* tab) noexcept
		{
			auto* tabView = tab->getTabView();
			tabView->getUIDriver().destroyContainer<TabView>(tabView);
			delete tab;
		});
		if(m_isTabAnimGroupOwner)
			getUIDriver().getAnimationEngine().destroyAnimGroup<TabAnimGroup>(m_tabAnimGroup);
		getUIDriver().getAnimationEngine().destroyAnimGroup<TabShiftAnimGroup>(m_tabShiftAnimGroup);
	}

	void TabBar::onRecalculateLayout() noexcept
	{
		HBoxContainer::onRecalculateLayout();
		if(!getRootTab())
			return;
		// Sync position of Tab with that of TabView
		com::TraverseLinkedListBiDirect(getRootTab(), [](auto node) noexcept
		{
			node->m_pos = node->getTabView()->getPosition();
		});
	}

	void TabBar::onMouseMove(Vec2Df pos) noexcept
	{
		TabView* tabView = m_tabRearrangeContext.grabbedTab->getTabView();
		if(!m_tabRearrangeContext.isMoved)
		{
			m_tabRearrangeContext.layer = tabView->getLayer();
			tabView->setLayer(MaxLayer);
			onTabPullOut(m_tabRearrangeContext.grabbedTab);
			m_tabRearrangeContext.isMoved = com::True;
		}
		updateGrabbedTabViewPos();
		checkForTabSwap();
	}

	void TabBar::dispatchAnimNewTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabInsertAnimation>(m_tabAnimGroup, tab);
	}

	void TabBar::dispatchAnimRemoveTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabRemoveAnimation>(m_tabAnimGroup, tab, this);
	}

	void TabBar::swapWithNext(Tab* tab) noexcept
	{
		auto nextTab = tab->getNext();
		std::cout << "Swaping: " << tab->m_pos << ", " << nextTab->m_pos << std::endl;
		nextTab->m_pos = tab->m_pos;
		tab->m_pos.x += nextTab->getTabView()->getSize().width;
		std::swap(tab->m_index, nextTab->m_index);
		swapChildren(tab->m_index, nextTab->m_index);
		com::RemoveLinkedListNode(tab);
		com::InsertLinkedListNodeAfter(tab, /* After: */ nextTab);
	}

	void TabBar::swapWithPrev(Tab* tab) noexcept
	{
		swapWithNext(tab->getPrev());
	}

	void TabBar::updateGrabbedTabViewPos() noexcept
	{
		TabView* tabView = m_tabRearrangeContext.grabbedTab->getTabView();
		auto pos = m_tabRearrangeContext.positionOffset + tabView->getParent()->getScreenCoordsToLocalCoords(getUIDriver().getInputDriver().getMousePosition());
		tabView->setPosition(pos);
	}

	void TabBar::checkForTabSwap() noexcept
	{
		Tab* tab = m_tabRearrangeContext.grabbedTab;
		TabView* tabView = tab->getTabView();
		Tab* next = tab->getNext();
		auto pos = tabView->getPosition();
		if(next)
		{
			auto nextPosX = next->getPos().x;
			auto nextWidth = next->getTabView()->getSize().width;
			f32 middle = nextPosX + nextWidth * 0.5f + 0.1f;
			if(((pos.x + tabView->getSize().width) > middle) && (pos.x < (nextPosX + nextWidth)))
			{
				std::cout << "Swapping with next tab" << std::endl;
				swapWithNext(tab);
				getUIDriver().getAnimationEngine().dispatchAnimation<TabShiftAnimation>(m_tabShiftAnimGroup, next, /* isLeft: */ com::True);
			}
		}
		Tab* prev = tab->getPrev();
		if(prev)
		{
			auto prevPosX = prev->getPos().x;
			f32 middle = prevPosX + prev->getTabView()->getSize().width * 0.5f - 0.1f;
			if((pos.x < middle) && (pos.x > prevPosX))
			{
				std::cout << "Swapping with prev tab" << std::endl;
				swapWithPrev(tab);
				getUIDriver().getAnimationEngine().dispatchAnimation<TabShiftAnimation>(m_tabShiftAnimGroup, prev, /* isLeft: */ com::False);
			}
		}
	}

	TabView* TabBar::createTabView(Tab* tab) noexcept
	{
		auto* tabView = getUIDriver().createContainer<TabView>(com::null_pointer<Container>());
		tab->m_tabView = tabView;
		tabView->getOnPressEvent().subscribe([this, tab](SUTK::Button* button) noexcept
		{
			this->selectTab(tab);

			this->m_tabRearrangeContext.grabbedTab = tab;
			Vec2Df mousePos = getUIDriver().getInputDriver().getMousePosition();
			this->m_tabRearrangeContext.positionOffset = tab->getTabView()->getGlobalPosition() - mousePos;
			this->m_tabRearrangeContext.isMoved = com::False;
			this->lockLayout();
			
			this->GlobalMouseMoveHandlerObject::awake();
		});
		tabView->getOnReleaseEvent().subscribe([this, tab](SUTK::Button* button) noexcept
		{
			_com_assert(this->m_tabRearrangeContext.grabbedTab != com::null_pointer<Tab>());
			if(this->m_tabRearrangeContext.isMoved)
			{
				this->m_tabRearrangeContext.grabbedTab->getTabView()->setLayer(m_tabRearrangeContext.layer);
				this->onTabPutBack(this->m_tabRearrangeContext.grabbedTab);
				// NOTE: abort should be called first to get the final changes, 
				// otherwise, GetLinkedListRoot() would return wrong result.
				this->m_tabShiftAnimGroup->abort();
				this->m_root = GetLinkedListRoot(this->m_tabRearrangeContext.grabbedTab);
			}
			this->m_tabRearrangeContext.grabbedTab = com::null_pointer<Tab>();
			this->unlockLayout(static_cast<bool>(this->m_tabRearrangeContext.isMoved));
			this->GlobalMouseMoveHandlerObject::sleep();
		});
		tabView->getCloseButton()->getOnReleaseEvent().subscribe([this, tab](Button* button) noexcept
		{
			this->removeTab(tab);
		});
		return tabView;
	}

	Tab* TabBar::createTab(const std::string_view labelStr, Tab* after) noexcept
	{
		// If the supplied tab is null_pointer then create the tab after the currently selected tab.
		if(!after)
			after = m_selectedTab;
		lockLayout();
		Tab* tab = new Tab();
		tab->m_tabBar = this;
		TabView* tabView = createTabView(tab);
		if(after)
		{
			com::InsertLinkedListNodeAfter(tab, after);
			tab->m_index = after->getIndex() + 1;
			auto node = tab->getNext();
			while(node)
			{
				node->m_index += 1;
				node = node->getNext();
			}
		}
		else
			tab->m_index = 0;
		if(!tab->getPrev())
			m_root = tab;
		addAt(tabView, tab->m_index);
		// Unlock the layout but do not recalculate, because we are further calling setLabel() which does recalculates at the end
		unlockLayout();
		tab->setLabel(labelStr);
		// <Begin> Initialize Animation Start state and Setup Animation Context
		dispatchAnimNewTab(tab);
		// <End>
		return tab;
	}
	void TabBar::destroyTab(Tab* tab) noexcept
	{
		auto node = tab->getNext();
		while(node)
		{
			node->m_index -= 1;
			node = node->getNext();
		}
		getUIDriver().destroyContainer<TabView>(tab->m_tabView);
		delete tab;
	}
	static Tab* GetAdjacentTab(Tab* tab) noexcept
	{
		return tab->getPrev() ? tab->getPrev() : tab->getNext();
	}
	void TabBar::removeTab(Tab* tab) noexcept
	{
		// Select an adjacent tab if exists
		if(auto* adjTab = GetAdjacentTab(tab))
			selectTab(adjTab);

		m_onRemoveEvent.publish(tab);

		dispatchAnimRemoveTab(tab);

		// If a Selected tab is being removed then we must set m_selectedTab to null
		if(tab == m_selectedTab)
			m_selectedTab = com::null_pointer<Tab>();
		if(!tab->getPrev())
			m_root = tab->getNext();
		com::RemoveLinkedListNode(tab, /* Do not set next and prev ptrs to NULL */ com::False);
	}
	void TabBar::selectTab(Tab* tab) noexcept
	{
		// Deactivate the current page
		if(m_selectedTab != com::null_pointer<Tab>())
		{
			// Move the associated tabView into unselected state
			m_selectedTab->getTabView()->unselectedState();
			m_selectedTab->getPage()->setActive(com::False);
		}
		// Activate the requested page
		if(tab)
		{
			m_selectedTab = tab;
			// Move the associated tabView into selected state
			tab->getTabView()->selectedState();
			// Activate the requested page's container
			tab->getPage()->setActive(com::True);
		}
		m_onSelectEvent.publish(tab);
	}

	ScrollableTabBar::ScrollableTabBar(UIDriver& driver, Container* parent) noexcept : BaseType(driver, parent,
											driver.getAnimationEngine().createAnimGroup<ScrollTabAnimGroup>(this)),
											Runnable(driver)
	{

	}

	ScrollableTabBar::~ScrollableTabBar() noexcept
	{
		auto* animGroup = dynamic_cast<ScrollTabAnimGroup*>(getTabAnimGroup());
		_com_assert(!com::is_nullptr(animGroup));
		getUIDriver().getAnimationEngine().destroyAnimGroup<ScrollTabAnimGroup>(animGroup);
	}

	void ScrollableTabBar::scrollToTab(Tab* tab) noexcept
	{
		ScrollContainer* scrollCont = getScrollContainer();
		// Get rectangle of TabView in the local space of ScrollContainer
		auto rect = tab->getTabView()->getRectRelativeTo(scrollCont);
		// If the TabView is hiding on the right side of the ScrollContainer then scroll to left
		if(f32 distance = rect.getRight() - scrollCont->getSize().width; distance > 0)
			scrollCont->addScrollDelta(Vec2Df::left() * distance);
		// else If the TabView is hiding on the left side of the ScrollContainer then scroll to right
		else if(f32 distance = rect.getLeft() - scrollCont->getPosition().x; distance < 0)
			scrollCont->addScrollDelta(Vec2Df::left() * distance);
	}

	void ScrollableTabBar::onMouseMove(Vec2Df pos) noexcept
	{
		BaseType::onMouseMove(pos);
		Tab* tab = getGrabbedTab();
		_com_assert(tab != com::null_pointer<Tab>());
		auto* scrollCont = getScrollContainer();
		auto rect = tab->getTabView()->getRectRelativeTo(scrollCont);
		com::Bool isLeft { (rect.x < 0) && scrollCont->getScrollDelta().x };
		com::Bool isRight { (rect.getRight() > scrollCont->getSize().width) && (getRect().getRight() > scrollCont->getSize().width) };
		m_isAutoScroll = isLeft ^ isRight;
		if(m_isAutoScroll)
			m_autoScrollDir = isLeft ? 1 : -1;
	}

	void ScrollableTabBar::onTabPullOut(Tab* tab) noexcept
	{
		restoreMaskFor(tab->getTabView());
	}

	void ScrollableTabBar::onTabPutBack(Tab* tab) noexcept
	{
		updateMaskFor(tab->getTabView());
		m_isAutoScroll = com::False;
	}

	bool ScrollableTabBar::isRunning()
	{
		return static_cast<bool>(m_isAutoScroll);
	}

	void ScrollableTabBar::update()
	{
		auto* scrollCont = getScrollContainer();
		if(scrollCont->isScrollableHorziontal(m_autoScrollDir))
		{
			auto dt = getUIDriver().getDeltaTime();
			auto delta = Vec2Df::right() * m_autoScrollDir * dt * 5.0f;
			scrollCont->addScrollDelta(delta);
			updateGrabbedTabViewPos();
			checkForTabSwap();
		}
		else m_isAutoScroll = com::False;
	}

	Tab* NotebookView::createTab(const std::string_view labelStr, NotebookPage* page, Tab* afterTab) noexcept
	{
		// Create TabView
		Tab* tab = getTabBar()->createTab(labelStr, afterTab);
		tab->setPage(page);
		return tab;
	}

	Container* NotebookView::createPageContainer() noexcept
	{
		Container* container = getUIDriver().createContainer<Container>(m_pageContainerParent);
		container->alwaysFitInParent();
		container->setActive(com::False);
		return container;
	}

	void NotebookView::viewPage(NotebookPage* page) noexcept
	{
		m_tabBar->selectTab(page->getTab());
		// The tab associated with this page must be in the view
		m_tabBar->scrollToTab(page->getTab());
	}

	void NotebookView::removePage(NotebookPage* page) noexcept
	{
		if(!page)
		{
			debug_log_warning("(null) value is passed, no page/tab is removed");
			return;
		}
		getTabBar()->removeTab(page->getTab());
		dump();
	}

	void NotebookView::dump() noexcept
	{
		std::cout << "Dump (Notebook): \n"
				  << "Current Selected: " << (getCurrentPage() ? getCurrentPage()->getTab()->getTabView()->getLabel() : "<null>") << "\n";
		NotebookPage* page = getRootPage();
		while(page != com::null_pointer<NotebookPage>())
		{
			std::cout << page->getTab()->getTabView()->getLabel() << ", index = " << page->getTab()->getIndex() << std::endl;
			page = page->getNext();
		}
	}
}