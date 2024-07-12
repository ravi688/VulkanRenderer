#pragma once

#include <sutk/defines.hpp> /* for Rect2D */
#include <vector> /* for std::vector */

namespace SUTK
{
	class Text;
	class Container;
	class UIDriver;

	class Container
	{
	private:
		UIDriver& m_driver;
		std::vector<Container*> m_containers;
		Rect2D<DisplaySizeType> m_rect;
		Container* m_parent;

		void setParent(Container* parent) noexcept { m_parent = parent; }

	protected:
		// this can only be called by SUTK::UIDriver
		Container(SUTK::UIDriver& driver);

		friend class UIDriver;

		UIDriver& getUIDriver() { return m_driver; }
		std::vector<Container*>& getContainerList() { return m_containers; }
		const std::vector<Container*>& getContainerList() const { return m_containers; }

		// called after adding this container to 'parent' container
		virtual void onAdd(Container* parent);
		// called after removing this container from 'parent' container
		virtual void onRemove(Container* parent);

	public:
		Container* getParent() { return m_parent; }
		const Container* getParent() const { return m_parent; }
		Rect2D<DisplaySizeType> getRect() const { return m_rect; }
		Vec2D<DisplaySizeType> getLocalCoordsToScreenCoords(Vec2D<DisplaySizeType> localCoords) const;
		virtual void setRect(Rect2D<DisplaySizeType> rect) { m_rect = rect; onResize(rect, true, true); }
		void setPosition(Vec2D<DisplaySizeType> pos) { m_rect.setPosition(pos); onResize(m_rect, true, false); }
		void setSize(Vec2D<DisplaySizeType> size) { m_rect.setSize(size); onResize(m_rect, false, true); }
		virtual void add(Container* container);
		virtual void remove(Container* container);

		virtual void onResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged) { }
	};
}
