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

namespace SUTK
{

	NotebookPage::NotebookPage(Container* container) noexcept : m_data(NULL), m_tabView(com::null_pointer<TabView>()), m_container(container) { }
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
		auto next = m_tabView->getNext();
		if(next)
			return next->getPage();
		return com::null_pointer<NotebookPage>();
	}

	NotebookPage* NotebookPage::getPrev() noexcept
	{
		auto prev = m_tabView->getPrev();
		if(prev)
			return prev->getPage();
		return com::null_pointer<NotebookPage>();
	}

	void NotebookPage::setLabel(const std::string_view str) noexcept
	{
		m_tabView->setLabel(str);
	}

	const std::string& NotebookPage::getLabel() const noexcept
	{
		return m_tabView->getLabel();
	}

	u32 NotebookPage::getIndex() const noexcept
	{
		return m_tabView->getIndex();
	}

	TabView::TabView(UIDriver& driver, Container* parent) noexcept : Button(driver, parent, /* isCreateDefaultGraphic: */ true), m_next(com::null_pointer<TabView>()), m_prev(com::null_pointer<TabView>())
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

		m_tabAnimGroup = driver.createObject<TabAnimGroup>(m_tabContainer);

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
		delete m_tabAnimGroup;
	}

	void NotebookView::onMouseMove(Vec2Df pos) noexcept
	{
		// TabView* tabView = this->m_tabRearrangeContext.grabbedTabView;
		// if(!m_tabRearrangeContext.isMoved)
		// {
		// 	m_tabRearrangeContext.layer = tabView->getLayer();
		// 	tabView->setLayer(MaxLayer);
		// 	m_tabRearrangeContext.isMoved = com::True;
		// }
		// pos = m_tabRearrangeContext.positionOffset + tabView->getParent()->getScreenCoordsToLocalCoords(pos);
		// tabView->setPosition(pos);
		// TabView* next = tabView->getNext();
		// if(next)
		// {
		// 	f32 middle = next->getPosition().x + next->getSize().width * 0.5f + TAB_VIEW_MIDDLE_SPREAD;
		// 	if(((pos.x + tabView->getSize().width) > middle) && !isRunning())
		// 	{
		// 		std::cout << "Swapping with next tab" << std::endl;
		// 		TabView* nextTab = next;
		// 		tabView->m_next = nextTab->m_next;
		// 		if(nextTab->m_next)
		// 			nextTab->m_next->m_prev = tabView;
				
		// 		if(tabView->m_prev)
		// 			tabView->m_prev->m_next = nextTab;
		// 		nextTab->m_prev = tabView->m_prev;

		// 		nextTab->m_next = tabView;
		// 		tabView->m_prev = nextTab;
		// 		dispatchAnimTabShift(next, /* isLeft: */ com::True).onEndEvent.subscribe([tabView](TabView* nextTab) noexcept
		// 		{
		// 		});
		// 	}
		// }
		// TabView* prev = tabView->getPrev();
		// if(prev)
		// {
		// 	f32 middle = prev->getPosition().x + prev->getSize().width * 0.5f;
		// 	if((pos.x < middle) && !isRunning())
		// 	{
		// 		std::cout << "Swapping with prev tab" << std::endl;
		// 		TabView* prevTab = prev;
		// 		prevTab->m_next = tabView->m_next;
		// 		if(tabView->m_next)
		// 			tabView->m_next->m_prev = prevTab;

		// 		if(prevTab->m_prev)
		// 			prevTab->m_prev->m_next = tabView;
		// 		tabView->m_prev = prevTab->m_prev;

		// 		tabView->m_next = prevTab;
		// 		prevTab->m_prev = tabView;
		// 		dispatchAnimTabShift(prev, /* isRight: */ com::False).onEndEvent.subscribe([tabView](TabView* prevTab) noexcept
		// 		{
		// 		});
		// 	}
		// }
	}

	class TabInsertAnimation : public AnimationEngine::AnimContextBase
	{
		friend class TabAnimGroup;
	private:
		TabView* m_tab;
		u32 m_index;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			m_index = m_tab->getIndex();
			f32 dstTabWidth = m_tab->getSize().width;
			f32 offset = dstTabWidth - TAB_VIEW_ANIM_START_WIDTH;
			setSpeed(offset / DEFAULT_NEW_TAB_ANIM_DURATION);
			setLength(offset);
			Container* tabContainer = m_tab->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_index + 1; i < childs.size(); ++i)
				childs[i]->moveLeft(offset);
			m_tab->setSize({ TAB_VIEW_ANIM_START_WIDTH, m_tab->getSize().height });
			LayoutAttributes& attr = m_tab->getLayoutAttributes();
			attr.prefSize = m_tab->getSize();
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			m_index = m_tab->getIndex();
			Container* tabContainer = m_tab->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_index + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaValue);
			m_tab->extendRight(deltaValue);
			LayoutAttributes& attr = m_tab->getLayoutAttributes();
			attr.prefSize = m_tab->getSize();
		}
	public:
		TabInsertAnimation(UIDriver& driver, TabAnimGroup* group, TabView* tab) noexcept : AnimContextBase(driver, group), m_tab(tab) { }
	};

	class TabRemoveAnimation : public AnimationEngine::AnimContextBase
	{
		friend class TabAnimGroup;
	private:
		TabView* m_tab;
		u32 m_index;
	protected:
		virtual void onStart() noexcept override
		{
			AnimContextBase::onStart();
			m_index = m_tab->getIndex();
			f32 dstTabWidth = TAB_VIEW_ANIM_START_WIDTH;
			f32 offset = dstTabWidth - m_tab->getSize().width;
			setSpeed(offset / DEFAULT_NEW_TAB_ANIM_DURATION);
			setLength(offset);
		}
		virtual void onStep(f32 deltaValue) noexcept override
		{
			m_index = m_tab->getIndex();
			Container* tabContainer = m_tab->getParent();
			std::vector<Container*>& childs = tabContainer->getChilds();
			for(std::size_t i = m_index + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaValue);
			m_tab->extendRight(deltaValue);
			LayoutAttributes& attr = m_tab->getLayoutAttributes();
			attr.prefSize = m_tab->getSize();
		}
		virtual void onEnd() noexcept override
		{
			auto node = m_tab->getNext();
			while(node)
			{
				_com_assert(node->m_index > 0);
				node->m_index -= 1;
				node = node->getNext();
			}
			m_tab->getUIDriver().destroyContainer<TabView>(m_tab);
			AnimContextBase::onEnd();
		}
	public:
		TabRemoveAnimation(UIDriver& driver, TabAnimGroup* group, TabView* tab) noexcept : AnimContextBase(driver, group), m_tab(tab) { }
	};

	void TabAnimGroup::onPresent(AnimationEngine::AnimContextBase* animContext) noexcept
	{
		if(auto insertAnim = dynamic_cast<TabInsertAnimation*>(animContext))
		{
			TabView* insertTab = insertAnim->m_tab;
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
			TabView* removeTab = removeAnim->m_tab;
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
		if(!m_tabLayoutController->isLockedLayout())
			m_tabLayoutController->lockLayout();
	}
	void TabAnimGroup::onWhenAllEnd() noexcept
	{
		std::cout << "All ended" << std::endl;
		m_tabLayoutController->unlockLayout();
	}
	void TabAnimGroup::onStepAll() noexcept
	{
		// Recalculate hBox layout when all of the children containers are done with their size/pos changes for this step.
		m_tabLayoutController->recalculateLayout();
	}
	TabAnimGroup::TabAnimGroup(UIDriver& driver, ILayoutController* tabLayoutController) noexcept : AnimGroup(driver), m_tabLayoutController(tabLayoutController)
	{

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

	void NotebookView::dispatchAnimNewTab(TabView* tabView) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabInsertAnimation>(m_tabAnimGroup, tabView);
	}

	void NotebookView::dispatchAnimRemoveTab(TabView* tabView) noexcept
	{
		getUIDriver().getAnimationEngine().dispatchAnimation<TabRemoveAnimation>(m_tabAnimGroup, tabView);		// if(m_isRunning)
	}

	void  NotebookView::dispatchAnimTabShift(TabView* tabView, com::Bool isLeft) noexcept
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
		m_tabContainer->lockLayout();
		TabView* tabView = getUIDriver().createContainer<TabView>(com::null_pointer<Container>());
		if(afterPage)
		{
			TabView* afterPageTabView = afterPage->getTabView();
			TabView* afterPageNext = afterPageTabView->m_next;
			if(afterPageNext)
			{
				afterPageNext->m_prev = tabView;
				tabView->m_next = afterPageNext;
			}
			tabView->m_prev = afterPageTabView;
			afterPageTabView->m_next = tabView;
			tabView->m_index = afterPageTabView->getIndex() + 1;
			auto node = afterPageNext;
			while(node)
			{
				node->m_index += 1;
				node = node->m_next;
			}
		}
		else
			tabView->m_index = 0;
		m_tabContainer->addAt(tabView, tabView->getIndex());
		tabView->setLabel(labelStr);
		LayoutAttributes attr = tabView->getLayoutAttributes();
		attr.minSize.width = TAB_VIEW_MIN_WIDTH;
		attr.prefSize.width = std::max(TAB_VIEW_MIN_WIDTH, tabView->getSize().width);
		tabView->setLayoutAttributes(attr);
		// Recalculate the layout to set the position and sizes of the tab views correctly
		m_tabContainer->unlockLayout(true);

		// <Begin> Initialize Animation Start state and Setup Animation Context
		dispatchAnimNewTab(tabView);
		// <End>

		// Create Page
		NotebookPage* page = new NotebookPage(container);
		tabView->m_page = page;
		page->m_tabView = tabView;

		if(!tabView->getPrev())
		{
			_com_assert(tabView->m_index == 0);
			m_head = page;
		}

		tabView->getOnPressEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			this->viewPage(page);
			this->m_onPageSelectEvent.publish(page);

			this->m_tabRearrangeContext.grabbedTabView = page->getTabView();
			Vec2Df mousePos = getUIDriver().getInputDriver().getMousePosition();
			this->m_tabRearrangeContext.positionOffset = page->getTabView()->getGlobalPosition() - mousePos;
			this->m_tabRearrangeContext.tabIndex = page->getTabView()->getIndex();
			this->m_tabRearrangeContext.isMoved = com::False;
			this->m_tabContainer->lockLayout();
			
			this->GlobalMouseMoveHandlerObject::awake();
		});
		tabView->getOnReleaseEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			_com_assert(this->m_tabRearrangeContext.grabbedTabView != com::null_pointer<TabView>());
			if(this->m_tabRearrangeContext.isMoved)
			{
				this->m_tabRearrangeContext.grabbedTabView->setLayer(m_tabRearrangeContext.layer);
				auto index = page->getTabView()->getIndex();
				if(this->m_tabRearrangeContext.tabIndex != index)
					this->m_tabContainer->swapChildren(this->m_tabRearrangeContext.tabIndex, index);
			}
			this->m_tabRearrangeContext.grabbedTabView = com::null_pointer<TabView>();
			this->m_tabContainer->unlockLayout(static_cast<bool>(this->m_tabRearrangeContext.isMoved));
			this->GlobalMouseMoveHandlerObject::sleep();
		});

		viewPage(page);

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
			TabView* tabView = m_currentPage->getTabView();
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
			TabView* tabView = page->getTabView();
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
		TabView* tabView = page->m_tabView;
		if(tabView->m_prev)
			tabView->m_prev->m_next = tabView->m_next;
		else
			m_head = page->getNext();
		if(tabView->m_next)
			tabView->m_next->m_prev = tabView->m_prev;
		if(page->m_onPageRemove.has_value())
			page->m_onPageRemove.value() (page);
		driver.destroyContainer<Container>(page->m_container);
		delete page;
		tabView->m_page = com::null_pointer<NotebookPage>();

		dispatchAnimRemoveTab(tabView);
	}

	void NotebookView::dump() noexcept
	{
		std::cout << "Dump (Notebook): \n"
				  << "Current Selected: " << (m_currentPage ? m_currentPage->getTabView()->getLabel() : "<null>") << "\n";
		NotebookPage* page = m_head;
		while(page != com::null_pointer<NotebookPage>())
		{
			std::cout << page->getTabView()->getLabel() << std::endl;
			page = page->getNext();
		}
	}
}