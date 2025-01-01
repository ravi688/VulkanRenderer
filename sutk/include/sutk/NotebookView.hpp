#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/VBoxContainer.hpp> // for SUTK::VBoxContainer
#include <sutk/HBoxContainer.hpp> // for SUTK::HBoxContainer
#include <sutk/Button.hpp> // for SUTK::Button
#include <sutk/Label.hpp> // for SUTK::Label::set()
#include <sutk/AnimationEngine.hpp>
#include <sutk/MaskedScrollableContainer.hpp>

#include <common/Concepts.hpp> // for com::CompareFunction
#include <common/Event.hpp> // for com::Event

#include <optional> // for std::optional

namespace SUTK
{
	class Tab;

	// Customized Pages can be created by deriving from this class and passing the type of the derived class 
	// in the template argument list of createPage<>() function
	class SUTK_API NotebookPage : public UIDriverObject
	{
		friend class NotebookView;
		friend class Tab;
	private:
		Tab* m_tab;
		Container* m_container;

		Tab* getTab() noexcept { return m_tab; }
		Container* getContainer() noexcept { return m_container; }
		u32 getIndex() const noexcept;
	protected:
		// Called whenever this page gets activated, i.e. the tab associated with this page has been selected
		virtual void onActivate() noexcept { };
		// Called whenver this page gets deactivated, i.e. any other tab has now been selected
		virtual void onDeactivate() noexcept { };
	public:
		NotebookPage(UIDriver& driver, Container* container) noexcept;
		virtual ~NotebookPage() noexcept;

		// Getters for pointer to the next and previous NotebookPage objects
		NotebookPage* getNext() noexcept;
		const NotebookPage* getNext() const noexcept { return com::cast_away_const(this)->getNext(); }
		NotebookPage* getPrev() noexcept;
		const NotebookPage* getPrev() const noexcept { return com::cast_away_const(this)->getPrev(); }
		
		// Getters and Setters
		void setLabel(const std::string_view str) noexcept;
		const std::string& getLabel() const noexcept;
	};

	class ImageButtonGraphic;
	class DefaultButtonGraphic;

	class SUTK_API TabView : public Button
	{
	private:
		DefaultButtonGraphic* m_graphic;
		Button* m_closeButton;
		ImageButtonGraphic* m_closeButtonGraphic;
		// Preserve hover and press color of the m_graphic
		// Since we modify those when the tab is selected
		// we would need to restore them to their initial values
		// using these preserved values.
		Color4 m_hoverColor;
		Color4 m_pressColor;
		Color4 m_idleColor;
		static UIDriver::ImageReference s_closeIcon;
	public:
		TabView(UIDriver& driver, Container* parent) noexcept;
		~TabView() noexcept;

		void setLabel(const std::string_view str) noexcept;
		const std::string& getLabel() noexcept;

		void unselectedState() noexcept;
		void selectedState() noexcept;
	};

	class NotebookView;
	class TabBar;

	class SUTK_API Tab : public com::LinkedListNodeBase<Tab>
	{
		friend class NotebookView;
		friend class TabAnimGroup;
		friend class TabRemoveAnimation;
		friend class TabShiftAnimation;
		friend class TabBar;
	private:
		TabBar* m_tabBar;
		TabView* m_tabView;
		NotebookPage* m_page;
		u32 m_index;
		Vec2Df m_pos;
	public:
		// Setters
		void setPage(NotebookPage* page) noexcept;
		// NOTE: this function must be used to change the lable of the tab. Do not use getTabView()->setLabel()
		void setLabel(const std::string_view str) noexcept;
		// Getters
		Vec2Df getPos() const noexcept { return m_pos; }
		TabBar* getTabBar() noexcept { return m_tabBar; }
		TabView* getTabView() noexcept { return m_tabView; }
		NotebookPage* getPage() noexcept { return m_page; }
		u32 getIndex() const noexcept { return m_index; }
		u32 getLinkedListIndex() const noexcept;
	};

	class SUTK_API TabBar : public HBoxContainer
	{
		friend class NotebookView;
	private:
		Tab* m_root;
	protected:
		virtual void onRecalculateLayout() noexcept override;
	public:
		TabBar(UIDriver& driver, Container* parent) noexcept;
		~TabBar() noexcept;

		Tab* createTab(const std::string_view labelStr, Tab* after = com::null_pointer<Tab>()) noexcept;
		void destroyTab(Tab* tab) noexcept;
		void removeTab(Tab* tab) noexcept;
		Tab* getRootTab() noexcept { return m_root; }
	};

	typedef MaskedScrollableContainer<TabBar> ScrollableTabBar;

	class TabRemoveAnimation;
	class TabInsertAnimation;
	class NotebookView;

	class TabAnimGroup : public AnimationEngine::AnimGroup
	{
	private:
		NotebookView* m_notebook;
		std::vector<TabRemoveAnimation*> m_inFlightRemoves;
		std::vector<TabInsertAnimation*> m_inFlightInserts;	
	protected:
		virtual void onPresent(AnimationEngine::AnimContextBase* animContext) noexcept override;
		virtual void onAbsent(AnimationEngine::AnimContextBase* animContext) noexcept override;
		virtual void onWhenAnyStart() noexcept override;
		virtual void onWhenAllEnd() noexcept override;
	public:
		TabAnimGroup(UIDriver& driver, NotebookView* notebook) noexcept;
		NotebookView* getNotebook() noexcept { return m_notebook; }
	};

