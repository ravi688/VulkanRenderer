#pragma once

#include <sutk/Container.hpp>
#include <sutk/InputEventHandlerObject.hpp>

namespace SUTK
{
	class RenderRectFill;
	
	class HPaneContainer : public Container,
							public MouseClickHandlerObject,
							public MouseMoveHandlerObject
	{
	private:
		bool m_isGrabbed;
		std::vector<RenderRectFill*> m_handleRects;
		std::vector<Container*> m_externalChilds;
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action) override;
		virtual void onMouseMove(Vec2Df position) override;

		virtual void onAddChild(Container* child) override;
		virtual void onRemoveChild(Container* child) override;
	public:
		HPaneContainer(UIDriver& driver, Container* parent = NULL) noexcept;
		virtual ~HPaneContainer() noexcept = default;

		std::vector<Container*>& getExternChilds() noexcept { return m_externalChilds; }
		const std::vector<Container*>& getExternChilds() const noexcept { return m_externalChilds; }
	};
}