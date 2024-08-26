#pragma once

#include <sutk/Container.hpp> // for SUTK::Container
#include <sutk/ILayoutController.hpp> // for SUTK::ILayoutController

namespace SUTK
{
	class VBoxContainer : public Container, public ILayoutController
	{
	protected:
		// Overrides of Container::onAddChild, Container::onRemoveChild, and Container::onResize
		virtual void onAddChild(Container* child) override;
		virtual void onRemoveChild(Container* child) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

		// Overrides of ILayoutController::recalculateLayout
		virtual void recalculateLayout() override;
	public:
		VBoxContainer(UIDriver& driver, Container* parent = NULL) noexcept;
		~VBoxContainer() noexcept = default;
	};
}