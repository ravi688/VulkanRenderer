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

	protected:
		UIDriver& getUIDriver() { return m_driver; }
		std::vector<Container*>& getContainerList() { return m_containers; }
		const std::vector<Container*>& getContainerList() const { return m_containers; }

		virtual void onAdd(Container* parent);
		virtual void onRemove(Container* parent);

	public:
		Container(SUTK::UIDriver& driver);

		Rect2D<DisplaySizeType> getRect() const { return m_rect; }
		virtual void setRect(Rect2D<DisplaySizeType> rect) { m_rect = rect; }
		virtual void add(Container* container);
		virtual void remove(Container* container);
	};
}
