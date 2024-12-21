#pragma once

#include <sutk/VBoxContainer.hpp> // for SUTK::VBoxContainer
#include <sutk/Concepts.hpp> // for SUTK::ContainerT
#include <sutk/Constants.hpp> // for SUTK::Constants
#include <functional> // for std::function
#include <optional> // for std::optional
#include <common/DynamicPool.hpp> // for com::DynamicPool

namespace SUTK
{
	template<ContainerT T>
	class SUTK_API DynamicVListContainer : public VBoxContainer
	{
	public:
		typedef std::function<void(T*)> OnPostCreateHandler;
		typedef com::DynamicPool<T*> PoolType;
	private:
		// This must come before m_pool, see: onCreate
		std::optional<OnPostCreateHandler> m_onPostCreateHandler;
		PoolType m_pool;
		f32 m_width;
		f32 m_yOffset;
		com::Bool m_isResizeOnSetWidth { com::True };

	protected:
		// Invoked when a button is returned to the pool
		// Typically, this would be used to deactivate the button to make it invisible and do not consume resources
		virtual void onReturn(T* &button) noexcept = 0;
		// Invoekd when a button is recycled (re-used) from the pool
		// Typically, this would be used to reactivate the button
		virtual void onRecycle(T* &button) noexcept = 0;
		// Invoked when the pool requests a new button instance
		// Mandatory to be called in the overriding method
		virtual T* onCreate() noexcept = 0;

		PoolType& getPool() noexcept { return m_pool; }

		// Following set of functions must be called by the deriving class to correctly present ListContainer

		void invokeOnPostCreateHandler(T* obj) noexcept;
		std::pair<T*, f32> get(f32 height = 0.7f) noexcept;

		f32 getWidth() const noexcept { return m_width; }
		void setWidth(f32 width) noexcept { m_width = width; if(m_isResizeOnSetWidth) fit(); }
		
	public:
		DynamicVListContainer(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer, u32 poolSize = 7) noexcept;

		void setOnPostCreateHandler(OnPostCreateHandler handler) noexcept;
		void setResizeOnSetWidth(com::Bool isResizeOnSetWidth) noexcept { m_isResizeOnSetWidth = isResizeOnSetWidth; }

		// Call this to resize the DynamicVListContainer to fit all the buttons
		// Not calling this might need to visual artefacts
		void fit() noexcept;

		void clear() noexcept;
	};


	template<ContainerT T>
	DynamicVListContainer<T>::DynamicVListContainer(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer, u32 poolSize) noexcept : VBoxContainer(driver, parent, /*isLayoutLocked: */ true, isLayoutIgnore, layer),
																							m_onPostCreateHandler({ }),
																							m_pool(std::bind(&DynamicVListContainer::onCreate, this),
																										std::bind(&DynamicVListContainer::onReturn, this, std::placeholders::_1),
																										std::bind(&DynamicVListContainer::onRecycle, this, std::placeholders::_1),
																										true,
																										poolSize),
																							m_width(Constants::Defaults::DynamicVListContainer::Width),
																							m_yOffset(0)
	{
		setRect({ 0.0f, 0.0f, m_width, Constants::Defaults::DynamicVListContainer::Height });
		unlockLayout(true);
		setTight(true);
	}

	template<ContainerT T>
	void DynamicVListContainer<T>::invokeOnPostCreateHandler(T* obj) noexcept
	{
		if(m_onPostCreateHandler.has_value())
			(*m_onPostCreateHandler)(obj);
	}

	template<ContainerT T>
	std::pair<T*, f32> DynamicVListContainer<T>::get(f32 height) noexcept
	{
		f32 yOffset = m_yOffset;
		T* obj = m_pool.get();
		LayoutAttributes attr = obj->getLayoutAttributes();
		attr.minSize.height = height;
		attr.prefSize.height = height;
		obj->setLayoutAttributes(attr);
		m_yOffset += height;
		return { obj, yOffset };
	}

	template<ContainerT T>
	void DynamicVListContainer<T>::setOnPostCreateHandler(OnPostCreateHandler handler) noexcept
	{ 
		m_onPostCreateHandler = handler;
		for(auto& obj : m_pool)
			handler(obj);
	}

	template<ContainerT T>
	void DynamicVListContainer<T>::fit() noexcept
	{
		setSize({ m_width, getSize().height });
	}

	template<ContainerT T>
	void DynamicVListContainer<T>::clear() noexcept
	{
		m_pool.reclaim();
		m_yOffset = 0;
	}
}