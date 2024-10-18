#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> // for SUTK::Renderable abstract class
#include <sutk/IColorable.hpp> // for SUTK::IColorable

namespace SUTK
{
	class RenderRect : public GeometryRenderable, public IColorable
	{
	private:
		Rect2Df m_rect;
		Color4 m_color;
		bool m_isActiveDirty;
		bool m_isPosDirty;
		bool m_isSizeDirty;
		bool m_isColorDirty;
		bool m_isGeometryDirty;

	protected:
		void setRect(const Rect2Df& rect) noexcept;
		Rect2Df getRect() const noexcept { return m_rect; }

		bool isPosDirty() const noexcept { return m_isPosDirty; }
		bool isSizeDirty() const noexcept { return m_isSizeDirty; }
		bool isRectDirty() const noexcept { return isPosDirty() || isSizeDirty(); }

		void setGeometryDirty(bool isDirty) noexcept { m_isGeometryDirty = isDirty; }

		// Overrides of Renderable::onGlobalCoordDirty, Renderable::onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

		// Constructors
		RenderRect(UIDriver& driver, RenderableContainer* container, RenderMode renderMode = RenderMode::Opaque) noexcept;

	public:

		// Implementation of Renderable
		virtual bool isDirty() noexcept;
		virtual void update() noexcept;
		virtual void updateNormalizedDrawOrder(f32 normalizedDrawOrder) noexcept;

		virtual void setActive(bool isActive) noexcept override;

		Color4 getColor() const noexcept { return m_color; }
		void setColor(const Color4 color) noexcept;
	};
};
