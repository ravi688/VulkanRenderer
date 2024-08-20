#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>

#include <sutk/assert.h>

namespace SUTK
{

	void RenderRect::setActive(bool isActive) noexcept
	{
		// mandatory to call
		GeometryRenderable::setActive(isActive);
		m_isActiveDirty = true;
	}

	bool RenderRect::isDirty()
	{
		return m_isActiveDirty;
	}

	void RenderRect::update()
	{
		if(m_isActiveDirty)
		{
			auto handle = getGfxDriverObjectHandle();
			_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
			auto obj = getGfxDriver().getGeometryObject(handle);
			getGfxDriver().setObjectActive(obj, isActive());
			m_isActiveDirty = false;
		}
	}

	RenderRectOutline::RenderRectOutline(UIDriver& driver, RenderableContainer* container) noexcept : RenderRect(driver, container),
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_isColorDirty(true),
																						m_thickness(0.2f)
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(8)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3) // top line
			.quad(3, 2, 4, 5) // right line
			.quad(5, 4, 6, 7) // bottom line
			.quad(7, 6, 1, 0)// left line
			.fillColor(getColor());

		// call update for the first time as we have geometry description already and
		// this geometry is supposed to be displayed in the very first frame.
		update();
	}

	bool RenderRectOutline::isDirty()
	{
		return m_isPosDirty || m_isSizeDirty || m_isColorDirty || RenderRect::isDirty();
	}

	void RenderRectOutline::update()
	{
		RenderRect::update();
		if(m_isSizeDirty || m_isPosDirty)
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_assert(array.size() == 8);
			Rect2Df rect = getRect();
			auto topLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getTopLeft());
			auto bottomLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomLeft());
			auto bottomRight = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomRight());
			auto topRight = getContainer()->getLocalCoordsToScreenCoords(rect.getTopRight());

			/*   0   		  3
				 _____________
				|\___________/|
				| |1	   2| |
				| |			| |
				| |6	   4| |
				| |_________| |
				|/___________\|
				7  			  5
			*/

			auto downRight = (Vec2Df::down() + Vec2Df::right()) * m_thickness * 0.5f;
			auto downLeft = (Vec2Df::down() + Vec2Df::left()) * m_thickness * 0.5f;
			array[0] = static_cast<Vec2Df>(topLeft) - downRight;
			array[1] = static_cast<Vec2Df>(topLeft) + downRight;
			array[2] = static_cast<Vec2Df>(topRight) + downLeft;
			array[3] = static_cast<Vec2Df>(topRight) - downLeft;

			auto upLeft = (Vec2Df::up() + Vec2Df::left()) * m_thickness * 0.5f;
			auto upRight = (Vec2Df::up() + Vec2Df::right()) * m_thickness * 0.5f;
			array[4] = static_cast<Vec2Df>(bottomRight) + upLeft;
			array[5] = static_cast<Vec2Df>(bottomRight) - upLeft;
			array[6] = static_cast<Vec2Df>(bottomLeft) + upRight;
			array[7] = static_cast<Vec2Df>(bottomLeft) - upRight;
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isColorDirty)
			getGeometry().fillColor(getColor());

		if(m_isPosDirty || m_isSizeDirty || m_isColorDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
			m_isColorDirty = false;
		}
	}

	void RenderRectOutline::setColor(Color4 color) noexcept
	{
		RenderRect::setColor(color);
		m_isColorDirty = true;
	}

	void RenderRectOutline::setThickness(f32 thickness) noexcept
	{
		 m_thickness = thickness;
		 m_isSizeDirty = true;
	}

	void RenderRectOutline::onGlobalCoordDirty() noexcept
	{
		m_isPosDirty = true;
	}

	void RenderRectOutline::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(m_isSizeDirty)
		{
			// The rect for this RenderRect is in local coordinates of the RenderableContainer for this RenderRect.
			// Therefore, the position would always be { 0, 0 } for this rect.
			rect.setPosition({ 0, 0 });
			setRect(rect);
		}
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}

	RenderRectFill::RenderRectFill(UIDriver& driver, RenderableContainer* container, bool isUpdate) noexcept : RenderRect(driver, container),
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_isColorDirty(true)
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(4)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3)
			.fillColor(getColor());

		if(isUpdate)
		{
			// call update for the first time as we have geometry description already and
			// this geometry is supposed to be displayed in the very first frame.
			update();
		}
	}

	bool RenderRectFill::isDirty()
	{
		return m_isPosDirty || m_isSizeDirty || m_isColorDirty || RenderRect::isDirty();
	}

	void RenderRectFill::update()
	{
		RenderRect::update();
		if(m_isSizeDirty || m_isPosDirty)
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_assert(array.size() == 4);
			Rect2Df rect = getRect();
			auto topLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getTopLeft());
			auto bottomLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomLeft());
			auto bottomRight = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomRight());
			auto topRight = getContainer()->getLocalCoordsToScreenCoords(rect.getTopRight());

			array[0] = topLeft;
			array[1] = bottomLeft;
			array[2] = bottomRight;
			array[3] = topRight;
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isColorDirty)
		{
			getGeometry().fillColor(getColor());
		}

		if(m_isPosDirty || m_isSizeDirty || m_isColorDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
			m_isColorDirty = false;
		}
	}

	void RenderRectFill::setColor(Color4 color) noexcept
	{
		 RenderRect::setColor(color);
		 m_isColorDirty = true;
	}

	void RenderRectFill::onGlobalCoordDirty() noexcept
	{
		m_isPosDirty = true;
	}

	void RenderRectFill::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(isSizeChanged)
		{
			// The rect for this RenderRect is in local coordinates of the RenderableContainer for this RenderRect.
			// Therefore, the position would always be { 0, 0 } for this rect.
			rect.setPosition({ 0, 0 });
			setRect(rect);
		}
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}

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

	void RenderRectFillRound::update()
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