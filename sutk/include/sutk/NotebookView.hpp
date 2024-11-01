#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/VBoxContainer.hpp> // for SUTK::VBoxContainer
#include <sutk/Button.hpp> // for SUTK::Button
#include <sutk/Label.hpp> // for SUTK::Label::set()

#include <common/Concepts.hpp> // for com::CompareFunction


namespace SUTK
{
	class TabView;

	class NotebookPage
	{
		friend class NotebookView;
	private:
		void* m_data;
		void (*m_dataDeleter)(void*);
		TabView* m_tabView;
		Container* m_container;
		NotebookPage* m_next;
		NotebookPage* m_prev;

		NotebookPage* getNext() noexcept { return m_next; }
		const NotebookPage* getNext() const noexcept { return m_next; }
		NotebookPage* getPrev() noexcept { return m_prev; }
		const NotebookPage* getPrev() const noexcept { return m_prev; }

		TabView* getTabView() noexcept { return m_tabView; }
	public:
		NotebookPage(TabView* tabView, Container* container) noexcept;
		~NotebookPage() noexcept;
		void setLabel(const std::string_view str) noexcept;
		Container* getContainer() noexcept { return m_container; }
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

		void setLabel(const std::string_view str) noexcept;

		void unselectedState() noexcept;
		void selectedState() noexcept;
	};

	class HBoxContainer;

	class SUTK_API NotebookView : public VBoxContainer
	{
	private:
		// TextGroup for TabView(s)
		TextGroupContainer* m_textGroupContainer;
		HBoxContainer* m_tabContainer;
		Container* m_pageContainer;
		NotebookPage* m_head;
		NotebookPage* m_currentPage;
	public:
		NotebookView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::False, Layer layer = InvalidLayer) noexcept;
		NotebookPage* createPage(const std::string_view labelStr) noexcept;
		void viewPage(NotebookPage* page) noexcept;
		void removePage(NotebookPage* page) noexcept;
		template<typename T, com::CompareFunction<T> EqualTo = std::equal_to<T>>
		NotebookPage* findPage(const T& data) const noexcept;
	};

	template<typename T, com::CompareFunction<T> EqualTo>
	NotebookPage* NotebookView::findPage(const T& data) const noexcept
	{
		const NotebookPage* page = m_head;
		EqualTo isEqual { };
		while(page != com::null_pointer<NotebookPage>())
		{
			if(isEqual(*page->getData<T>, data))
				return page;
			page = page->getNext();
		}
		return com::null_pointer<NotebookPage>();
	}
}