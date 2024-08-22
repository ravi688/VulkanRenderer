#include <sutk/RenderRectFillRound.hpp>
#include <sutk/RenderableContainer.hpp>

namespace SUTK
{
	RenderRectFillRound::RenderRectFillRound(UIDriver& driver, RenderableContainer* container) noexcept : RenderRectFill(driver, container, false)
	{
		auto& geometry = getGeometry().setSDF(true);

		SDF& sdf = geometry.getSDF();
		m_roundnessID = sdf.addParameter<f32>("roundness", 0.5f);
		sdf.addParameter<f32>("sharpness", 1.0f);
		m_widthID = sdf.addParameter<InvariantWidth>("width", getRect().width);
		m_heightID = sdf.addParameter<InvariantHeight>("height", getRect().height);
		sdf.addParameter<f32>("numGradPixels", 4);
		m_centerID = sdf.addParameter<InvariantSize>("center", getContainer()->getLocalCoordsToScreenCoords(getRect().getCenter()));

		const char* str = 

		"float radius = roundness * height * 0.5f;\n"
		"vec2 topLeftCOC = vec2(-width * 0.5f + radius, height * 0.5f - radius) + center.xy;\n"
		"vec2 bottomLeftCOC = vec2(-width * 0.5f + radius, -height * 0.5f + radius) + center.xy;\n"
		"vec2 bottomRightCOC = vec2(width * 0.5f - radius, -height * 0.5f + radius) + center.xy;\n"
		"vec2 topRightCOC = vec2(width * 0.5f - radius, height * 0.5f - radius) + center.xy;\n"

		"vec2 topLeftDir = fragCoord.xy - topLeftCOC;\n"
		"vec2 bottomLeftDir = fragCoord.xy - bottomLeftCOC;\n"
		"vec2 bottomRightDir = fragCoord.xy - bottomRightCOC;\n"
		"vec2 topRightDir = fragCoord.xy - topRightCOC;\n"

		"float radiusSqr = radius * radius;\n"
		"if((topLeftDir.x <= 0) && (topLeftDir.y >= 0) && (dot(topLeftDir, topLeftDir) >= radiusSqr))\n"
		"	alpha = peekFn((dot(topLeftDir, topLeftDir) - radiusSqr) / (numGradPixels * numGradPixels), sharpness);\n"
		"if((bottomLeftDir.x <= 0) && (bottomLeftDir.y <= 0) && (dot(bottomLeftDir, bottomLeftDir) >= radiusSqr))\n"
		"	alpha = peekFn((dot(bottomLeftDir, bottomLeftDir) - radiusSqr) / (numGradPixels * numGradPixels), sharpness);\n"
		"if((bottomRightDir.x >= 0) && (bottomRightDir.y <= 0) && (dot(bottomRightDir, bottomRightDir) >= radiusSqr))\n"
		"	alpha = peekFn((dot(bottomRightDir, bottomRightDir) - radiusSqr) / (numGradPixels * numGradPixels), sharpness);\n"
		"if((topRightDir.x >= 0) && (topRightDir.y >= 0) && (dot(topRightDir, topRightDir) >= radiusSqr))\n"
		"	alpha = peekFn((dot(topRightDir, topRightDir) - radiusSqr) / (numGradPixels * numGradPixels), sharpness);\n";

		sdf.setCode(str);

		update();
	}

	void RenderRectFillRound::update() noexcept
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
}