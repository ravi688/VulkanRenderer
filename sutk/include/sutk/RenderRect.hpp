#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> // for SUTK::Renderable abstract class
#include <sutk/Geometry.hpp> // for SUTK::Geometry

namespace SUTK
{
	class RenderRect : public GeometryRenderable
	{
	private:
		Rect2Df m_rect;

	protected:
		void setRect(const Rect2Df& rect) noexcept { m_rect = rect; }
		Rect2Df getRect() const noexcept { return m_rect; }

	public:
		// Constructors
		RenderRect(UIDriver& driver, RenderableContainer* container) noexcept : GeometryRenderable(driver, container), m_rect { 0, 0, 2.0f, 2.0f } { }

		// Implementation of Renderable
		virtual bool isDirty() = 0;
		virtual void update() = 0;
	};

	class RenderRectOutline : public RenderRect
	{
	private:
		bool m_isPosDirty;
		bool m_isSizeDirty;
		f32 m_thickness;

	protected:
		// Overrides of Renderable::onGlobalCoordDirty and Renderable::onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

	public:
		// Constructors
		RenderRectOutline(UIDriver& driver, RenderableContainer* container) noexcept;

		// Implementation of Renderable
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
		// Overrides of Renderable::onGlobalCoordDirty and Renderable::onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

	public:
		// Constructors
		RenderRectFill(UIDriver& driver, RenderableContainer* container) noexcept;

		// Implementation of RenderRect
		virtual bool isDirty() override;
		virtual void update() override;

		void setColor(Color3 color) noexcept;
	};
};
