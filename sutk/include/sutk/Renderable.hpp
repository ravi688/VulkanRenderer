#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp>  // for SUTK::UIDriverObject
#include <sutk/Geometry.hpp> // for SUTK::Geometry
#include <sutk/Activatable.hpp> // for SUTK::Activatable

namespace SUTK
{
	class RenderableContainer;

	// This abstract class should be derived by any class which models a visual element, i.e. which can be seen on the display as a graphical object

	class Renderable : public UIDriverObject, public Activatable
	{
		friend class UIDriver;
		friend class Container;
		friend class RenderableContainer;

	private:
		RenderableContainer* m_container;
		u32 m_drawOrder;
		bool m_isDrawOrderDirty;
		// Initially it is set to false
		com::Bool m_isActiveDirty { };

		// This must never be called outside of Renderable (at least for now)
		void setDrawOrder(u32 drawOrder) { m_drawOrder = drawOrder;  m_isDrawOrderDirty = true; }

	protected:
		// Rendering Geometries (or Text) requires conversion from local coordinates to global coordinates (target rendering backend, SGE)
		// So, whenever any of the Anscestor containers change their position, the old transformed coordinates become invalid (dirty),
		// and this requires to walk recursively up the container tree to finally calculate the new global coordiantes with possibly same
		// local coordinates.
		// This function onGlobalCoordDirty is called when any of the Anscestor containers change their local position or any of the anscestors themselves get changed (setParent() calls)
		virtual void onGlobalCoordDirty() noexcept { }
		// This function onContainerResize is called whenever the container containing this renderable changes its size or local position.
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept { }
		// This function is called by RenderableContainer whenever its own onAnscestorChange() is invoked
		// i.e. this function is called whenever the RenderableContainer's anscestor is changed
		// The new anscestor is passed into this function.
		virtual void onAnscestorChange(Container* anscestor) noexcept;
		// updates draw order (z-buffer) values to the GPU side memory.
		// mandatory to be called in the overridng method
		virtual void updateNormalizedDrawOrder(f32 normalizedDrawOrder) { m_isDrawOrderDirty = false; }
		// This function onActiveUpdate() is called inside Renderable::update() whenever isActiveDirty() returns true
		// A deriving class can override this method to responde to activate and deactivate status changes. 
		// isActive: new value
		virtual void onActiveUpdate(com::Bool isActive) noexcept { }

	public:
		Renderable(UIDriver& driver, RenderableContainer* container = NULL) noexcept;
		virtual ~Renderable() noexcept;

		// Override of Activatable::setActive()
		virtual void setActive(com::Bool isActive) noexcept override final;

		// returns true, if GPU side data is out of sync with respect to the CPU side data, otherwise false 
		// Must be called in the overriding method
		virtual bool isDirty();
		// updates (copies CPU side data to) GPU side data, and it may also create or recreate exisiting GPU Driver objects 
		// Must be called in the overriding method
		virtual void update();

		bool isDrawOrderDirty() const noexcept { return m_isDrawOrderDirty; }
		com::Bool isActiveDirty() const noexcept { return m_isActiveDirty; }
		// Example:
		// 	In Frame 0:
		//		setActive(com::False)
		//		isActiveForThisFrame() -> returns com::True
		// In Frame 1:
		//		isActiveForThisFrame() -> returns com::False, given that update() has been called
		com::Bool isActiveForThisFrame() const noexcept { return isActiveDirty() || isActive(); }

		u32 getDrawOrder() const noexcept { return m_drawOrder; }

		bool ensureUpdated() noexcept
		{ 
			if(isDirty()) 
			{ 
				update(); 
				return true; 
			} 
			return false; 
		}

		// Getters
		RenderableContainer* getContainer() noexcept { return m_container; }
	};

	class GfxDriverRenderable : public Renderable
	{
	private:
		GfxDriverObjectHandleType m_handle;

	protected:
		void setGfxDriverObjectHandle(GfxDriverObjectHandleType handle) { m_handle = handle; }

	public:
		GfxDriverRenderable(UIDriver& driver, RenderableContainer* container = NULL) noexcept : Renderable(driver, container), m_handle(GFX_DRIVER_OBJECT_NULL_HANDLE) { }
		virtual ~GfxDriverRenderable() = default;

		virtual void destroy() { }

		void setClipRectGlobalCoords(const Rect2Df rect) noexcept;

		// sets the clip rect in the local coordinates of the parent container of its renderable container
		// if parent is NULL then it does it in the local coordinates of its renderable container
		void setClipRect(const Rect2Df rect) noexcept;
		
		GfxDriverObjectHandleType getGfxDriverObjectHandle() const { return m_handle; }
	};

	class GeometryRenderable : public GfxDriverRenderable
	{
	private:
		Geometry m_geometry;
	protected:
		GeometryRenderable(UIDriver& driver, RenderableContainer* container = NULL, RenderMode renderMode = RenderMode::Opaque) noexcept : GfxDriverRenderable(driver, container), m_geometry(driver, renderMode) { }
		virtual ~GeometryRenderable() = default;
		Geometry& getGeometry() noexcept { return m_geometry; }
	};
}