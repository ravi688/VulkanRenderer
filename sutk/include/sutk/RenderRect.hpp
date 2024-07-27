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
		Rect2D<DisplaySizeType> m_rect;
		RenderRectContainer* m_container;
		Geometry m_geometry;

		friend class RenderRectContainer;
		void onContainerResize(Rect2D<DisplaySizeType> rect, bool isPositionChanged, bool isSizeChanged) noexcept;

	public:
		// Constructors
		RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept;

		// Implementation of SUTK::IRenderable interface
		virtual bool isDirty() override;
		virtual void update() override;

		Rect2D<DisplaySizeType> getRect() const noexcept { return m_rect; }

		RenderRectContainer* getContainer() noexcept { return m_container; }
	};
};
