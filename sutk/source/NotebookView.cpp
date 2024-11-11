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


#define TAB_VIEW_ANIM_START_WIDTH 0.4f
#define DEFAULT_NEW_TAB_ANIM_DURATION 0.4f

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

	u32 TabView::getIndex() const noexcept
	{
		u32 index = 0;
		TabView* tab = m_prev;
		while(tab)
		{
			tab = tab->m_prev;
			++index;
		}
		return index;
	}

	void TabView::unselectedState() noexcept
	{

	}

	void TabView::selectedState() noexcept
	{
		
	}

	NotebookView::NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : VBoxContainer(driver, parent, /* isLockLayout: */ true, isLayoutIgnore, layer), Runnable(driver),
																																					m_head(com::null_pointer<NotebookPage>()),
																																					m_currentPage(com::null_pointer<NotebookPage>()),
																																					m_onPageSelectEvent(this),
																																					m_isRunning(com::False),
																																					m_isStartAnimBatch(com::False),
																																					m_animDuration(DEFAULT_NEW_TAB_ANIM_DURATION)
	{
		// Create Tab Container
		m_textGroupContainer = driver.createContainer<TextGroupContainer>(this, com::False, getDepth() + 10000);
		LayoutAttributes attr = m_textGroupContainer->getLayoutAttributes();
		attr.minSize.height = TAB_BAR_HEIGHT;
		attr.prefSize = attr.minSize;
		m_textGroupContainer->setLayoutAttributes(attr);
		m_tabContainer = driver.createContainer<HBoxContainer>(m_textGroupContainer);
		m_tabContainer->alwaysFitInParent();

		// Create Container for holding page container
		m_pageContainer = driver.createContainer<Container>(this);
		attr = m_pageContainer->getLayoutAttributes();
		attr.prefSize = attr.maxSize;
		_com_assert((attr.prefSize.height == F32_INFINITY) && (attr.prefSize.width == F32_INFINITY));
		m_pageContainer->setLayoutAttributes(attr);
		m_pageContainer->enableDebug(true, Color4::yellow());

		unlockLayout();
	}

	bool NotebookView::isRunning()
	{
		return static_cast<bool>(m_isRunning || m_isStartAnimBatch);
	}

	void NotebookView::update()
	{
		if(m_isStartAnimBatch)
		{
			m_tabContainer->lockLayout();
			for(auto& ctx : m_animContexts)
			{
				if(ctx.speed < 0)
					continue;
				f32 width = ctx.tabView->getSize().width;
				std::vector<Container*>& childs = m_tabContainer->getChilds();
				ctx.tabIndex = ctx.tabView->getIndex();
				for(std::size_t i = ctx.tabIndex + 1; i < childs.size(); ++i)
					childs[i]->moveLeft(width);
				ctx.tabView->setSize({ ctx.curTabWidth, ctx.tabView->getSize().height });
			}
			m_isStartAnimBatch = com::False;
			m_isRunning = com::True;
		}

		com::Bool isUnlockLayout = com::True;

		std::vector<Container*>& childs = m_tabContainer->getChilds();
		for(auto& ctx : m_animContexts)
		{
			f32 deltaWidth = ctx.speed * getUIDriver().getDeltaTime();
			f32 safeDeltaWidth = ctx.dstTabWidth - ctx.curTabWidth;
			if(std::abs(deltaWidth) >= std::abs(safeDeltaWidth))
			{
				deltaWidth = safeDeltaWidth;
				ctx.curTabWidth = ctx.dstTabWidth;
			}
			else 
			{
				ctx.curTabWidth += deltaWidth;
				isUnlockLayout = com::False;
			}

			for(std::size_t i = ctx.tabIndex + 1; i < childs.size(); ++i)
				childs[i]->moveRight(deltaWidth);
			ctx.tabView->extendRight(deltaWidth);
		}
		if(isUnlockLayout)
			abortTabAnim();
	}

	void NotebookView::abortTabAnim() noexcept
	{
		for(auto& ctx : m_animContexts)
		{
			ctx.onEndEvent.publish(ctx.tabView);
			ctx.onEndEvent.clear();
		}
		m_tabContainer->unlockLayout(true);
		m_animContexts.clear();
		m_isRunning = com::False;
	}

	void NotebookView::dispatchAnimNewTab(TabView* tabView) noexcept
	{
		if(m_isRunning)
			abortTabAnim();

		AnimContext ctx;
		ctx.tabView = tabView;
		ctx.dstTabWidth = tabView->getSize().width;
		ctx.curTabWidth = TAB_VIEW_ANIM_START_WIDTH;
		ctx.speed = ctx.dstTabWidth / m_animDuration;
		m_animContexts.push_back(ctx);
		m_isStartAnimBatch = com::True;
	}

	NotebookView::AnimContext& NotebookView::dispatchAnimRemoveTab(TabView* tabView) noexcept
	{
		if(m_isRunning)
			abortTabAnim();
		AnimContext ctx;
		ctx.tabView = tabView;
		ctx.dstTabWidth = TAB_VIEW_ANIM_START_WIDTH;
		ctx.curTabWidth = tabView->getSize().width;
		ctx.speed = -ctx.curTabWidth / m_animDuration;
		m_animContexts.push_back(ctx);
		m_isStartAnimBatch = com::True;
		return m_animContexts.back();
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
		}
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
			m_head = page;

		tabView->getOnReleaseEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			this->viewPage(page);
			this->m_onPageSelectEvent.publish(page);
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

		dispatchAnimRemoveTab(tabView).onEndEvent.subscribe([this](TabView* tabView)
		{
			this->getUIDriver().destroyContainer<TabView>(tabView);
		});
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