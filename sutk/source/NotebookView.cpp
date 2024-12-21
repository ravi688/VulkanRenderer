#include <sutk//NotebookView.hpp>
#include <sutk/TabBarView.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/TextGroupContainer.hpp>
#include <sutk/ContainerUtility.hpp>
#include <sutk/ButtonGraphic.hpp> // for SUTK::ImageButtonGraphic
#include <sutk/Panel.hpp> // for SUTK::Panel

#define TAB_VIEW_MIN_WIDTH 3.0f
#define TAB_BAR_HEIGHT 0.8f
#define TAB_LABEL_LEFT_MARGIN 0.2f
#define TAB_LABEL_TOP_MARGIN 0.2f
#define TAB_LABEL_BOTTOM_MARGIN 0.2f
#define TAB_LABEL_CLOSE_BUTTON_SPACING 0.1f
#define CLOSE_BUTTON_RIGHT_MARGIN 0.2f
#define CLOSE_BUTTON_WIDTH 0.5f
#define CLOSE_BUTTON_HEIGHT 0.5f

#define TAB_VIEW_MIDDLE_SPREAD 0.1f

#define TAB_VIEW_ANIM_START_WIDTH 0.0f
#define DEFAULT_NEW_TAB_ANIM_DURATION 0.28f
#define TAB_SHIFT_ANIM_DURATION 0.2f

namespace SUTK
{

