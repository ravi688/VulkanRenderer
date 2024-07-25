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
		Rect2D<DisplaySizeType> m_rect;
		RenderRectContainer* m_container;
		Geometry m_geometry;
	public:
		// Constructors
		RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept;

		// Implementation of SUTK::IRenderable interface
		virtual bool isDirty() override;
		virtual void update() override;

		void setRect(Rect2D<DisplaySizeType> rect) noexcept;
		Rect2D<DisplaySizeType> getRect() const noexcept { return m_rect; }

		RenderRectContainer* getContainer() noexcept { return m_container; }
	};
};
