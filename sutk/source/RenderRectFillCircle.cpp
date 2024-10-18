#include <sutk/RenderRectFillCircle.hpp>
#include <sutk/RenderableContainer.hpp>

namespace SUTK
{
	RenderRectFillCircle::RenderRectFillCircle(UIDriver& driver, RenderableContainer* container, RenderMode renderMode) noexcept : RenderRectFill(driver, container, renderMode, false)
	{
		auto& geometry = getGeometry().setSDF(true);

		SDF& sdf = geometry.getSDF();
		m_sharpnessID = sdf.addParameter<f32>("sharpness", 1.0f);
		m_widthID = sdf.addParameter<InvariantWidth>("width", getRect().width);
		m_heightID = sdf.addParameter<InvariantHeight>("height", getRect().height);
		m_numGradPixelsID = sdf.addParameter<f32>("numGradPixels", 4);
		m_centerID = sdf.addParameter<InvariantSize>("center", getContainer()->getLocalCoordsToScreenCoords(getRect().getCenter()));

		const char* str = 

		"vec2 radiusVector = fragCoord.xy - center.xy;\n"

		"vec2 ray = normalize(radiusVector);\n"
		"float k = sqrt(1.0 / ((ray.x * ray.x) / (width * width * 0.25) + (ray.y * ray.y) / (height * height * 0.25)));\n"
		"vec2 point = k * ray;\n"
		"float diff = length(radiusVector) - (length(point) - numGradPixels);\n"
		"if(diff > 0.0f)\n"
		"	alpha = pow(1.0 - diff / numGradPixels, sharpness);\n";

		sdf.setCode(str);

		update();
	}

	void RenderRectFillCircle::update() noexcept
	{
		SDF& sdf = getGeometry().getSDF();
		if(isSizeDirty())
		{
			sdf.setParameterValue<InvariantWidth>(m_widthID, getRect().width);
			sdf.setParameterValue<InvariantHeight>(m_heightID, getRect().height);
			sdf.setParameterValue<InvariantSize>(m_centerID, getContainer()->getLocalCoordsToScreenCoords(getRect().getCenter()));
		}
		if(isPosDirty())
		{
			sdf.setParameterValue<InvariantSize>(m_centerID, getContainer()->getLocalCoordsToScreenCoords(getRect().getCenter()));
		}

		RenderRectFill::update();
	}

	void RenderRectFillCircle::setSharpness(f32 sharpness) noexcept
	{
		SDF& sdf = getGeometry().getSDF();
		sdf.setParameterValue<f32>(m_sharpnessID, sharpness);
	}

	void RenderRectFillCircle::setNumGradPixels(f32 numGradPixels) noexcept
	{
		SDF& sdf = getGeometry().getSDF();
		sdf.setParameterValue<f32>(m_numGradPixelsID, numGradPixels);
	}
}