#include <sutk/Text.hpp>

#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IGfxDriver.hpp> /* for SUTK::IGfxDriver::createText() */
#include <sutk/RenderableContainer.hpp> /* for SUTK::RenderableContainer::getLocalCoordsToScreenCoords() */

#include <common/assert.h> /* for _assert() */

#include <algorithm> // for std::min and std::clamp

namespace SUTK
{
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfText() { return { END_OF_TEXT, END_OF_LINE }; }
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfLine(LineCountType line) { return { line, END_OF_LINE }; }

	LineText::LineText(UIDriver& driver, GfxDriverObjectHandleType textGroup) noexcept : GfxDriverRenderable(driver, NULL), m_isPosDirty(true), m_isDataDirty(false), m_isPointSizeDirty(false)
	{
		setGfxDriverObjectHandle(getGfxDriver().createText(textGroup));
		m_pointSize = getFontSize();
	}
	bool LineText::isDirty()
	{
		return m_isPosDirty || m_isDataDirty || m_isPointSizeDirty;
	}
	void LineText::update()
	{
		if(m_isPointSizeDirty)
		{
			getGfxDriver().setTextPointSize(getGfxDriverObjectHandle(), m_pointSize);
			m_isPointSizeDirty = false;
		}
		if(m_isPosDirty)
		{
			getGfxDriver().setTextPosition(getGfxDriverObjectHandle(), m_pos);
			m_isPosDirty = false;
		}
		if(m_isDataDirty)
		{
			getGfxDriver().setTextData(getGfxDriverObjectHandle(), m_data);
			m_isDataDirty = false;
		}
	}
	void LineText::destroy()
	{
		_com_assert(getGfxDriverObjectHandle() != GFX_DRIVER_OBJECT_NULL_HANDLE);
		getGfxDriver().destroyText(getGfxDriverObjectHandle());
		setGfxDriverObjectHandle(GFX_DRIVER_OBJECT_NULL_HANDLE);
		m_isPosDirty = false;
		m_isDataDirty = false;
	}
	LineCountType LineText::getColPosFromCoord(f32 coord) noexcept
	{
		// LineText::getCoordFromColPos function does all the calculations based on the most recent data in SGE
		ensureUpdated();
		return getGfxDriver().getTextGlyphIndexFromCoord(getGfxDriverObjectHandle(), coord);
	}
	f32 LineText::getCoordFromColPos(LineCountType col) noexcept
	{
		// LineText::getCoordFromColPos function does all the calculations based on the most recent data in SGE
		ensureUpdated();
		return getGfxDriver().getTextCoordFromGlyphIndex(getGfxDriverObjectHandle(), col);
	}
	void LineText::setData(const std::string& data) noexcept
	{
		m_data = std::string(data);
		m_isDataDirty = true;
	}
	void LineText::append(const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data += data;
		m_isDataDirty = true;
	}
	void LineText::removeRange(std::size_t pos, std::size_t len) noexcept
	{
		if(len == 0)
			return;
		m_data.removeRange(pos, len);
		m_isDataDirty = true;
	}
	void LineText::insert(LineCountType col, const std::string& data) noexcept
	{
		if(data.empty())
			return;
		m_data.insert(col, data);
		m_isDataDirty = true;
	}
	void LineText::setPosition(Vec2Df pos) noexcept
	{
		m_pos = pos;
		m_isPosDirty = true;
	}
	void LineText::addPosition(Vec2Df pos) noexcept
	{
		m_pos += pos;
		m_isPosDirty = true;
	}
	void LineText::subPosition(Vec2Df pos) noexcept
	{
		m_pos -= pos;
		m_isPosDirty = true;
	}
	void LineText::clear() noexcept
	{
		m_data.clear();
		m_isDataDirty = true;
	}
	void LineText::updatePointSize() noexcept
	{
		if(m_isPointSizeDirty)
		{
			getGfxDriver().setTextPointSize(getGfxDriverObjectHandle(), m_pointSize);
			m_isPointSizeDirty = false;
		}
	}
	void LineText::setFontSize(const f32 pointSize) noexcept
	{
		m_isPointSizeDirty = true;
		m_pointSize = pointSize;
	}
	f32 LineText::getFontSize() noexcept
	{
		updatePointSize();
		return getGfxDriver().getTextPointSize(getGfxDriverObjectHandle());
	}
	f32 LineText::getBaselineHeight() noexcept
	{
		updatePointSize();
		return getGfxDriver().getTextBaselineHeightInCentimeters(getGfxDriverObjectHandle());
	}

