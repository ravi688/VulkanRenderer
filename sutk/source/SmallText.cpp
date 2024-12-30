#include <sutk/SmallText.hpp>

#include <sutk/UIDriver.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer

#include <common/assert.h> // for _com_assert()

namespace SUTK
{
	SmallText::SmallText(UIDriver& driver, RenderableContainer* container, GfxDriverObjectHandleType textGroup, Color4 color) noexcept : 
															GfxDriverRenderable(driver, container),  
															m_color(color),
															m_horizontalAlignment(HorizontalAlignment::Invalid),
															m_verticalAlignment(VerticalAlignment::Invalid),
															m_normalizedDrawOrder(0)

	{
		m_dirtyTable.isColorDirty = true;
		m_dirtyTable.isPosDirty = true;
		_com_assert(textGroup != GFX_DRIVER_OBJECT_NULL_HANDLE);
		setGfxDriverObjectHandle(getGfxDriver().createText(textGroup));
		getGfxDriver().setTextFont(getGfxDriverObjectHandle(), m_font.getHandle());
		m_pointSize = getFontSize();
	}

	SmallText::~SmallText() noexcept
	{
		getGfxDriver().destroyText(getGfxDriverObjectHandle());
		setGfxDriverObjectHandle(GFX_DRIVER_OBJECT_NULL_HANDLE);
	}

	bool SmallText::isDirty()
	{
		return GfxDriverRenderable::isDirty() || m_dirtyTable.isAny();
	}
	void SmallText::update()
	{
		// Mandatory to be called in the overriding method
		GfxDriverRenderable::update();
		auto dirtyTable = m_dirtyTable;
		m_dirtyTable.clear();
		// NOTE: Order of function calls are important here

		// should be updated before any updates to character array
		// this is because calling setTextColor() sets internal variable to 'm_color'
		// and it uses that variable in subsequent calls to setTextData()
		if(dirtyTable.isColorDirty)
			getGfxDriver().setTextColor(getGfxDriverObjectHandle(), m_color);

		if(dirtyTable.isPointSizeDirty)
			getGfxDriver().setTextPointSize(getGfxDriverObjectHandle(), m_pointSize);

		if(dirtyTable.isDataDirty)
			getGfxDriver().setTextData(getGfxDriverObjectHandle(), m_data);

		if(dirtyTable.isPosDirty)
		{
			Vec2Df pos = m_pos;
			if(getContainer() != NULL)
			{
				pos = getAlignedPosition(m_pos);
				pos = getContainer()->getLocalCoordsToScreenCoords(pos);
			}
			getGfxDriver().setTextPosition(getGfxDriverObjectHandle(), { pos.x, pos.y, 0.0f });
		}

		// should be updated after character array has been updated with setTextData().
		if(dirtyTable.isColorRangesDirty)
			getGfxDriver().setTextColorRanges(getGfxDriverObjectHandle(), m_colorRanges.data(), static_cast<u32>(m_colorRanges.size()));
	}
	void SmallText::updateNormalizedDrawOrder(f32 normalizedDrawOrder)
	{
		// mandatory to be called in the overriding function
		GfxDriverRenderable::updateNormalizedDrawOrder(normalizedDrawOrder);
		getGfxDriver().setTextDepth(getGfxDriverObjectHandle(), normalizedDrawOrder);
		m_normalizedDrawOrder = normalizedDrawOrder;
	}
	void SmallText::onActiveUpdate(com::Bool isActive) noexcept
	{
		// Activatation or Deactivation updates must happen immediately
		getGfxDriver().setTextActive(getGfxDriverObjectHandle(), isActive);
	}
	void SmallText::setColor(Color4 color) noexcept
	{
		m_color = color;
		m_dirtyTable.isColorDirty = true;
	}

	Color4 SmallText::getColor() const noexcept
	{
		return m_color;
	}

	void SmallText::clearColorRanges() noexcept
	{
		m_colorRanges.clear();
		m_dirtyTable.isColorRangesDirty = true;
	}
	void SmallText::addColorRange(std::size_t pos, std::size_t len, const Color4 color) noexcept
	{
		if(len == std::string::npos)
			len = getColumnCount();
		m_colorRanges.push_back(ColorRange { static_cast<LineCountType>(pos), static_cast<LineCountType>(pos + len), color });
		m_dirtyTable.isColorRangesDirty = true;
	}
	LineCountType SmallText::getColPosFromCoord(f32 coord) noexcept
	{
		// Ensure GPU side data is updated so that we get correct output from getTextGlyphIndexFromCoord()
		// NOTE: Data (chars) and PointSize both affect the value returned by getTextGlyphIndexFromCoord()
		if(isDirty())
			update();
		return getGfxDriver().getTextGlyphIndexFromCoord(getGfxDriverObjectHandle(), coord);
	}
	f32 SmallText::getCoordFromColPos(LineCountType col) noexcept
	{
		// Ensure GPU side data is updated so that we get correct output from getTextGlyphIndexFromCoord()
		// NOTE: Data (chars) and PointSize both affect the value returned by getTextGlyphIndexFromCoord()
		if(isDirty())
			update();
		return getGfxDriver().getTextCoordFromGlyphIndex(getGfxDriverObjectHandle(), col);
	}
	void SmallText::setData(const std::string_view data) noexcept
	{
		m_data = std::string(data);
		m_dirtyTable.isDataDirty = true;
	}
	void SmallText::append(const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data += data;
		m_dirtyTable.isDataDirty = true;
	}
	void SmallText::removeRange(std::size_t pos, std::size_t len) noexcept
	{
		if(len == 0)
			return;
		m_data.removeRange(pos, len);
		m_dirtyTable.isDataDirty = true;
	}
	void SmallText::insert(LineCountType col, const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data.insert(col, data);
		m_dirtyTable.isDataDirty = true;
	}
	void SmallText::setPosition(Vec2Df pos) noexcept
	{
		m_pos = pos;
		m_dirtyTable.isPosDirty = true;
	}
	void SmallText::addPosition(Vec2Df pos) noexcept
	{
		m_pos += pos;
		m_dirtyTable.isPosDirty = true;
	}
	void SmallText::subPosition(Vec2Df pos) noexcept
	{
		m_pos -= pos;
		m_dirtyTable.isPosDirty = true;
	}
	void SmallText::clear() noexcept
	{
		m_data.clear();
		m_dirtyTable.isDataDirty = true;
	}
	void SmallText::onGlobalCoordDirty() noexcept
	{
		_com_assert(getContainer() != NULL);
		m_dirtyTable.isPosDirty = true;
	}
	void SmallText::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(isSizeChanged)
			m_dirtyTable.isPosDirty = true;
	}
	void SmallText::setFontSize(const f32 pointSize) noexcept
	{
		m_dirtyTable.isPointSizeDirty = true;
		m_pointSize = pointSize;
	}
	f32 SmallText::getFontSize() noexcept
	{
		if(isDirty()) update();
		return getGfxDriver().getTextPointSize(getGfxDriverObjectHandle());
	}
	void SmallText::setFont(UIDriver::FontReference font) noexcept
	{
		getGfxDriver().setTextFont(getGfxDriverObjectHandle(), font.getHandle());
		m_font = font;
		// Setting m_isDataDirty to true would cause isDirty() to return true in UIDriver's render method which will the entire frame again
		// It needs to be noted that the data is not dirty, we just want the frame to be redrawn.
		m_dirtyTable.isDataDirty = true;
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
		m_dirtyTable.isPosDirty = true;
	}
}