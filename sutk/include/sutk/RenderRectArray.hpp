#pragma once

#include <sutk/defines.hpp>		// for SUTK_API
#include <sutk/Renderable.hpp> // for SUTK::Renderable abstract class
#include <sutk/IColorable.hpp> // for SUTK::IColorable

namespace SUTK
{
	class SUTK_API RenderRectFillArray : public GeometryRenderable, public IColorable
	{
	private:
		Color4 m_color;
		bool m_isColorDirty;
		bool m_isActiveDirty;
		std::vector<Rect2Df> m_rects;
		bool m_isRectsDirty;

		// overrides of Renderable::onGlobalCoordDirty(), and onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;


	public:
		// Constructors
		RenderRectFillArray(UIDriver& driver, RenderableContainer* container, RenderMode renderMode = RenderMode::Opaque) noexcept;

		// Implementation of Renderable
		virtual bool isDirty() override;
		virtual void update() override;

		// Overrides of Renderable::setActive
		virtual void setActive(com::Bool isActive) noexcept override;

		const std::vector<Rect2Df>& getRects() const noexcept { return m_rects; }
		std::vector<Rect2Df>& getRectsForWrite() noexcept;

		virtual Color4 getColor() const noexcept override { return m_color; }
		// it must be called in the overriding method
		virtual void setColor(const Color4 color) noexcept override;
	};
}