	Text::Text(UIDriver& driver, RenderableContainer* container) noexcept : Renderable(driver, container), m_textGroup(GFX_DRIVER_OBJECT_NULL_HANDLE), m_baselineHeight(0), m_isDirty(false), m_isClippingEnabled(false), m_pointSize(12)
	{
		m_textGroup = getGfxDriver().createTextGroup();
	}

	bool Text::isDirty()
	{
		if(m_isDirty)
			return true;

		for(std::size_t i = 0; i < m_lines.size(); i++)
			if(m_lines[i]->isDirty())
				return true;

		return false;
	}

	void Text::update()
	{
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			LineText* line = m_lines[i];
			if(line->isDirty())
				line->update();
		}
	}

	void Text::clear() noexcept
	{
		// clear existing data
		// NOTE: here clearing means, the CPU side data clear.
		// GPU side data will be cleared upon call to IRenderable::update as IRenderable::isDirty() will return true.
		// This is to ensure that we do not write to GPU side memory multiple times in a single frame.
		// In this case, we are (want to) first clearing the data and then write again, so there could have been double writes to GPU side storage!
		for(std::size_t i = 0; i < m_lines.size(); i++)
			m_lines[i]->clear();
		m_lines.clear();
	}

	Vec2Df Text::getLocalPositionFromCursorPosition(const CursorPosition<LineCountType>& cursor) noexcept
	{
		LineCountType line = cursor.getLine();
		_assert((line >= 0) && (line < m_lines.size()));
		return { m_lines[line]->getCoordFromColPos(cursor.getColumn()) + m_scrollDelta.x, getBaselineHeight() * cursor.getLine() + m_scrollDelta.y };
	}

	std::pair<s32, s32> Text::getUnclippedLineRange() noexcept
	{
		auto rect = getContainer()->getRect();
		auto scrollDelta = getScrollDelta();
		s32 lowerBound = static_cast<s32>(-scrollDelta.y / getBaselineHeight());
		s32 upperBound = static_cast<s32>((-scrollDelta.y + rect.getHeight()) / getBaselineHeight());
		return { 
					std::clamp(lowerBound, 0, static_cast<s32>(m_lines.size()) - 1),  
				 	std::clamp(upperBound, 0, static_cast<s32>(m_lines.size()) - 1)
			   };
	}

	CursorPosition<LineCountType> Text::getCursorPosFromCoords(Vec2Df coords) noexcept
	{
		if(m_lines.size() == 0)
			return { 0, 0 };
		std::pair<s32, s32> range = getUnclippedLineRange();
		auto lineNo = static_cast<s32>((coords - m_scrollDelta).y / getBaselineHeight());
		lineNo = std::min(lineNo, static_cast<s32>(m_lines.size()) - 1);
		lineNo = std::max(lineNo, 0);
		_assert(lineNo >= 0);
		LineCountType colNo = m_lines[lineNo]->getColPosFromCoord(coords.x - m_scrollDelta.x);
		return { lineNo, colNo };
	}

	// this creates a new line before line pointed by current cursor
	LineText* Text::createNewLine(Flags flags, LineCountType line) noexcept
	{
		CursorPosition<LineCountType> cursorPosition;
		
		if(line == END_OF_TEXT)
		{
			line = m_lines.size();
			if(line >= 1u)
				line -= 1u;
		};
		cursorPosition.moveToLine(static_cast<DisplaySizeType>(line));

		if(flags == Flags::After)
			cursorPosition.moveToNextLine(m_lines.size());

		// shift the line at which the cursor points to and the lines succeeding it
		for(std::size_t i = cursorPosition.getLine(); i < m_lines.size(); i++)
			m_lines[i]->addPosition({ 0, getBaselineHeight() });

		// insert the line at which the cursor points to
		LineText* lineText = new LineText(getUIDriver(), m_textGroup);
		m_lines.insert(com::GetIteratorFromIndex<std::vector, LineText*>(m_lines, cursorPosition.getLine()), lineText);

		lineText->setFontSize(m_pointSize);
		Vec2Df localCoords = getLocalPositionFromCursorPosition(cursorPosition);
		Vec2Df screenCoords = getContainer()->getLocalCoordsToScreenCoords(localCoords);
		lineText->setPosition(screenCoords);

		return lineText;
	}

	void Text::removeLine(LineCountType line) noexcept
	{
		// destroy the Gfx driver side objects
		// m_lines[line]->destroy();
		// delete m_lines[line];
		m_lines[line]->setData("");
		// remove the line at index 'line'
		m_lines.erase(com::GetIteratorFromIndex<std::vector, LineText*>(m_lines, line));

		// shift the line at which the cursor points to and the lines succeeding it
		for(std::size_t i = line; i < m_lines.size(); i++)
			m_lines[i]->addPosition({ 0, -getBaselineHeight() });
	}

	LineText* Text::getOrCreateLastLine() noexcept
	{
		// if there are no lines
		if(m_lines.size() == 0)
			// create a new line (the very first one; it would also be the last line as it is the only line currently)
			return createNewLine(Flags::After, END_OF_TEXT);
		// otherwise, the last 
		return m_lines.back();
	}

	LineText* Text::getLine(LineCountType line) noexcept
	{
		if(line == END_OF_TEXT)
			return getOrCreateLastLine();
		if(line >= m_lines.size())
			return NULL;
		return m_lines[line];
	}

	void Text::insert(const CursorPosition<LineCountType>& position, const std::string& str) noexcept
	{
		// if there is nothing to write then return
		if(str.empty())
			return;
		LineText* lineText = getLine(position.getLine());

		std::string::size_type index = str.find_first_of('\n');
		if(index == std::string::npos)
			lineText->insert(position.getColumn(), str);
		else
			lineText->insert(position.getColumn(), str.substr(0, index));

		LineText* newLineText = NULL;
		auto line = position.getLine();
		auto saveIndex = index;
		while(index != std::string::npos)
		{
			// create a new line
			newLineText = createNewLine(Flags::After, line++);
			
			// if the new line character is at the very end, then no extra characters to add,
			// i.e. it is an empty new line
			if(index == (str.size() - 1))
				break;

			// character just after '\n'
			++index;

			// determine the substring to be added into the just created new line
			// i.e. the substring is sandwitched with either '\n' characters on both the sides (left and right),
			// or '\n' character on left and end of the string on the right.
			auto end = str.find_first_of('\n', index);

			// if no new line character on the right side, then append rest of the string
			if(end == std::string::npos)
				newLineText->append(str.substr(index, std::string::npos));
			// otherwise, append a substring surrounded by '\n' charactesr from both the sides (left and right)
			else
				newLineText->append(str.substr(index, end - index));
			index = end;
		}

		if((newLineText != NULL) && (position.getColumn() != END_OF_LINE))
		{
			newLineText->append(lineText->substr(position.getColumn() + saveIndex, std::string::npos));
			lineText->removeRange(position.getColumn() + saveIndex);
		}
	}

	void Text::removeRange(const CursorPosition<LineCountType>& start, const CursorPosition<LineCountType>& end) noexcept
	{
		_com_assert(start.getLine() < m_lines.size());
		_com_assert(end.getLine() < m_lines.size());
		//  start line ----- 	 	lower line number
		//  ---------------- <-- intermediate line
		//  ---------------- <-- intermediate line
		/// ---------- end line  	higher line number
		_com_assert(start.getLine() <= end.getLine());

		// remove the intermediate lines if any
		LineCountType lineDiff = end.getLine() - start.getLine();
		for(LineCountType i = 1; i < lineDiff; i++)
			removeLine(start.getLine() + i);

		// if start and end are straddled across different lines
		if(start.getLine() != end.getLine())
		{
			// remove characters from start.getColumn() to m_lines[start.getLine()].getColumnCount() exclusive
			LineText* startLineText = m_lines[start.getLine()];
			startLineText->removeRange(start.getColumn(), std::string::npos);

			// remove characters from 0 to end.getColumn() 
			LineText* endLineText = m_lines[end.getLine()];
			endLineText->removeRange(0, std::min(static_cast<std::string::size_type>(end.getColumn()), endLineText->getColumnCount()));

			// merge
			startLineText->append(endLineText->getData());

			// remove the last line (end line)
			removeLine(end.getLine());
		}
		// if start and end are in the same line
		else
		{
			LineText* lineText = m_lines[end.getLine()];
			std::size_t colDiff = end.getColumn() - start.getColumn();
			lineText->removeRange(start.getColumn(), colDiff);
		}
	}

	void Text::set(const std::string& str) noexcept
	{
		// clear the exisiting data
		clear();
		// wring new data
		append(str);
	}

	void Text::recalculateClipRect() noexcept
	{
		RenderableContainer* container = getContainer();
		// convert the top-left corner of the container, in which this text is, to screen coordinates
		auto position = container->getLocalCoordsToScreenCoords({ 0u, 0u });
		
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			// set the clip rect, note that width and height should remain as that of its text container
			m_lines[i]->setClipRectGlobalCoords({ position.x, position.y, container->getRect().width, container->getRect().height });
		}
	}

	void Text::enableClipping(bool isEnable) noexcept
	{
		m_isClippingEnabled = isEnable;
		recalculateClipRect();
	}

	void Text::setScrollDelta(Vec2Df delta) noexcept
	{
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			LineText* lineText = m_lines[i];
			auto pos = lineText->getPosition();
			pos -= m_scrollDelta;
			pos += delta;
			lineText->setPosition(pos);
		}
		m_scrollDelta = delta;
	}

	void Text::addScrollDelta(Vec2Df delta) noexcept
	{
		setScrollDelta(m_scrollDelta + delta);
	}

	void Text::onGlobalCoordDirty() noexcept
	{
		onContainerResize(getContainer()->getRect(), true, true);
	}

	void Text::updateLinePositions() noexcept
	{
		CursorPosition<LineCountType> cursorPosition { 0u, 0u };
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			Vec2Df localCoords = getLocalPositionFromCursorPosition(cursorPosition);
			Vec2Df screenCoords = getContainer()->getLocalCoordsToScreenCoords(localCoords);
			LineText* lineText = m_lines[i];
			lineText->setPosition(screenCoords);
			cursorPosition.moveToNextLine();
		}
	}

	void Text::onContainerResize(Rect2Df newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		updateLinePositions();
		if(m_isClippingEnabled)
			recalculateClipRect();
	}

	f32 Text::getBaselineHeight() noexcept
	{
		if((m_baselineHeight == 0.0f) && (m_lines.size() > 0))
			m_baselineHeight = m_lines[0]->getBaselineHeight();
		return m_baselineHeight;
	}

	void Text::setFontSize(const f32 pointSize) noexcept
	{
		// update the point sizes for each line
		for(LineText*& lineText : m_lines)
			lineText->setFontSize(pointSize);
		m_pointSize = pointSize;

		// update the baseline height
		if(m_lines.size() > 0)
			m_baselineHeight = m_lines[0]->getBaselineHeight();

		updateLinePositions();
	}

	f32 Text::getFontSize() const noexcept
	{
		return m_pointSize;
	}
}