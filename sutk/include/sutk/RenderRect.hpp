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
		bool m_isDirty;
		bool m_isPosDirty;
		bool m_isSizeDirty;
		Rect2Df m_rect;
		RenderRectContainer* m_container;
		Geometry m_geometry;
		f32 m_thickness;

		friend class RenderRectContainer;
		void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept;

	public:
		// Constructors
		RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept;

		// Implementation of SUTK::IRenderable interface
		virtual bool isDirty() override;
		virtual void update() override;

		void setThickness(f32 thickness) noexcept;
		Rect2Df getRect() const noexcept { return m_rect; }

		RenderRectContainer* getContainer() noexcept { return m_container; }
	};
};
