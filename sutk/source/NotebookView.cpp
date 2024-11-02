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

	NotebookView::NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : VBoxContainer(driver, parent, /* isLockLayout: */ true, isLayoutIgnore, layer),
																																					m_head(NULL),
																																					m_currentPage(NULL)
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

	void TabView::setLabel(const std::string_view str) noexcept
	{
		m_graphic->getLabel().set(str);
		SUTK::Vec2Df size = m_graphic->getMinBoundSize();
		setSize({ size.width + TAB_LABEL_CLOSE_BUTTON_SPACING + CLOSE_BUTTON_RIGHT_MARGIN + CLOSE_BUTTON_WIDTH, getSize().height });
	}

	void TabView::unselectedState() noexcept
	{

	}

	void TabView::selectedState() noexcept
	{
		
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
		TabView* tabView = getUIDriver().createContainer<TabView>(m_tabContainer);
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
		tabView->setLabel(labelStr);
		LayoutAttributes attr = tabView->getLayoutAttributes();
		attr.minSize.width = TAB_VIEW_MIN_WIDTH;
		attr.prefSize.width = std::max(TAB_VIEW_MIN_WIDTH, tabView->getSize().width);
		tabView->setLayoutAttributes(attr);
		m_tabContainer->unlockLayout(true);

		// Create Page
		NotebookPage* page = new NotebookPage(container);
		tabView->m_page = page;
		page->m_tabView = tabView;

		tabView->getOnReleaseEvent().subscribe([this, page](SUTK::Button* button) noexcept
		{
			this->viewPage(page);
		});
		return page;
	}

	void NotebookView::viewPage(NotebookPage* page) noexcept
	{
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
		// Activate the requested page's container
		Container* container = page->getContainer();
		if(!container->isActive())
			ContainerUtility::SetActiveAllRecursive(container, com::True);
		// Move the associated tabView into selected state
		TabView* tabView = page->getTabView();
		tabView->selectedState();
		m_currentPage = page;
	}

	void NotebookView::removePage(NotebookPage* page) noexcept
	{

	}
}