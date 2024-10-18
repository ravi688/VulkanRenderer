#pragma once

#include <sutk/Container.hpp> // for SUTK::Container
#include <sutk/ContainerController.hpp> // for SUTK::ContainerController
#include <sutk/IRunnable.hpp> // for SUTK::IRunnable
#include <sutk/Concepts.hpp> // for SUTK::ContainerT concept

namespace SUTK
{
	// It is an abstract class and it should never be instantiated without a deriving class
	template<ContainerT ContainerType>
	class SUTK_API TransientContainer : public ContainerType
	{
	private:
		ContainerController m_controller;
		bool m_isHidden;
	protected:
		// Must be called in the constructor of the derived class for the first time
		// The reason we do this way is that, the user code may add child containers which will remain active even if 
		// this container collectively (including its children recursively) was supposted to be hidden (inactive).
		// Giving more control to the deriving class allows it to apply in-active state after all the child containers have
		// been added.
		void restoreState() noexcept;
	public:
		template<typename... Args>
		TransientContainer(UIDriver& driver, Container* parent, bool isShow, Args&&... args) noexcept;
		virtual ~TransientContainer() noexcept;

		bool isHidden() noexcept { return m_isHidden; }

		// Mandatory to be called in the overriding method
		virtual void show() noexcept;
		// Mandatory to be called in the overriding method
		virtual void hide() noexcept;

		ContainerController& getController() noexcept { return m_controller; }
	};

	template<ContainerT ContainerType>
	template<typename... Args>
	TransientContainer<ContainerType>::TransientContainer(UIDriver& driver, Container* parent, bool isShow, Args&&... args) noexcept : ContainerType(driver, parent, std::forward<Args&&>(args)...), m_controller(this), m_isHidden(!isShow)
	{
		driver.addRunnable(&m_controller);
	}

	template<ContainerT ContainerType>
	TransientContainer<ContainerType>::~TransientContainer() noexcept
	{
		ContainerType::getUIDriver().removeRunnable(&m_controller);
	}

	template<ContainerT ContainerType>
	void TransientContainer<ContainerType>::restoreState() noexcept
	{
		if(m_isHidden)
			m_controller.hideImmediate();
		else
			m_controller.presentImmediate();
	}

	template<ContainerT ContainerType>
	void TransientContainer<ContainerType>::show() noexcept
	{
		m_controller.present();
		m_isHidden = false;
	}

	template<ContainerT ContainerType>
	void TransientContainer<ContainerType>::hide() noexcept
	{
		m_controller.hide();
		m_isHidden = true;
	}
}