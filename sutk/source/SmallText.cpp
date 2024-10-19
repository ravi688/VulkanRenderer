#include <sutk/SmallText.hpp>

#include <sutk/UIDriver.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer

#include <common/assert.h> // for _com_assert()

namespace SUTK
{
	SmallText::SmallText(UIDriver& driver, RenderableContainer* container, GfxDriverObjectHandleType textGroup, Color4 color) noexcept : 
															GfxDriverRenderable(driver, container), 
															m_isPosDirty(true), 
															m_isDataDirty(false), 
															m_isPointSizeDirty(false), 
															m_isColorDirty(true), 
															m_isColorRangesDirty(false), 
															m_isActiveDirty(false),
															m_color(color),
															m_horizontalAlignment(HorizontalAlignment::Invalid),
															m_verticalAlignment(VerticalAlignment::Invalid),
															m_normalizedDrawOrder(0)

	{
		_com_assert(textGroup != GFX_DRIVER_OBJECT_NULL_HANDLE);
		setGfxDriverObjectHandle(getGfxDriver().createText(textGroup));
		m_pointSize = getFontSize();
	}
	bool SmallText::isDirty()
	{
		// SmallText should only be updated when it is active (visible to the user),
		// There is no use of updating it upfront, in inactive mode, as the next update is guaranteed to occur when the it goes active.
		// So, not doing the update in inactive mode avoids redundant updates.
		return m_isActiveDirty || ((m_isPosDirty || m_isDataDirty || m_isPointSizeDirty || m_isColorRangesDirty || m_isColorDirty) && isActive());
	}
	void SmallText::update()
	{
		// NOTE: Order of function calls are important here

		// should be updated before any updates to character array
		// this is because calling setTextColor() sets internal variable to 'm_color'
		// and it uses that variable in subsequent calls to setTextData()
		if(m_isColorDirty)
		{
			getGfxDriver().setTextColor(getGfxDriverObjectHandle(), m_color);
			m_isColorDirty = false;
		}

		if(m_isPointSizeDirty)
		{
			getGfxDriver().setTextPointSize(getGfxDriverObjectHandle(), m_pointSize);
			m_isPointSizeDirty = false;
		}

		// NOTE: This must be happen before the position update, 
		// that's because 'getAlignedPosition()' only returns valid values when the text is active'
		if(m_isActiveDirty)
		{
			getGfxDriver().setTextActive(getGfxDriverObjectHandle(), isActive());
			m_isActiveDirty = false;
		}

		if(m_isDataDirty)
		{
			getGfxDriver().setTextData(getGfxDriverObjectHandle(), m_data);
			m_isDataDirty = false;
		}

		if(m_isPosDirty)
		{
			Vec2Df pos = m_pos;
			if(getContainer() != NULL)
			{
				pos = getAlignedPosition(m_pos);
				pos = getContainer()->getLocalCoordsToScreenCoords(pos);
			}
			getGfxDriver().setTextPosition(getGfxDriverObjectHandle(), { pos.x, pos.y, 0.0f });
			m_isPosDirty = false;
		}

		// should be updated after character array has been updated with setTextData().
		if(m_isColorRangesDirty)
		{
			getGfxDriver().setTextColorRanges(getGfxDriverObjectHandle(), m_colorRanges.data(), static_cast<u32>(m_colorRanges.size()));
			m_isColorRangesDirty = false;
		}
	}
	void SmallText::updateNormalizedDrawOrder(f32 normalizedDrawOrder)
	{
		// mandatory to be called in the overriding function
		Renderable::updateNormalizedDrawOrder(normalizedDrawOrder);
		getGfxDriver().setTextDepth(getGfxDriverObjectHandle(), normalizedDrawOrder);
		m_normalizedDrawOrder = normalizedDrawOrder;
	}
	void SmallText::setColor(Color4 color) noexcept
	{
		m_color = color;
		m_isColorDirty = true;
	}

	Color4 SmallText::getColor() const noexcept
	{
		return m_color;
	}

	void SmallText::setActive(com::Bool isActive) noexcept
	{
		// mandatory to be called
		Renderable::setActive(isActive);
		m_isActiveDirty = true;
	}

