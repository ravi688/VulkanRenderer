#pragma once

#include <sutk/RenderRect.hpp>

namespace SUTK
{
	class RenderRectFill : public RenderRect
	{
	public:
		// Constructors
		RenderRectFill(UIDriver& driver, RenderableContainer* container, RenderMode renderMode = RenderMode::Opaque, bool isUpdate = true) noexcept;

		// Override of RenderRect::update()
		virtual void update() noexcept override;
	};
}
