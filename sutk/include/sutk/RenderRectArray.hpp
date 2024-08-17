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
		void onGlobalCoordDirty() noexcept override;
		void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept;


	public:
		// Constructors
		RenderRectFillArray(UIDriver& driver, RenderableContainer* container) noexcept;

		// Implementation of Renderable
		virtual bool isDirty();
		virtual void update();

		// Overrides of Renderable::setActive
		virtual void setActive(bool isActive) noexcept override;

		const std::vector<Rect2Df>& getRects() const noexcept { return m_rects; }
		std::vector<Rect2Df>& getRectsForWrite() noexcept;

		virtual Color4 getColor() const noexcept { return m_color; }
		// it must be called in the overriding method
		virtual void setColor(const Color4 color) noexcept;
	};
}