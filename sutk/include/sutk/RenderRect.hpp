#pragma once

#include <sutk/defines.hpp>
#include <sutk/IRenderable.hpp> // for SUTK::IRenderable interfae
#include <sutk/UIDriver.hpp> // for SUTK::UIDriverObject
#include <sutk/Geometry.hpp> // for SUTK::Geometry

namespace SUTK
{
	class RenderRectContainer;

	class RenderRect : public UIDriverObject, public IRenderable
	{
	private:
		GfxDriverObjectHandleType m_handle;
		Rect2Df m_rect;
		RenderRectContainer* m_container;
		Geometry m_geometry;

	protected:

		Geometry& getGeometry() noexcept { return m_geometry; }
		void setRect(const Rect2Df& rect) { m_rect = rect; }
		GfxDriverObjectHandleType getGfxDriverObjectHandle() const { return m_handle; }
		void setGfxDriverObjectHandle(GfxDriverObjectHandleType handle) { m_handle = handle; }

		friend class RenderRectContainer;
		// Rendering Geometries (or Text) requires conversion from local coordinates to global coordinates (target rendering backend, SGE)
		// So, whenever any of the Anscestor containers change their position, the old transformed coordinates become invalid (dirty),
		// and this requires to walk recursively up the container tree to finally calculate the new global coordiantes with possibly same
		// local coordinates.
		// This function onGlobalCoordDirty is called when any of the Anscestor containers change their local position.
		virtual void onGlobalCoordDirty() noexcept { }
		// This function onContainerResize is called whenever the container containing this renderable changes its size or local position.
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept { }

	public:
		RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept : 
					UIDriverObject(driver),
					m_handle(GFX_DRIVER_OBJECT_NULL_HANDLE), 
					m_rect({0, 0, 1, 1}), 
					m_container(container), 
					m_geometry(driver) { }

		// Implementation of SUTK::IRenderable
		virtual bool isDirty() = 0;
		virtual void update() = 0;

		// Getters
		RenderRectContainer* getContainer() noexcept { return m_container; }
		const Rect2Df& getRect() const { return m_rect; }
	};

	class RenderRectOutline : public RenderRect
	{
	private:
		bool m_isPosDirty;
		bool m_isSizeDirty;
		f32 m_thickness;

	protected:
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

	public:
		// Constructors
		RenderRectOutline(UIDriver& driver, RenderRectContainer* container) noexcept;

		// Implementation of SUTK::RenderRect
		virtual bool isDirty() override;
		virtual void update() override;

		void setThickness(f32 thickness) noexcept;
	};

	class RenderRectFill : public RenderRect
	{
	private:
		bool m_isPosDirty;
		bool m_isSizeDirty;
		bool m_isColorDirty;
		Color3 m_color;

	protected:
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

	public:
		// Constructors
		RenderRectFill(UIDriver& driver, RenderRectContainer* container) noexcept;

		// Implementation of SUTK::RenderRect
		virtual bool isDirty() override;
		virtual void update() override;

		void setColor(Color3 color) noexcept;
	};
};
