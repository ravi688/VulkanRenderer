#pragma once

#include <sutk/defines.hpp> /* for Rect2D */
#include <vector> /* for std::vector */
#include <sutk/UIDriver.hpp>

#include <sutk/IDebuggable.hpp> // for SUTK::IDebuggable class

namespace SUTK
{
	class Text;
	class Container;

	class Container : public UIDriverObject, public IDebuggable
	{
	private:
		std::vector<Container*> m_containers;
		Rect2D<DisplaySizeType> m_rect;
		Container* m_parent;
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
		virtual void onResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged) { }
	public:
		Container* getParent() { return m_parent; }
		void setParent(Container* parent) noexcept;
		const Container* getParent() const { return m_parent; }
		Rect2D<DisplaySizeType> getRect() const { return m_rect; }
		Vec2D<DisplaySizeType> getScreenCoordsToLocalCoords(Vec2D<DisplaySizeType> screenCoords) const;
		Vec2D<DisplaySizeType> getLocalCoordsToScreenCoords(Vec2D<DisplaySizeType> localCoords) const;
		virtual void setRect(Rect2D<DisplaySizeType> rect) { m_rect = rect; onResize(rect, true, true); }
		void setPosition(Vec2D<DisplaySizeType> pos) { m_rect.setPosition(pos); onResize(m_rect, true, false); }
		Vec2D<DisplaySizeType> getPosition() const noexcept { return m_rect.getPosition(); }
		void setSize(Vec2D<DisplaySizeType> size) { m_rect.setSize(size); onResize(m_rect, false, true); }
		Vec2D<DisplaySizeType> getSize() const noexcept { return m_rect.getSize(); }
		virtual void add(Container* container);
		virtual void remove(Container* container);

		// IMPLEMENTATION of IDebuggable
		virtual void enableDebug(bool isEnable = false) noexcept override;
	};
}
