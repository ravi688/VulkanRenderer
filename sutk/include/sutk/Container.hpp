#pragma once

#include <sutk/defines.hpp> /* for Rect2D */
#include <vector> /* for std::vector */
#include <sutk/UIDriver.hpp>

#include <sutk/IDebuggable.hpp> // for SUTK::IDebuggable class
#include <sutk/AnchorRect.hpp> // for SUTK::AnchorRect class
#include <sutk/Activatable.hpp> // for SUTK::Activatable class

namespace SUTK
{
	struct LayoutAttributes
	{
		// Minimum Size Allowed for this Layout Element
		Vec2Df minSize;
		// Maximum Size Allowed for this Layout Element
		Vec2Df maxSize;
		// Prefered Size for this Layout Element
		Vec2Df prefSize;
	};

	class Text;
	class Container;
	class RenderRectOutline;
	class RenderableContainer;

	class Container : public UIDriverObject, public IDebuggable, public Activatable
	{
	private:
		std::vector<Container*> m_containers;
		Rect2Df m_rect;
		LayoutAttributes m_layoutAttr;
		bool m_isLayoutIgnore;
		AnchorRect* m_anchorRect;
		Container* m_parent;
		RenderableContainer* m_renderRectCont;
		RenderRectOutline* m_renderRect;
		bool m_isDebug;


	protected:
		// this can only be called by SUTK::UIDriver
		Container(SUTK::UIDriver& driver, Container* parent = NULL, bool isLayoutIgnore = false);
		virtual ~Container();

		friend class UIDriver;

		std::vector<Container*>& getContainerList() { return m_containers; }
		const std::vector<Container*>& getContainerList() const { return m_containers; }

		// called after adding this container to 'parent' container
		virtual void onAdd(Container* parent);
		// called after removing this container from 'parent' container
		virtual void onRemove(Container* parent);
		// called after adding a child 'child' to this container
		virtual void onAddChild(Container* child) { }
		// called after removing a child 'child' from this container
		virtual void onRemoveChild(Container* child) { }
		// called after rect of this container has been resized
		// isPositionChanged is set to true if position has been modified or has changed its position
		// isSizeChanged is set to true if size has been modified
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged);
		// called after rect of its parent container has been resized or has changed its position
		// NOTE: mandatory to be called in the overriding method
		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged);

		// This is to expose private m_rect reference to deriving classes, and let them modify
		// it without any unindented side effects.
		// For example, if the deriving class were to use setSize() instead, it might lead to infinite recursion
		// if the setSize() was called from the same call-hierarchy in which onResize() was.
		Rect2Df& getRawRectRef() { return m_rect; }

		void recalculateLayoutParent() noexcept;

		void setParentChildRelation(Container* parent) noexcept;

	public:

		// IMPLEMENTATION of IDebuggable
		virtual void enableDebug(bool isEnable = true, Color4 color = Color4::green()) noexcept override;

		LayoutAttributes& getLayoutAttributes() noexcept { return m_layoutAttr; }
		const LayoutAttributes& getLayoutAttributes() const noexcept { return m_layoutAttr; }
		void setLayoutAttributes(const LayoutAttributes& attrs) noexcept;
		bool isLayoutIgnore() const noexcept { return m_isLayoutIgnore; }
		void setLayoutIgnore(bool isIgnore) noexcept;

		// childs getters
		std::vector<Container*>& getChilds() noexcept { return getContainerList(); }

		// parent getters
		Container* getParent() { return m_parent; }
		const Container* getParent() const { return m_parent; }
		// parent setters
		void setParent(Container* parent) noexcept;

		// rect getters
		Rect2Df getRect() const { return m_rect; }
		Vec2Df getPosition() const noexcept { return m_rect.getPosition(); }
		Vec2Df getSize() const noexcept { return m_rect.getSize(); }
		// NOTE: the localCoords must be in the same coordinate space as that of this Container's Rect
		// i.e. the localCoords must be in the space of ThisContainer::getParent() container.
		// If getParent() is NULL, then this 'localCoords' shall be considered in global coordinate space.
		bool contains(Vec2Df localCoords) const noexcept { return m_rect.contains(localCoords); }
		bool containsGlobalCoords(Vec2Df globalCoords) const noexcept;
		// rect setters
		virtual void setRect(Rect2Df rect) { m_rect = rect; onResize(rect, true, true); }
		void setPosition(Vec2Df pos) { m_rect.setPosition(pos); onResize(m_rect, true, false); }
		void setSize(Vec2Df size) { m_rect.setSize(size); onResize(m_rect, false, true); }

		// anchor rect getters
		AnchorRect* getAnchorRect() const noexcept { return m_anchorRect; }

		// returns the number of anscestor containers of this container
		u32 getDepth() const noexcept;

		// coordinate conversion functions

		// converts global coordinates (in centimeters) to local coordinates (in centimeters) in its rect
		Vec2Df getScreenCoordsToLocalCoords(Vec2Df screenCoords) const;
		// converts local coordinates in its rect (in centimeters) to global coordinates (in centimeters)
		Vec2Df getLocalCoordsToScreenCoords(Vec2Df localCoords) const;
		// overload for Rect2Df, does the exact same function as the above function, however, it doesn't modify the localRect.size
		Rect2Df getLocalCoordsToScreenCoords(Rect2Df localRect) const
		{ 
			Vec2Df pos = getLocalCoordsToScreenCoords(localRect.getPosition());
			return { pos, localRect.getSize() };
		}

		// addition and removal of child containers

		// adds a new container as a child to this container
		virtual void add(Container* container);
		// removes an existing container from this container
		virtual void remove(Container* container);
	};
}
