#pragma once

#include <sutk/defines.hpp> /* for Rect2D */
#include <vector> /* for std::vector */
#include <sutk/UIDriverObject.hpp> // for SUTK::UIDriverObject

#include <sutk/IDebuggable.hpp> // for SUTK::IDebuggable class
#include <sutk/AnchorRect.hpp> // for SUTK::AnchorRect class
#include <sutk/Activatable.hpp> // for SUTK::Activatable class

namespace SUTK
{
	struct LayoutAttributes
	{
		// If true, then all the fields in LayoutAttributes have range [0, 1]
		// And the Layout Controller calculates the absolute values by multiplying the widths and heights of parent's rect with the normalized attribute values.
		bool isNormalized;
		// Only used by HPaneContainer::recalibrate and Container::getAbsoluteLayoutAttributes 
		// to work around layouting of handle rectangles with other containers (external Containers) in HPaneContainers.
		// It will only be used if isNormalized is true.
		// The reason we need this work around is the following equation:
		// F - ((a / I) * F + (b / I) * F) not equals to I - (a + b)
		Vec2Df offset;

		// Minimum Size Allowed for this Layout Element
		Vec2Df minSize;
		// Maximum Size Allowed for this Layout Element
		Vec2Df maxSize;
		// Prefered Size for this Layout Element
		Vec2Df prefSize;

		constexpr LayoutAttributes() noexcept : isNormalized(false), minSize({ 0, 0 }), maxSize({ F32_INFINITY, F32_INFINITY }), prefSize({ 5.0f, 5.0f }) { }
		constexpr LayoutAttributes(bool _isNormalized, Vec2Df _offset, Vec2Df _minSize, Vec2Df _maxSize, Vec2Df _prefSize) noexcept : isNormalized(_isNormalized), offset(_offset), minSize(_minSize), maxSize(_maxSize), prefSize(_prefSize) { }
	};

	class Text;
	class Container;
	class RenderRectOutline;
	class RenderableContainer;
	class IInputEventHandlerObject;

	class SUTK_API Container : public UIDriverObject, public IDebuggable, public Activatable
	{
	public:
		enum class RecycleState : u8
		{
			Undefined,
			Disposed,
			Recycled
		};
	private:
		std::vector<Container*> m_containers;
		std::vector<IInputEventHandlerObject*>* m_inputEventHandlers;
		Rect2Df m_rect;
		LayoutAttributes m_layoutAttr;
		com::Bool m_isLayoutIgnore;
		AnchorRect* m_anchorRect;
		Container* m_parent;
		RenderableContainer* m_renderRectCont;
		RenderRectOutline* m_renderRect;
		bool m_isDebug;
		RecycleState m_recycleState;
		Layer m_layer;


	protected:
		// this can only be called by SUTK::UIDriver
		Container(SUTK::UIDriver& driver, Container* parent = NULL, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer);
		virtual ~Container();

		friend class UIDriver;

		std::vector<Container*>& getContainerList() noexcept { return m_containers; }
		const std::vector<Container*>& getContainerList() const noexcept { return m_containers; }

		// called after adding this container to 'parent' container
		virtual void onAdd(Container* parent);
		// called after removing this container from 'parent' container
		virtual void onRemove(Container* parent);
		// called after adding a child 'child' to this container
		virtual void onAddChild(Container* child) { }
		// called after removing a child 'child' from this container
		virtual void onRemoveChild(Container* child) { }
		// Called before recto fo this container has been resized
		virtual void onBeforeResize(Rect2Df newRect, bool isPositionChanged, bool isSizeChanged) { }
		// called after rect of this container has been resized
		// isPositionChanged is set to true if position has been modified or has changed its position
		// isSizeChanged is set to true if size has been modified
		// NOTE: mandatory to be called in the overriding method
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

		void setParentChildRelation(Container* parent, std::size_t index = std::numeric_limits<std::size_t>::max()) noexcept;

	public:

		// IMPLEMENTATION of IDebuggable
		virtual void enableDebug(bool isEnable = true, Color4 color = Color4::green()) noexcept override;

		// Calling setLayer might be expensive if this container's height in the tree is large
		// Instead pass the layer number in the constructor while constructing this Container.
		void setLayer(Layer layer) noexcept;
		Layer getLayer() const noexcept { return m_layer; }

		// Calling this with RecyckeState other than Undefined, marks it recyclable
		void setRecycleState(RecycleState state) noexcept { m_recycleState = state; }
		RecycleState getRecycleState() const noexcept { return m_recycleState; }
		bool isRecyclable() const noexcept { return m_recycleState != RecycleState::Undefined; }

