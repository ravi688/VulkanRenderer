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
	class TabView;

	class NotebookPage
	{
		friend class NotebookView;
		friend u32 getIndexOfPage(const NotebookPage* page) noexcept;
	private:
		void* m_data;
		void (*m_dataDeleter)(void*);
		std::optional<std::function<void(NotebookPage*)>> m_onPageRemove;
		TabView* m_tabView;
		Container* m_container;

		TabView* getTabView() noexcept { return m_tabView; }
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
		friend class NotebookView;
		friend class TabAnimGroup;
		friend class TabRemoveAnimation;
		friend u32 getIndexOfPage(const NotebookPage* page) noexcept;
	private:
		DefaultButtonGraphic* m_graphic;
		Button* m_closeButton;
		ImageButtonGraphic* m_closeButtonGraphic;
		NotebookPage* m_page;
		TabView* m_next;
		TabView* m_prev;
		u32 m_index;

	public:
		TabView(UIDriver& driver, Container* parent) noexcept;
		~TabView() noexcept;

		// Getters
		TabView* getNext() noexcept { return m_next; }
		const TabView* getNext() const noexcept { return m_next; }
		TabView* getPrev() noexcept { return m_prev; }
		const TabView* getPrev() const noexcept { return m_prev; }
		NotebookPage* getPage() noexcept { return m_page; }
		u32 getIndex() const noexcept { return m_index; }

		void setLabel(const std::string_view str) noexcept;
		const std::string& getLabel() noexcept;

		void unselectedState() noexcept;
		void selectedState() noexcept;
	};

	class HBoxContainer;

	class TabRemoveAnimation;
	class TabInsertAnimation;

	class TabAnimGroup : public AnimationEngine::AnimGroup
	{
	private:
		ILayoutController* m_tabLayoutController;
		std::vector<TabRemoveAnimation*> m_inFlightRemoves;
		std::vector<TabInsertAnimation*> m_inFlightInserts;	
	protected:
		virtual void onPresent(AnimationEngine::AnimContextBase* animContext) noexcept override;
		virtual void onAbsent(AnimationEngine::AnimContextBase* animContext) noexcept override;
		virtual void onWhenAnyStart() noexcept override;
		virtual void onWhenAllEnd() noexcept override;
	public:
		TabAnimGroup(UIDriver& driver, ILayoutController* tabLayoutController) noexcept;
	};

	class SUTK_API NotebookView : public VBoxContainer, public GlobalMouseMoveHandlerObject
	{
	public:
		typedef com::Event<NotebookView, NotebookPage*> OnPageSelectEvent;
	private:
		struct TabRearrangeContext
		{
			TabView* grabbedTabView;
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
		OnPageSelectEvent m_onPageSelectEvent;
		TabRearrangeContext m_tabRearrangeContext;
		com::Bool m_isRunning;
		com::Bool m_isStartAnimBatch;
		f32 m_animDuration;


		void abortTabAnim() noexcept;
		void dispatchAnimNewTab(TabView* tabView) noexcept;
		void dispatchAnimRemoveTab(TabView* tabView) noexcept;
		void dispatchAnimTabShift(TabView* tabView, com::Bool isLeft) noexcept;

	protected:
		virtual void onMouseMove(Vec2Df pos) noexcept override;

	public:
		NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::False, Layer layer = InvalidLayer) noexcept;
		~NotebookView() noexcept;

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