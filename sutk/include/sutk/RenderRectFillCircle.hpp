#pragma once

#include <sutk/RenderRectFill.hpp>

namespace SUTK
{
	class RenderRectFillCircle : public RenderRectFill
	{
	private:
		SDF::ParameterID m_roundnessID;
		SDF::ParameterID m_sharpnessID;
		SDF::ParameterID m_numGradPixelsID;
		SDF::ParameterID m_widthID;
		SDF::ParameterID m_heightID;
		SDF::ParameterID m_centerID;
	public:
		RenderRectFillCircle(UIDriver& driver, RenderableContainer* container) noexcept;

		// Override update()
		virtual void update() noexcept override;

		void setRoundness(f32 roundness) noexcept;
		void setSharpness(f32 sharpness) noexcept;
		void setNumGradPixels(f32 numGradPixels) noexcept;
	};
}