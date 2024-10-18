#pragma once

#include <sutk/RenderRect.hpp>

namespace SUTK
{
	class RenderRectOutline : public RenderRect
	{
	private:
		f32 m_thickness;
		bool m_isThicknessDirty;

	public:
		// Constructors
		RenderRectOutline(UIDriver& driver, RenderableContainer* container, RenderMode renderMode = RenderMode::Opaque) noexcept;

		virtual bool isDirty() noexcept override;
		// Override of Renderable::update()
		virtual void update() noexcept override;

		void setThickness(f32 thickness) noexcept;
	};
}