	NotebookPage::NotebookPage(Container* container) noexcept : m_data(NULL), m_tab(com::null_pointer<Tab>()), m_container(container) { }
	NotebookPage::~NotebookPage() noexcept
	{
		if(m_data != com::null_pointer<void>())
		{
			(*m_dataDeleter)(m_data);
			m_data = com::null_pointer<void>();
			m_dataDeleter = NULL;
		}
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

	UIDriver::ImageReference TabView::s_closeIcon = UIDriver::InvalidImage;

	TabView::TabView(UIDriver& driver, Container* parent) noexcept : Button(driver, parent, /* isCreateDefaultGraphic: */ true)
	{
		m_graphic = getGraphicAs<DefaultButtonGraphic>();
		Label& label = m_graphic->getLabel();
		label.setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
		label.fitInParent({ TAB_LABEL_LEFT_MARGIN, TAB_LABEL_CLOSE_BUTTON_SPACING + CLOSE_BUTTON_RIGHT_MARGIN + CLOSE_BUTTON_WIDTH, TAB_LABEL_TOP_MARGIN, TAB_LABEL_BOTTOM_MARGIN  });
		m_graphic->getAnchorRect()->fitToParentRect();

		m_closeButton = driver.createContainer<Button>(this, /* isCreateDefaultGraphic: */ false);
		m_closeButton->setRect({ getSize().width - CLOSE_BUTTON_WIDTH - CLOSE_BUTTON_RIGHT_MARGIN, 
								(getSize().height - CLOSE_BUTTON_HEIGHT) * 0.5f,
								 CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT });
		m_closeButton->getAnchorRect()->moveToRightMiddleOfParent();
		m_closeButtonGraphic = driver.createContainer<ImageButtonGraphic>(m_closeButton);
		if(!s_closeIcon)
			s_closeIcon = driver.loadImage("svg_files/close-square-svgrepo-com.svg");
		m_closeButtonGraphic->setImage(s_closeIcon);
	}

	TabView::~TabView() noexcept
	{
		auto& driver = getUIDriver();
		driver.destroyContainer<ImageButtonGraphic>(m_closeButtonGraphic);
		driver.destroyContainer<Button>(m_closeButton);
	}

	void TabView::setLabel(const std::string_view str) noexcept
	{
		m_graphic->getLabel().set(str);
		SUTK::Vec2Df size = m_graphic->getMinBoundSize();
		setSize({ size.width + TAB_LABEL_CLOSE_BUTTON_SPACING + CLOSE_BUTTON_RIGHT_MARGIN + CLOSE_BUTTON_WIDTH, getSize().height });
	}

	const std::string& TabView::getLabel() noexcept
	{
		return m_graphic->getLabel().get();
	}

	void TabView::unselectedState() noexcept
	{

	}

	void TabView::selectedState() noexcept
	{
		
	}

	NotebookView::NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : VBoxContainer(driver, parent, /* isLockLayout: */ true, isLayoutIgnore, layer), GlobalMouseMoveHandlerObject(driver),
																																					m_currentPage(com::null_pointer<NotebookPage>()),
																																					m_onPageSelectEvent(this),
																																					m_isRunning(com::False),
																																					m_isStartAnimBatch(com::False),
																																					m_animDuration(DEFAULT_NEW_TAB_ANIM_DURATION)
	{
		// Create Tab Container
		// NOTE: the 'getDepth() + 10000' is needed to keep the tabs always on top of the pages (pages can be also be scrolled up and overlap with the tabs!)
		m_textGroupContainer = driver.createContainer<TextGroupContainer>(this, com::False, getDepth() + 10000);
		LayoutAttributes attr = m_textGroupContainer->getLayoutAttributes();
		attr.minSize.height = TAB_BAR_HEIGHT;
		attr.prefSize = attr.minSize;
		m_textGroupContainer->setLayoutAttributes(attr);
		m_tabBarBGPanel = driver.createContainer<Panel>(m_textGroupContainer);
		m_tabBarBGPanel->alwaysFitInParent();
		m_tabBarBGPanel->setColor(Color4::grey(0.8f));
		m_tabBar = driver.createContainer<TabBar>(m_tabBarBGPanel);
		m_tabBar->alwaysFitInParent();

		m_tabAnimGroup = driver.getAnimationEngine().createAnimGroup<TabAnimGroup>(this);
		m_tabShiftAnimGroup = driver.getAnimationEngine().createAnimGroup<TabShiftAnimGroup>(this);

		// Create Container for holding page container
		m_pageContainer = driver.createContainer<Panel>(this);
		attr = m_pageContainer->getLayoutAttributes();
		attr.prefSize = attr.maxSize;
		_com_assert((attr.prefSize.height == F32_INFINITY) && (attr.prefSize.width == F32_INFINITY));
		m_pageContainer->setLayoutAttributes(attr);
		m_pageContainer->setColor(Color4::grey(0.6f));

		unlockLayout();
		this->GlobalMouseMoveHandlerObject::sleep();
	}

	NotebookView::~NotebookView() noexcept
	{
		getUIDriver().destroyContainer<Panel>(m_tabBarBGPanel);
		getUIDriver().getAnimationEngine().destroyAnimGroup<TabAnimGroup>(m_tabAnimGroup);
		getUIDriver().getAnimationEngine().destroyAnimGroup<TabShiftAnimGroup>(m_tabShiftAnimGroup);
		delete m_tabAnimGroup;
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
			f32 offset = dstTabWidth - TAB_VIEW_ANIM_START_WIDTH;
			setSpeed(offset / DEFAULT_NEW_TAB_ANIM_DURATION);
			setLength(offset);
			Container* tabContainer = tabView->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_tab->getIndex() + 1; i < childs.size(); ++i)
				childs[i]->moveLeft(offset);
			tabView->setSize({ TAB_VIEW_ANIM_START_WIDTH, tabView->getSize().height });
			LayoutAttributes& attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			TabView* tabView = m_tab->getTabView();
			Container* tabContainer = tabView->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_tab->getIndex() + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaValue);
			tabView->extendRight(deltaValue);
			LayoutAttributes& attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
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
			f32 dstTabWidth = TAB_VIEW_ANIM_START_WIDTH;
			f32 offset = dstTabWidth - m_tab->getTabView()->getSize().width;
			setSpeed(offset / DEFAULT_NEW_TAB_ANIM_DURATION);
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
			LayoutAttributes& attr = tabView->getLayoutAttributes();
			attr.prefSize = tabView->getSize();
		}
		virtual void onEnd(com::Bool isAborted) noexcept override
		{
			m_tabBar->destroyTab(m_tab);
			AnimContextBase::onEnd(isAborted);
		}
	public:
		TabRemoveAnimation(UIDriver& driver, TabAnimGroup* group, Tab* tab, TabBar* tabBar) noexcept : AnimContextBase(driver, group), m_tab(tab), m_tabBar(tabBar) { }
	};

	template<typename T>
	static void RemoveLinkedListNode(T* node, com::Bool isSetNull = com::True) noexcept
	{
		if(node->getPrev())
			node->getPrev()->setNext(node->getNext());
		if(node->getNext())
			node->getNext()->setPrev(node->getPrev());
		if(isSetNull)
		{
			node->setNext(com::null_pointer<T>());
			node->setPrev(com::null_pointer<T>());
		}
	}

	template<typename T>
	static void InsertLinkedListNodeAfter(T* node, T* after) noexcept
	{
		if(auto afterNext = after->getNext())
		{
			afterNext->setPrev(node);
			node->setNext(afterNext);
		}
		node->setPrev(after);	
		after->setNext(node);
	}

	template<typename T>
	static void InsertLinkedListNodeBefore(T* node, T* before) noexcept
	{
		if(auto beforePrev = before->getPrev())
		{
			beforePrev->setNext(node);
			node->setPrev(beforePrev);
		}
		node->setNext(before);
		before->setPrev(node);
	}

	template<typename T>
	u32 GetLinkedListNodeIndex(const T* node) noexcept
	{
		u32 index = 0;
		node = node->getPrev();
		while(node)
		{
			++index;
			node = node->getPrev();
		}
		return index;
	}

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
			setSpeed(offset / TAB_SHIFT_ANIM_DURATION);
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
		if(!m_notebook->getTabBar()->isLockedLayout())
			m_notebook->getTabBar()->lockLayout();
	}
	void TabAnimGroup::onWhenAllEnd() noexcept
	{
		std::cout << "All ended" << std::endl;
		m_notebook->getTabBar()->unlockLayout(true);
	}
	TabAnimGroup::TabAnimGroup(UIDriver& driver, NotebookView* notebook) noexcept : AnimGroup(driver), m_notebook(notebook)
	{

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
	TabShiftAnimGroup::TabShiftAnimGroup(UIDriver& driver, NotebookView* notebook) noexcept : AnimGroup(driver), m_notebook(notebook), m_isAborting(com::False)
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

	void NotebookView::swapWithNext(Tab* tab) noexcept
	{
		auto nextTab = tab->getNext();
		std::cout << "Swaping: " << tab->m_pos << ", " << nextTab->m_pos << std::endl;
		nextTab->m_pos = tab->m_pos;
		tab->m_pos.x += nextTab->getTabView()->getSize().width;
		std::swap(tab->m_index, nextTab->m_index);
		getTabBar()->swapChildren(tab->m_index, nextTab->m_index);
		RemoveLinkedListNode(tab);
		InsertLinkedListNodeAfter(tab, /* After: */ nextTab);
	}

	void NotebookView::swapWithPrev(Tab* tab) noexcept
	{
		swapWithNext(tab->getPrev());
	}

	void NotebookView::checkForTabSwap() noexcept
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

	void NotebookView::onMouseMove(Vec2Df pos) noexcept
	{
		TabView* tabView = this->m_tabRearrangeContext.grabbedTab->getTabView();
		if(!m_tabRearrangeContext.isMoved)
		{
			m_tabRearrangeContext.layer = tabView->getLayer();
			tabView->setLayer(MaxLayer);
			m_tabRearrangeContext.isMoved = com::True;
		}
		pos = m_tabRearrangeContext.positionOffset + tabView->getParent()->getScreenCoordsToLocalCoords(pos);
		tabView->setPosition(pos);
		checkForTabSwap();
	}

	void NotebookView::dispatchAnimNewTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabInsertAnimation>(m_tabAnimGroup, tab);
	}

	void NotebookView::dispatchAnimRemoveTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabRemoveAnimation>(m_tabAnimGroup, tab, getTabBar());		// if(m_isRunning)
	}

	template<typename T>
	static T* GetLinkedListRoot(T* node) noexcept
	{
		while(node && node->getPrev())
			node = node->getPrev();
		return node;
	}

	template<typename T, com::concepts::UnaryVisitor<T*> VisitorType>
	static void TraverseLinkedListBiDirect(T* node, VisitorType visitor) noexcept
	{
		auto left = node->getPrev();
		while(left)
		{
			visitor(left);
			left = left->getPrev();
		}
		while(node)
		{
			visitor(node);
			node = node->getNext();
		}
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
		attr.minSize.width = TAB_VIEW_MIN_WIDTH;
		attr.prefSize.width = std::max(TAB_VIEW_MIN_WIDTH, m_tabView->getSize().width);
		m_tabView->setLayoutAttributes(attr);
		m_tabBar->unlockLayout(true);
	}

	TabBar::TabBar(UIDriver& driver, Container* parent) noexcept : HBoxContainer(driver, parent), m_root(com::null_pointer<Tab>())
	{
	}

	void TabBar::onRecalculateLayout() noexcept
	{
		HBoxContainer::onRecalculateLayout();
		if(!getRootTab())
			return;
		// Sync position of Tab with that of TabView
		TraverseLinkedListBiDirect(getRootTab(), [](auto node) noexcept
		{
			node->m_pos = node->getTabView()->getPosition();
		});
	}
	Tab* TabBar::createTab(const std::string_view labelStr, Tab* after) noexcept
	{
		lockLayout();
		Tab* tab = new Tab();
		tab->m_tabBar = this;
		TabView* tabView = getUIDriver().createContainer<TabView>(com::null_pointer<Container>());
		tab->m_tabView = tabView;
		if(after)
		{
			InsertLinkedListNodeAfter(tab, after);
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
	void TabBar::removeTab(Tab* tab) noexcept
	{
		if(!tab->getPrev())
			m_root = tab->getNext();
		RemoveLinkedListNode(tab, /* Do not set next and prev ptrs to NULL */ com::False);
	}

	NotebookPage* NotebookView::createPage(const std::string_view labelStr, NotebookPage* afterPage) noexcept
	{
		// If the supplied page is null_pointer then create the page after the current being viewed page.
		if(!afterPage)
			afterPage = m_currentPage;

		// Create Container for the page
		Container* container = getUIDriver().createContainer<Container>(m_pageContainer);
		container->alwaysFitInParent();
		container->setActive(com::False);

		// Create TabView for the page
		Tab* tab = getTabBar()->createTab(labelStr, afterPage ? afterPage->getTab() : com::null_pointer<Tab>());
		// Create Page
		NotebookPage* page = new NotebookPage(container);
		tab->setPage(page);

		// <Begin> Initialize Animation Start state and Setup Animation Context
		dispatchAnimNewTab(tab);
		// <End>

		TabView* tabView = tab->getTabView();

		tabView->getOnPressEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			this->viewPage(page);
			this->m_onPageSelectEvent.publish(page);

			this->m_tabRearrangeContext.grabbedTab = page->getTab();
			Vec2Df mousePos = getUIDriver().getInputDriver().getMousePosition();
			this->m_tabRearrangeContext.positionOffset = page->getTab()->getTabView()->getGlobalPosition() - mousePos;
			this->m_tabRearrangeContext.isMoved = com::False;
			this->getTabBar()->lockLayout();
			
			this->GlobalMouseMoveHandlerObject::awake();
		});
		tabView->getOnReleaseEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			_com_assert(this->m_tabRearrangeContext.grabbedTab != com::null_pointer<Tab>());
			if(this->m_tabRearrangeContext.isMoved)
			{
				this->m_tabRearrangeContext.grabbedTab->getTabView()->setLayer(m_tabRearrangeContext.layer);
				// NOTE: abort should be called first to get the final changes, 
				// otherwise, GetLinkedListRoot() would return wrong result.
				this->m_tabShiftAnimGroup->abort();
				this->m_tabBar->m_root = GetLinkedListRoot(this->m_tabRearrangeContext.grabbedTab);
			}
			this->m_tabRearrangeContext.grabbedTab = com::null_pointer<Tab>();
			this->getTabBar()->unlockLayout(static_cast<bool>(this->m_tabRearrangeContext.isMoved));
			this->GlobalMouseMoveHandlerObject::sleep();
			this->dump();
		});

		viewPage(page);

		dump();

		return page;
	}

	void NotebookView::viewPage(NotebookPage* page) noexcept
	{
		// Deactivate the current page
		if(m_currentPage != com::null_pointer<NotebookPage>())
		{
			Container* container = m_currentPage->getContainer();
			// Deactivate the current page's container
			if(container->isActive())
				ContainerUtility::SetActiveAllRecursive(container, com::False);
			// Move the associated tabView into unselected state
			TabView* tabView = m_currentPage->getTab()->getTabView();
			tabView->unselectedState();
		}
		
		// Active the requested page
		if(page)
		{
			// Activate the requested page's container
			Container* container = page->getContainer();
			if(!container->isActive())
				ContainerUtility::SetActiveAllRecursive(container, com::True);
			// Move the associated tabView into selected state
			TabView* tabView = page->getTab()->getTabView();
			tabView->selectedState();
		}

		m_currentPage = page;
	}

	void NotebookView::removePage(NotebookPage* page) noexcept
	{
		if(!page)
		{
			debug_log_warning("(null) value is passed, no page/tab is removed");
			return;
		}
		
		viewPage(page->getPrev() ? page->getPrev() : page->getNext());

		Tab* tab = page->getTab();
		getTabBar()->removeTab(tab);
		
		if(page->m_onPageRemove.has_value())
			page->m_onPageRemove.value() (page);
		getUIDriver().destroyContainer<Container>(page->m_container);
		delete page;
		tab->setPage(com::null_pointer<NotebookPage>());

		dispatchAnimRemoveTab(tab);

		dump();
	}

	void NotebookView::dump() noexcept
	{
		std::cout << "Dump (Notebook): \n"
				  << "Current Selected: " << (m_currentPage ? m_currentPage->getTab()->getTabView()->getLabel() : "<null>") << "\n";
		NotebookPage* page = getRootPage();
		while(page != com::null_pointer<NotebookPage>())
		{
			std::cout << page->getTab()->getTabView()->getLabel() << ", index = " << page->getTab()->getIndex() << std::endl;
			page = page->getNext();
		}
	}
}