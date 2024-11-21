#include <sutk//NotebookView.hpp>
#include <sutk/TabBarView.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/TextGroupContainer.hpp>
#include <sutk/ContainerUtility.hpp>
#include <sutk/ButtonGraphic.hpp> // for SUTK::ImageButtonGraphic
#include <sutk/HBoxContainer.hpp> // for SUTK::HBoxContainer

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
#define DEFAULT_NEW_TAB_ANIM_DURATION 2.5f
#define TAB_SHIFT_ANIM_DURATION 2.5f

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
		m_tab->getTabView()->setLabel(str);
	}

	const std::string& NotebookPage::getLabel() const noexcept
	{
		return m_tab->getTabView()->getLabel();
	}

	u32 NotebookPage::getIndex() const noexcept
	{
		return m_tab->getIndex();
	}

	TabView::TabView(UIDriver& driver, Container* parent) noexcept : Button(driver, parent, /* isCreateDefaultGraphic: */ true)
	{
		m_graphic = getGraphicAs<DefaultButtonGraphic>();
		Label& label = m_graphic->getLabel();
		label.setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
		label.fitInParent({ TAB_LABEL_LEFT_MARGIN, TAB_LABEL_CLOSE_BUTTON_SPACING + CLOSE_BUTTON_RIGHT_MARGIN + CLOSE_BUTTON_WIDTH, TAB_LABEL_TOP_MARGIN, TAB_LABEL_BOTTOM_MARGIN  });
		label.enableDebug();
		m_graphic->getAnchorRect()->fitToParentRect();
		m_graphic->setIdleColor(Color4::grey(0.05f));
		m_graphic->setHoverColor(Color4::grey(0.1f));
		m_graphic->setPressColor(Color4::grey(0.15f));
		m_graphic->setTransitionDelay(0.08f);

		m_closeButton = driver.createContainer<Button>(this, /* isCreateDefaultGraphic: */ false);
		m_closeButton->setRect({ getSize().width - CLOSE_BUTTON_WIDTH - CLOSE_BUTTON_RIGHT_MARGIN, 
								(getSize().height - CLOSE_BUTTON_HEIGHT) * 0.5f,
								 CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT });
		m_closeButton->getAnchorRect()->moveToRightMiddleOfParent();
		m_closeButtonGraphic = driver.createContainer<ImageButtonGraphic>(m_closeButton);
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
																																					m_head(com::null_pointer<NotebookPage>()),
																																					m_currentPage(com::null_pointer<NotebookPage>()),
																																					m_onPageSelectEvent(this),
																																					m_isRunning(com::False),
																																					m_isStartAnimBatch(com::False),
																																					m_animDuration(DEFAULT_NEW_TAB_ANIM_DURATION)
	{
		// Create Tab Container
		// NOTE: the 'getDepth() + 10000' is need to keep the tabs always on top of the pages (pages can be also be scrolled up and overlap with the tabs!)
		m_textGroupContainer = driver.createContainer<TextGroupContainer>(this, com::False, getDepth() + 10000);
		LayoutAttributes attr = m_textGroupContainer->getLayoutAttributes();
		attr.minSize.height = TAB_BAR_HEIGHT;
		attr.prefSize = attr.minSize;
		m_textGroupContainer->setLayoutAttributes(attr);
		m_tabContainer = driver.createContainer<HBoxContainer>(m_textGroupContainer);
		m_tabContainer->alwaysFitInParent();

		m_tabAnimGroup = driver.getAnimationEngine().createAnimGroup<TabAnimGroup>(this);
		m_tabShiftAnimGroup = driver.getAnimationEngine().createAnimGroup<TabShiftAnimGroup>(this);

		// Create Container for holding page container
		m_pageContainer = driver.createContainer<Container>(this);
		attr = m_pageContainer->getLayoutAttributes();
		attr.prefSize = attr.maxSize;
		_com_assert((attr.prefSize.height == F32_INFINITY) && (attr.prefSize.width == F32_INFINITY));
		m_pageContainer->setLayoutAttributes(attr);
		m_pageContainer->enableDebug(true, Color4::yellow());

		unlockLayout();
		this->GlobalMouseMoveHandlerObject::sleep();
	}

	NotebookView::~NotebookView() noexcept
	{
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
			delete m_tab;
			AnimContextBase::onEnd(isAborted);
		}
	public:
		TabRemoveAnimation(UIDriver& driver, TabAnimGroup* group, Tab* tab) noexcept : AnimContextBase(driver, group), m_tab(tab) { }
	};

	template<typename T>
	static void RemoveLinkedListNode(T* node) noexcept
	{
		if(node->getPrev())
			node->getPrev()->setNext(node->getNext());
		if(node->getNext())
			node->getNext()->setPrev(node->getPrev());
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
	private:
		Tab* m_tab;
		com::Bool m_isLeft;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			f32 offset = m_isLeft ? (m_tab->getPrev() ? -m_tab->getPrev()->getTabView()->getSize().width : 0) : (m_tab->getNext() ? m_tab->getNext()->getTabView()->getSize().width : 0);
			setSpeed(offset / TAB_SHIFT_ANIM_DURATION);
			setLength(offset);
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			m_tab->getTabView()->moveRight(deltaValue);
		}
		virtual void onEnd(com::Bool isAborted) noexcept override
		{
			Tab* swapTab;;
			if(m_isLeft)
			{
				_com_assert(m_tab->getPrev() != com::null_pointer<Tab>());
				swapTab = m_tab->getPrev();
				RemoveLinkedListNode(swapTab);
				InsertLinkedListNodeAfter(swapTab, /* After: */ m_tab);
			}
			else
			{
				_com_assert(m_tab->getNext() != com::null_pointer<Tab>());
				swapTab = m_tab->getNext();
				RemoveLinkedListNode(swapTab);
				InsertLinkedListNodeBefore(swapTab, /* Before: */ m_tab);
			}

			u32 index = GetLinkedListNodeIndex(m_tab);
			u32 swapIndex = m_isLeft ? (index + 1) : (index - 1);
			m_tab->m_index = index;
			swapTab->m_index = swapIndex;

			Container* tabContainer = m_tab->getTabView()->getParent();
			tabContainer->swapChildren(swapIndex, index);

			if(!isAborted)
				dynamic_cast<TabShiftAnimGroup*>(getAnimGroup())->getNotebook()->checkForTabSwap();

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
		if(!m_notebook->m_tabContainer->isLockedLayout())
			m_notebook->m_tabContainer->lockLayout();
	}
	void TabAnimGroup::onWhenAllEnd() noexcept
	{
		std::cout << "All ended" << std::endl;
		m_notebook->m_tabContainer->unlockLayout(true);
	}
	TabAnimGroup::TabAnimGroup(UIDriver& driver, NotebookView* notebook) noexcept : AnimGroup(driver), m_notebook(notebook)
	{

	}

	void TabShiftAnimGroup::onPresent(AnimationEngine::AnimContextBase* animContext) noexcept
	{

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

	void NotebookView::checkForTabSwap() noexcept
	{
		Tab* tab = m_tabRearrangeContext.grabbedTab;
		TabView* tabView = tab->getTabView();
		Tab* next = tab->getNext();
		auto pos = tabView->getPosition();
		if(next)
		{
			f32 middle = next->getTabView()->getPosition().x + next->getTabView()->getSize().width * 0.5f;
			if((pos.x + tabView->getSize().width) > middle)
			{
				std::cout << "Swapping with next tab" << std::endl;
				Tab* nextTab = next;
				tab->m_next = nextTab->m_next;
				if(nextTab->m_next)
					nextTab->m_next->m_prev = tab;
				getUIDriver().getAnimationEngine().dispatchAnimation<TabShiftAnimation>(m_tabShiftAnimGroup, nextTab, /* isLeft: */ com::True);
			}
		}
		Tab* prev = tab->getPrev();
		if(prev)
		{
			f32 middle = prev->getTabView()->getPosition().x + prev->getTabView()->getSize().width * 0.5f;
			if(pos.x < middle)
			{
				std::cout << "Swapping with prev tab" << std::endl;
				Tab* prevTab = prev;
				tab->m_prev = prevTab->m_prev;
				if(prevTab->m_prev)
					prevTab->m_prev->m_next = tab;
				getUIDriver().getAnimationEngine().dispatchAnimation<TabShiftAnimation>(m_tabShiftAnimGroup, prevTab, /* isLeft: */ com::False);
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

	void NotebookView::abortTabAnim() noexcept
	{
		// for(auto& ctx : m_animContexts)
		// {
		// 	ctx.onEndEvent.publish(ctx.tabView);
		// 	ctx.onEndEvent.clear();
		// }
		// m_tabContainer->unlockLayout(true);
		// m_animContexts.clear();
		// m_isRunning = com::False;
	}

	void NotebookView::dispatchAnimNewTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabInsertAnimation>(m_tabAnimGroup, tab);
	}

	void NotebookView::dispatchAnimRemoveTab(Tab* tab) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabRemoveAnimation>(m_tabAnimGroup, tab);		// if(m_isRunning)
	}

	void  NotebookView::dispatchAnimTabShift(Tab* tab, com::Bool isLeft) noexcept
	{
		// if(m_isRunning)
			// abortTabAnim();
		// AnimContext ctx { };
		// ctx.tabView = tabView;
		// ctx.curTabPos = tabView->getPosition().x;
		// ctx.dstTabPos = ctx.curTabPos + (isLeft ? -tabView->getPrev()->getSize().width : tabView->getNext()->getSize().width);
		// ctx.speed = (ctx.dstTabPos - ctx.curTabPos) / m_animDuration;
		// ctx.isTabRearrange = com::True;
		// m_animContexts.push_back(ctx);
		// m_isStartAnimBatch = com::True;
		// return m_animContexts.back();
	}

	template<typename T>
	static T* GetLinkedListRoot(T* node) noexcept
	{
		while(node && node->getPrev())
			node = node->getPrev();
		return node;
	}

	Tab::Tab(NotebookView* notebook, const std::string_view labelStr, Tab* after) noexcept
	{
		notebook->getTabContainer()->lockLayout();
		TabView* tabView = notebook->getUIDriver().createContainer<TabView>(com::null_pointer<Container>());
		if(after)
		{
			InsertLinkedListNodeAfter(this, after);
			m_index = after->getIndex() + 1;
			auto node = getNext();
			while(node)
			{
				node->m_index += 1;
				node = node->getNext();
			}
		}
		else
			m_index = 0;
		notebook->getTabContainer()->addAt(tabView, m_index);
		tabView->setLabel(labelStr);
		LayoutAttributes attr = tabView->getLayoutAttributes();
		attr.minSize.width = TAB_VIEW_MIN_WIDTH;
		attr.prefSize.width = std::max(TAB_VIEW_MIN_WIDTH, tabView->getSize().width);
		tabView->setLayoutAttributes(attr);
		// Recalculate the layout to set the position and sizes of the tab views correctly
		notebook->getTabContainer()->unlockLayout(true);	
		m_tabView = tabView;
	}
	Tab::~Tab() noexcept
	{
		auto node = getNext();
		while(node)
		{
			node->m_index -= 1;
			node = node->getNext();
		}
		RemoveLinkedListNode(this);
		m_tabView->getUIDriver().destroyContainer<TabView>(m_tabView);
	}
	void Tab::setPage(NotebookPage* page) noexcept
	{
		m_page = page;
		if(page)
			page->m_tab = this;
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
		Tab* tab = new Tab(this, labelStr, afterPage ? afterPage->getTab() : com::null_pointer<Tab>());

		// <Begin> Initialize Animation Start state and Setup Animation Context
		dispatchAnimNewTab(tab);
		// <End>

		// Create Page
		NotebookPage* page = new NotebookPage(container);
		tab->setPage(page);

		if(!tab->getPrev())
		{
			_com_assert(tab->m_index == 0);
			m_head = page;
		}

		TabView* tabView = tab->getTabView();

		tabView->getOnPressEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			this->viewPage(page);
			this->m_onPageSelectEvent.publish(page);

			this->m_tabRearrangeContext.grabbedTab = page->getTab();
			Vec2Df mousePos = getUIDriver().getInputDriver().getMousePosition();
			this->m_tabRearrangeContext.positionOffset = page->getTab()->getTabView()->getGlobalPosition() - mousePos;
			this->m_tabRearrangeContext.isMoved = com::False;
			this->m_tabContainer->lockLayout();
			
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
				this->m_head = GetLinkedListRoot(this->m_tabRearrangeContext.grabbedTab)->getPage();
			}
			this->m_tabRearrangeContext.grabbedTab = com::null_pointer<Tab>();
			this->m_tabContainer->unlockLayout(static_cast<bool>(this->m_tabRearrangeContext.isMoved));
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

		// NOTE: The following doubly-linked list manipulation preserves the "backward" links for the tabView which needs to be destroyed.
		// So that, the index (by calling getIndex() on the TabView) could still be calculated accurately!
		auto& driver = getUIDriver();

		Tab* tab = page->getTab();
		if(!tab->getPrev())
			m_head = page->getNext();
		RemoveLinkedListNode(tab);
		
		if(page->m_onPageRemove.has_value())
			page->m_onPageRemove.value() (page);
		driver.destroyContainer<Container>(page->m_container);
		delete page;
		tab->setPage(com::null_pointer<NotebookPage>());

		dispatchAnimRemoveTab(tab);

		dump();
	}

	void NotebookView::dump() noexcept
	{
		std::cout << "Dump (Notebook): \n"
				  << "Current Selected: " << (m_currentPage ? m_currentPage->getTab()->getTabView()->getLabel() : "<null>") << "\n";
		NotebookPage* page = m_head;
		while(page != com::null_pointer<NotebookPage>())
		{
			std::cout << page->getTab()->getTabView()->getLabel() << ", index = " << page->getTab()->getIndex() << std::endl;
			page = page->getNext();
		}
	}
}