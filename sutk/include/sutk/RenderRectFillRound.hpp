#pragma once

#include <sutk/RenderRectFill.hpp>

namespace SUTK
{
	class RenderRectFillRound : public RenderRectFill
	{
	private:
		SDF::ParameterID m_roundnessID;
		SDF::ParameterID m_widthID;
		SDF::ParameterID m_heightID;
		SDF::ParameterID m_centerID;
	public:
		RenderRectFillRound(UIDriver& driver, RenderableContainer* container) noexcept;

		// Override update()
		virtual void update() noexcept override;
	};
}