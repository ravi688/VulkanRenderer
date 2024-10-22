#pragma once

#include <sutk/HBoxContainer.hpp>
#include <sutk/InputEventHandlerObject.hpp>

#include <array> // for std::array

namespace SUTK
{
	class RenderableContainer;
	
	class HPaneContainer : public HBoxContainer,
							public MouseClickHandlerObject,
							public MouseMoveHandlerObject
	{
	private:
		bool m_isHandleRect;
		std::array<Container*, 3> m_grabbedHandle;
		struct HandleAdjacencyData
		{
			Container* left;
			Container* right;
		};
		std::vector<std::pair<RenderableContainer*, HandleAdjacencyData>> m_handleRects;
		std::vector<Container*> m_externalChilds;
		Vec2Df m_prevPos;
		bool m_isCalibratedForFirstTime;
	protected:
		virtual bool onMouseClick(MouseButton button, KeyEvent action) override;
		virtual bool onMouseMove(MouseMoveEvent event, Vec2Df position) override;

		virtual void add(Container* child, bool isInvariantPos = true) override;
		virtual void remove(Container* child) override;

		virtual void onBeforeResize(Rect2Df newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onRecalculateLayout() noexcept override;
	public:
		HPaneContainer(UIDriver& driver, Container* parent = NULL) noexcept;
		virtual ~HPaneContainer() noexcept = default;

		// The user code may also call this explicitly to recalculate normalized layout attributes of external containers
		void recalibrate() noexcept;

		std::vector<Container*>& getExternChilds() noexcept { return m_externalChilds; }
		const std::vector<Container*>& getExternChilds() const noexcept { return m_externalChilds; }
	};
}