		LayoutAttributes& getLayoutAttributes() noexcept { return m_layoutAttr; }
		const LayoutAttributes& getLayoutAttributes() const noexcept { return m_layoutAttr; }
		void setLayoutAttributes(const LayoutAttributes& attrs) noexcept;
		LayoutAttributes getAbsoluteLayoutAttributes() const noexcept;
		com::Bool isLayoutIgnore() const noexcept { return m_isLayoutIgnore || com::Bool(!isActive()); }
		void setLayoutIgnore(com::Bool isIgnore) noexcept;
		void setLayoutExpand() noexcept;
		void setLayoutFixed(Vec2Df fixedSize) noexcept;

		// childs getters
		std::vector<Container*>& getChilds() noexcept { return getContainerList(); }
		const std::vector<Container*>& getChilds() const noexcept { return getContainerList(); }


		// parent getters
		Container* getParent() { return m_parent; }
		const Container* getParent() const { return m_parent; }
		// parent setters
		void setParent(Container* parent, bool isInvariantPos = true) noexcept;

		// input event handler objects getter (a workaround for a problem with virtual inheritance from IInputEventHandlerObject involving multiple virtual function overrids)
		bool hasInputEventHandlers() const noexcept { return m_inputEventHandlers != NULL; }
		std::vector<IInputEventHandlerObject*>& getInputEventHandlers() noexcept;
		const std::vector<IInputEventHandlerObject*>& getInputEventHandlers() const noexcept;

		// rect getters
		Rect2Df getRect() const { return m_rect; }
		// Size of the rect remains invariant, while position of the rect is in the global coordinate space
		Rect2Df getGlobalRect() const noexcept;
		Vec2Df getPosition() const noexcept { return m_rect.getPosition(); }
		Vec2Df getSize() const noexcept { return m_rect.getSize(); }
		// NOTE: the localCoords must be in the same coordinate space as that of this Container's Rect
		// i.e. the localCoords must be in the space of ThisContainer::getParent() container.
		// If getParent() is NULL, then this 'localCoords' shall be considered in global coordinate space.
		bool contains(Vec2Df localCoords) const noexcept { return m_rect.contains(localCoords); }
		bool containsGlobalCoords(Vec2Df globalCoords) const noexcept;
		// rect setters
		void setRect(Rect2Df rect) noexcept;
		void setPosition(Vec2Df pos) noexcept;
		void setSize(Vec2Df size) noexcept;

		// If called with true, then this Container's rect is always in-sync with its parent's rect,
		// That also means, this container's rect's position will always be { 0, 0 } (in local coordinates of its parent's rect)
		void alwaysFitInParent() noexcept;
		// Resizes this container's rect to fit into its parent's rect
		// does nothing if parent is NULL
		void fitInParent(Vec4Df margins = { }) noexcept;

		// anchor rect getters and setters
		AnchorRect* getAnchorRect() const noexcept { return m_anchorRect; }
		void setAnchorsActive(bool isActive) noexcept { return getAnchorRect()->setActive(isActive); }
		bool isAnchorsActive() const noexcept { return getAnchorRect()->isActive(); }

		// returns the number of anscestor containers of this container
		u32 getDepth() const noexcept;

		// coordinate conversion functions

		// converts global coordinates (in centimeters) to local coordinates (in centimeters) in its rect
		Vec2Df getScreenCoordsToLocalCoords(Vec2Df screenCoords) const;
		// overload for Rect2Df, does the exact same function (for the globalRect.pos) as the above function, however, it doesn't modify the globalRect.size
		Rect2Df getScreenCoordsToLocalCoords(Rect2Df globalRect) const noexcept
		{
			Vec2Df pos = getScreenCoordsToLocalCoords(globalRect.getPosition());
			return { pos, globalRect.getSize() };			
		}
		// converts local coordinates in its rect (in centimeters) to global coordinates (in centimeters)
		Vec2Df getLocalCoordsToScreenCoords(Vec2Df localCoords) const;
		// overload for Rect2Df, does the exact same function (for localRect.pos) as the above function, however, it doesn't modify the localRect.size
		Rect2Df getLocalCoordsToScreenCoords(Rect2Df localRect) const
		{ 
			Vec2Df pos = getLocalCoordsToScreenCoords(localRect.getPosition());
			return { pos, localRect.getSize() };
		}
		// Calculates rect of this container's rect relative to the passed container's rect, i.e. 'relativeTo' container
		Rect2Df getRectRelativeTo(Container* relativeTo) const noexcept
		{
			Rect2Df rect = getGlobalRect();
			if(relativeTo != NULL)
				return relativeTo->getScreenCoordsToLocalCoords(rect);
			else
				return rect;
		}

		// addition and removal of child containers

		// adds a new container as a child to this container
		virtual void add(Container* container, bool isInvariantPos = true);
		virtual void addAt(Container* container, std::size_t index, bool isInvariantPos = true);
		// removes an existing container from this container
		virtual void remove(Container* container);

		// Override of Activatable::setActive;
		virtual void setActive(com::Bool isActive) noexcept override;
	};
}