	class TabShiftAnimation;

	class TabShiftAnimGroup : public AnimationEngine::AnimGroup
	{
	private:
		NotebookView* m_notebook;
		std::vector<TabShiftAnimation*> m_inFlightLeftShifts;
		std::vector<TabShiftAnimation*> m_inFlightRightShifts;	
		com::Bool m_isAborting;
	protected:
		virtual void onPresent(AnimationEngine::AnimContextBase* animContext) noexcept override;
		virtual void onAbsent(AnimationEngine::AnimContextBase* animContext) noexcept override;
	public:
		TabShiftAnimGroup(UIDriver& driver, NotebookView* notebook) noexcept;

		NotebookView* getNotebook() noexcept { return m_notebook; }
		void abort() noexcept;
	};

	template<typename T>
	concept NotebookPageType = std::derived_from<T, NotebookPage>;

	class Panel;
	class TextGroupContainer;
	
	class SUTK_API NotebookView : public VBoxContainer, public GlobalMouseMoveHandlerObject
	{
		friend class TabAnimGroup;
	public:
		typedef com::Event<NotebookView, NotebookPage*> OnPageSelectEvent;
	private:
		struct TabRearrangeContext
		{
			Tab* grabbedTab;
			Vec2Df positionOffset;
			u32 tabIndex;
			com::Bool isMoved;
			Layer layer;
		};
	private:
		// TextGroup for TabView(s)
		TextGroupContainer* m_textGroupContainer;
		Panel* m_tabBarBGPanel;
		ScrollContainer* m_tabBarContainer;
		ScrollableTabBar* m_tabBar;
		Panel* m_pageContainerParent;
		NotebookPage* m_currentPage;
		TabAnimGroup* m_tabAnimGroup;
		TabShiftAnimGroup* m_tabShiftAnimGroup;
		OnPageSelectEvent m_onPageSelectEvent;
		TabRearrangeContext m_tabRearrangeContext;
		com::Bool m_isRunning;
		com::Bool m_isStartAnimBatch;
		f32 m_animDuration;


		void dispatchAnimNewTab(Tab* tab) noexcept;
		void dispatchAnimRemoveTab(Tab* tab) noexcept;

		void swapWithNext(Tab* tab) noexcept;
		void swapWithPrev(Tab* tab) noexcept;

		// Below methods are being used inside createPage<> method
		Tab* createTab(const std::string_view labelStr, NotebookPage* page, Tab* afterTab) noexcept;
		Container* createPageContainer() noexcept;
		void setupTabEvents(Tab* tab, NotebookPage* page) noexcept;
	protected:
		virtual void onMouseMove(Vec2Df pos) noexcept override;

	public:
		NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::False, Layer layer = InvalidLayer) noexcept;
		~NotebookView() noexcept;
		void checkForTabSwap() noexcept;

		TabBar* getTabBar() noexcept { return m_tabBar; }

		OnPageSelectEvent& getOnPageSelectEvent() noexcept { return m_onPageSelectEvent; }

		void setAnimDuration(f32 duration) noexcept { m_animDuration = duration; }

		NotebookPage* getRootPage() noexcept { return getTabBar()->getRootTab() ? getTabBar()->getRootTab()->getPage() : com::null_pointer<NotebookPage>(); }
		NotebookPage* getCurrentPage() noexcept { return m_currentPage; }

		template<NotebookPageType T = NotebookPage, typename... Args>
		T* createPage(const std::string_view labelStr, Args&&... args) noexcept;
		template<NotebookPageType T = NotebookPage, typename... Args>
		T* createPageAfter(const std::string_view labelStr, NotebookPage* afterPage, Args&&... args) noexcept;
		void viewPage(NotebookPage* page) noexcept;
		void removePage(NotebookPage* page) noexcept;

		void dump() noexcept;
	};
	
	template<NotebookPageType T, typename... Args>
	T* NotebookView::createPage(const std::string_view labelStr, Args&&... args) noexcept
	{
		return createPageAfter<T, Args...>(labelStr, com::null_pointer<NotebookPage>(), std::forward<Args&&>(args)...);
	}

	template<NotebookPageType T, typename... Args>
	T* NotebookView::createPageAfter(const std::string_view labelStr, NotebookPage* afterPage, Args&&... args) noexcept
	{
		// Create Container for the page
		Container* container = createPageContainer();
		// Create Page
		T* page = getUIDriver().createObject<T>(container, std::forward<Args&&>(args)...);
		// Create Tab for the page
		Tab* tab = createTab(labelStr, page, afterPage ? afterPage->getTab() : com::null_pointer<Tab>());
		// Subscribe to tab select and deselect events to activate or deactivate the associate page
		setupTabEvents(tab, page);
		// Newly created page should be activated
		viewPage(page);
		dump();
		return page;
	}
}