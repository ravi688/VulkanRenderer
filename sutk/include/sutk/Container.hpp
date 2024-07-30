#pragma once

#include <sutk/defines.hpp> /* for Rect2D */
#include <vector> /* for std::vector */
#include <sutk/UIDriver.hpp>

#include <sutk/IDebuggable.hpp> // for SUTK::IDebuggable class

namespace SUTK
{
	class Text;
	class Container;
	class RenderRect;
	class RenderRectContainer;

	class Container : public UIDriverObject, public IDebuggable
	{
	private:
		std::vector<Container*> m_containers;
		Rect2Df m_rect;
		Container* m_parent;
		RenderRectContainer* m_renderRectCont;
		RenderRect* m_renderRect;
		bool m_isDebug;


	protected:
		// this can only be called by SUTK::UIDriver
		Container(SUTK::UIDriver& driver, Container* parent = NULL);

		friend class UIDriver;

		std::vector<Container*>& getContainerList() { return m_containers; }
		const std::vector<Container*>& getContainerList() const { return m_containers; }

		// called after adding this container to 'parent' container
		virtual void onAdd(Container* parent);
		// called after removing this container from 'parent' container
		virtual void onRemove(Container* parent);
		// called after rect of this container has been resized
		// isPositionChanged is set to true if position has been modified
		// isSizeChanged is set to true if size has been modified
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged);
	public:
		Container* getParent() { return m_parent; }
		void setParent(Container* parent) noexcept;
		const Container* getParent() const { return m_parent; }
		Rect2Df getRect() const { return m_rect; }
		// converts global coordinates (in centimeters) to local coordinates (in centimeters) in its rect
		Vec2Df getScreenCoordsToLocalCoords(Vec2Df screenCoords) const;
		// converts local coordinates in its rect (in centimeters) to global coordinates (in centimeters)
		Vec2Df getLocalCoordsToScreenCoords(Vec2Df localCoords) const;
		virtual void setRect(Rect2Df rect) { m_rect = rect; onResize(rect, true, true); }
		void setPosition(Vec2Df pos) { m_rect.setPosition(pos); onResize(m_rect, true, false); }
		Vec2Df getPosition() const noexcept { return m_rect.getPosition(); }
		void setSize(Vec2Df size) { m_rect.setSize(size); onResize(m_rect, false, true); }
		Vec2Df getSize() const noexcept { return m_rect.getSize(); }
		virtual void add(Container* container);
		virtual void remove(Container* container);

		// IMPLEMENTATION of IDebuggable
		virtual void enableDebug(bool isEnable = false) noexcept override;
	};
}
