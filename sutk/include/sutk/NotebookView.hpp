#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/VBoxContainer.hpp> // for SUTK::VBoxContainer
#include <sutk/Button.hpp> // for SUTK::Button
#include <sutk/Label.hpp> // for SUTK::Label::set()
#include <sutk/AnimationEngine.hpp>

#include <common/Concepts.hpp> // for com::CompareFunction
#include <common/Event.hpp> // for com::Event

#include <optional> // for std::optional

namespace SUTK
{
	class Tab;

	class NotebookPage
	{
		friend class NotebookView;
		friend class Tab;
		friend u32 getIndexOfPage(const NotebookPage* page) noexcept;
	private:
		void* m_data;
		void (*m_dataDeleter)(void*);
		std::optional<std::function<void(NotebookPage*)>> m_onPageRemove;
		Tab* m_tab;
		Container* m_container;

		Tab* getTab() noexcept { return m_tab; }
		NotebookPage* getNext() noexcept;
		const NotebookPage* getNext() const noexcept { return com::cast_away_const(this)->getNext(); }
		NotebookPage* getPrev() noexcept;
		const NotebookPage* getPrev() const noexcept { return com::cast_away_const(this)->getPrev(); }
	public:
		NotebookPage(Container* container) noexcept;
		~NotebookPage() noexcept;
		void setOnRemove(std::function<void(NotebookPage*)> onPageRemove) noexcept { m_onPageRemove = onPageRemove; }
		void setLabel(const std::string_view str) noexcept;
		const std::string& getLabel() const noexcept;
		Container* getContainer() noexcept { return m_container; }
		u32 getIndex() const noexcept;
		template<typename T, typename... Args>
		void attachData(Args&&... args) noexcept;
		template<typename T>
		T* getData() noexcept;
		template<typename T>
		const T* getData() noexcept;
	};

	template<typename T, typename... Args>
	void NotebookPage::attachData(Args&&... args) noexcept
	{
		_com_assert(m_data != com::null_pointer<void>());
		m_data = new T(std::forward<Args&&>(args)...);
		m_dataDeleter = [](void* data) noexcept { delete static_cast<T*>(data); };
	}

	template<typename T>
	T* NotebookPage::getData() noexcept
	{
		_com_assert_wrn(m_data != com::null_pointer<void>());
		return static_cast<T*>(m_data);
	}

	template<typename T>
	const T* NotebookPage::getData() noexcept
	{
		_com_assert_wrn(m_data != com::null_pointer<const void>());
		return static_cast<const T*>(m_data);
	}

	class ImageButtonGraphic;
	class DefaultButtonGraphic;

	class SUTK_API TabView : public Button
	{
	private:
		DefaultButtonGraphic* m_graphic;
		Button* m_closeButton;
		ImageButtonGraphic* m_closeButtonGraphic;
	public:
		TabView(UIDriver& driver, Container* parent) noexcept;
		~TabView() noexcept;

		void setLabel(const std::string_view str) noexcept;
		const std::string& getLabel() noexcept;

		void unselectedState() noexcept;
		void selectedState() noexcept;
	};

	class NotebookView;

	class SUTK_API Tab : public com::LinkedListNodeBase<Tab>
	{
		friend class NotebookView;
		friend class TabAnimGroup;
		friend class TabRemoveAnimation;
		friend class TabShiftAnimation;
	private:
		TabView* m_tabView;
		NotebookPage* m_page;
		u32 m_index;
		Vec2Df m_pos;
	public:
		Tab(NotebookView* notebook, const std::string_view labelStr, Tab* after = com::null_pointer<Tab>()) noexcept;
		~Tab() noexcept;
		Vec2Df getPos() const noexcept { return m_pos; }
		void syncPos() noexcept;
		void setPage(NotebookPage* page) noexcept;
		// Getters
		TabView* getTabView() noexcept { return m_tabView; }
		NotebookPage* getPage() noexcept { return m_page; }
		u32 getIndex() const noexcept { return m_index; }
		u32 getLinkedListIndex() const noexcept;
	};

	class HBoxContainer;

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
		HBoxContainer* m_tabContainer;
		Container* m_pageContainer;
		NotebookPage* m_head;
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
	protected:
		virtual void onMouseMove(Vec2Df pos) noexcept override;

	public:
		NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::False, Layer layer = InvalidLayer) noexcept;
		~NotebookView() noexcept;
		void checkForTabSwap() noexcept;

		HBoxContainer* getTabContainer() noexcept { return m_tabContainer; }

		OnPageSelectEvent& getOnPageSelectEvent() noexcept { return m_onPageSelectEvent; }

		void setAnimDuration(f32 duration) noexcept { m_animDuration = duration; }

		NotebookPage* getCurrentPage() noexcept { return m_currentPage; }

		NotebookPage* createPage(const std::string_view labelStr, NotebookPage* afterPage = com::null_pointer<NotebookPage>()) noexcept;
		void viewPage(NotebookPage* page) noexcept;
		void removePage(NotebookPage* page) noexcept;
		template<typename T, com::CompareFunction<T> EqualTo = std::equal_to<T>>
		NotebookPage* findPage(const T& data) noexcept;

		void dump() noexcept;
	};

	template<typename T, com::CompareFunction<T> EqualTo>
	NotebookPage* NotebookView::findPage(const T& data) noexcept
	{
		NotebookPage* page = m_head;
		EqualTo isEqual { };
		while(page != com::null_pointer<NotebookPage>())
		{
			if(isEqual(*page->getData<T>(), data))
				return page;
			page = page->getNext();
		}
		return com::null_pointer<NotebookPage>();
	}
}