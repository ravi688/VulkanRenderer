#pragma once

#include <sutk/RenderRect.hpp>

namespace SUTK
{
	class RenderImage : public RenderRect
	{
	public:
		// Constructors
		RenderImage(UIDriver& driver, RenderableContainer* container, bool isUpdate = true) noexcept;

		// Override of RenderRect::update()
		virtual void update() noexcept override;
	};
}
