#pragma once

#include <sutk/Container.hpp> // for SUTK::Container
#include <sutk/ILayoutController.hpp> // for SUTK::ILayoutController

namespace SUTK
{
	class VBoxContainer : public Container, public ILayoutController
	{
	private:
		bool m_isTight;
		bool m_isInsideOnResize;
	protected:
		// Overrides of Container::onAddChild, Container::onRemoveChild, and Container::onResize
		virtual void onAddChild(Container* child) override;
		virtual void onRemoveChild(Container* child) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

		// Overrides of ILayoutController::onRecalculateLayout
		virtual void onRecalculateLayout() noexcept override;;
	public:
		VBoxContainer(UIDriver& driver, Container* parent = NULL, bool isLayoutLocked = false, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer) noexcept;
		~VBoxContainer() noexcept = default;
		
		// resizes this container to tightly fit its child containers
		// basically, it just removes any empty spaces
		void tight() noexcept;
		void setTight(bool isTight) noexcept;
	};
}