	void SmallText::destroy()
	{
		_com_assert(getGfxDriverObjectHandle() != GFX_DRIVER_OBJECT_NULL_HANDLE);
		getGfxDriver().destroyText(getGfxDriverObjectHandle());
		setGfxDriverObjectHandle(GFX_DRIVER_OBJECT_NULL_HANDLE);
		m_isPosDirty = false;
		m_isDataDirty = false;
		m_isPointSizeDirty = false;
		m_isColorRangesDirty = false;
		m_isColorDirty = false;
	}
	void SmallText::clearColorRanges() noexcept
	{
		m_colorRanges.clear();
		m_isColorRangesDirty = true;
	}
	void SmallText::addColorRange(std::size_t pos, std::size_t len, const Color4 color) noexcept
	{
		if(len == std::string::npos)
			len = getColumnCount();
		m_colorRanges.push_back(ColorRange { static_cast<LineCountType>(pos), static_cast<LineCountType>(pos + len), color });
		m_isColorRangesDirty = true;
	}
	LineCountType SmallText::getColPosFromCoord(f32 coord) noexcept
	{
		// SmallText::getCoordFromColPos function does all the calculations based on the most recent data in SGE
		if(m_isDataDirty)
			update();
		return getGfxDriver().getTextGlyphIndexFromCoord(getGfxDriverObjectHandle(), coord);
	}
	f32 SmallText::getCoordFromColPos(LineCountType col) noexcept
	{
		// SmallText::getCoordFromColPos function does all the calculations based on the most recent data in SGE
		if(m_isDataDirty)
			update();
		return getGfxDriver().getTextCoordFromGlyphIndex(getGfxDriverObjectHandle(), col);
	}
	void SmallText::setData(const std::string_view data) noexcept
	{
		m_data = std::string(data);
		m_isDataDirty = true;
	}
	void SmallText::append(const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data += data;
		m_isDataDirty = true;
	}
	void SmallText::removeRange(std::size_t pos, std::size_t len) noexcept
	{
		if(len == 0)
			return;
		m_data.removeRange(pos, len);
		m_isDataDirty = true;
	}
	void SmallText::insert(LineCountType col, const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data.insert(col, data);
		m_isDataDirty = true;
	}
	void SmallText::setPosition(Vec2Df pos) noexcept
	{
		m_pos = pos;
		m_isPosDirty = true;
	}
	void SmallText::addPosition(Vec2Df pos) noexcept
	{
		m_pos += pos;
		m_isPosDirty = true;
	}
	void SmallText::subPosition(Vec2Df pos) noexcept
	{
		m_pos -= pos;
		m_isPosDirty = true;
	}
	void SmallText::clear() noexcept
	{
		m_data.clear();
		m_isDataDirty = true;
	}
	void SmallText::updatePointSize() noexcept
	{
		if(m_isPointSizeDirty)
		{
			getGfxDriver().setTextPointSize(getGfxDriverObjectHandle(), m_pointSize);
			m_isPointSizeDirty = false;
		}
	}
	void SmallText::onGlobalCoordDirty() noexcept
	{
		_com_assert(getContainer() != NULL);
		m_isPosDirty = true;
	}
	void SmallText::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(isSizeChanged)
			m_isPosDirty = true;
	}
	void SmallText::setFontSize(const f32 pointSize) noexcept
	{
		m_isPointSizeDirty = true;
		m_pointSize = pointSize;
	}
	f32 SmallText::getFontSize() noexcept
	{
		updatePointSize();
		return getGfxDriver().getTextPointSize(getGfxDriverObjectHandle());
	}
	void SmallText::setFont(UIDriver::FontReference font) noexcept
	{
		getGfxDriver().setTextFont(getGfxDriverObjectHandle(), font);
	}
	f32 SmallText::getBaselineHeight() noexcept
	{
		return getGfxDriver().getTextBaselineHeightInCentimeters(getGfxDriverObjectHandle(), m_pointSize);
	}

	Vec2Df SmallText::getBoundingRectSize() noexcept
	{
		f32 width = getCoordFromColPos(getColumnCount());
		f32 height = getBaselineHeight();
		return { width, height };
	}

	Vec2Df SmallText::getAlignedPosition(Vec2Df pos) noexcept
	{
		if(getContainer() == NULL)
			return pos;

		// This is to avoid calling getBoundingRect() as much as possible,
		// as that internally calls getCoordFromColPos() which in turn calls ensureUpdated().
		if((m_horizontalAlignment == HorizontalAlignment::Invalid) && (m_verticalAlignment == VerticalAlignment::Invalid))
			return pos;

		Vec2Df size = getBoundingRectSize();
		f32 hPos = pos.x;
		switch(m_horizontalAlignment)
		{
			case HorizontalAlignment::Left:
			{
				hPos = 0.0f;
				break;
			}
			case HorizontalAlignment::Middle:
			{
				hPos = getContainer()->getRect().width * 0.5f - size.width * 0.5f;
				break;
			}
			case HorizontalAlignment::Right:
			{
				hPos = getContainer()->getRect().width - size.width;
				break;
			}
			default: { }
		}
		f32 vPos = pos.y;
		switch(m_verticalAlignment)
		{
			case VerticalAlignment::Top:
			{
				vPos = 0.0f;
				break;
			}
			case VerticalAlignment::Middle:
			{
				vPos = getContainer()->getRect().height * 0.5f - size.height * 0.5f;
				break;
			}
			case VerticalAlignment::Bottom:
			{
				vPos = getContainer()->getRect().height - size.height;
				break;
			}
			default: { }
		}
		return { hPos, vPos };
	}

	void SmallText::setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept
	{
		m_horizontalAlignment = hAlign;
		m_verticalAlignment = vAlign;
		m_isPosDirty = true;
	}
}