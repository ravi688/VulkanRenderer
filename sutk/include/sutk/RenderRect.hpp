#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> // for SUTK::Renderable abstract class
#include <sutk/Geometry.hpp> // for SUTK::Geometry
#include <sutk/IColorable.hpp> // for SUTK::IColorable

namespace SUTK
{
	class RenderRect : public GeometryRenderable, public IColorable
	{
	private:
		Rect2Df m_rect;
		Color4 m_color;

	protected:
		void setRect(const Rect2Df& rect) noexcept { m_rect = rect; }
		Rect2Df getRect() const noexcept { return m_rect; }


	public:
		// Constructors
		RenderRect(UIDriver& driver, RenderableContainer* container) noexcept : GeometryRenderable(driver, container), m_rect { 0, 0, 2.0f, 2.0f }, m_color(Color4::white()) { }

		// Implementation of Renderable
		virtual bool isDirty() = 0;
		virtual void update() = 0;

		virtual Color4 getColor() const noexcept override { return m_color; }
		// it must be called in the overriding method
		virtual void setColor(const Color4 color) noexcept override { m_color = color; }
	};

	class RenderRectOutline : public RenderRect
	{
	private:
		bool m_isPosDirty;
		bool m_isSizeDirty;
		bool m_isColorDirty;
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

		virtual void setColor(Color4 color) noexcept override;
		void setThickness(f32 thickness) noexcept;
	};

	class RenderRectFill : public RenderRect
	{
	private:
		bool m_isPosDirty;
		bool m_isSizeDirty;
		bool m_isColorDirty;

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

		virtual void setColor(Color4 color) noexcept override;
	};
};
