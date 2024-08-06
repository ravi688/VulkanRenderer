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

	LineText::LineText(UIDriver& driver) noexcept : GfxDriverRenderable(driver, NULL), m_isPosDirty(true), m_isDataDirty(false)
	{
		setGfxDriverObjectHandle(getGfxDriver().createText());
	}
	bool LineText::isDirty()
	{
		return m_isPosDirty || m_isDataDirty;
	}
	void LineText::update()
	{
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
	LineCountType LineText::getColPosFromCoord(f32 coord) noexcept
	{
		return getGfxDriver().getTextGlyphIndexFromCoord(getGfxDriverObjectHandle(), coord);
	}
	f32 LineText::getCoordFromColPos(LineCountType col) noexcept
	{
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

	Text::Text(UIDriver& driver, RenderableContainer* container) noexcept : Renderable(driver, container), m_isDirty(false), m_isClippingEnabled(false)
	{
		m_baselineHeight = getGfxDriver().getBaselineHeightInCentimeters();
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
		return { m_lines[line]->getCoordFromColPos(cursor.getColumn()), m_baselineHeight * cursor.getLine() + m_scrollDelta.y };
	}

	std::pair<s32, s32> Text::getUnclippedLineRange() noexcept
	{
		auto rect = getContainer()->getRect();
		auto scrollDelta = getScrollDelta();
		s32 lowerBound = static_cast<s32>(-scrollDelta.y / m_baselineHeight);
		s32 upperBound = static_cast<s32>((-scrollDelta.y + rect.getHeight()) / m_baselineHeight);
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
		auto lineNo = static_cast<s32>((coords - m_scrollDelta).y / m_baselineHeight);
		lineNo = std::min(lineNo, static_cast<s32>(m_lines.size()) - 1);
		lineNo = std::max(lineNo, 0);
		_assert(lineNo >= 0);
		LineCountType colNo = m_lines[lineNo]->getColPosFromCoord(coords.x);
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
			m_lines[i]->addPosition({ 0, m_baselineHeight });

		// insert the line at which the cursor points to
		LineText* lineText = new LineText(getUIDriver());
		m_lines.insert(com::GetIteratorFromIndex<std::vector, LineText*>(m_lines, cursorPosition.getLine()), lineText);

		Vec2Df localCoords = getLocalPositionFromCursorPosition(cursorPosition);
		Vec2Df screenCoords = getContainer()->getLocalCoordsToScreenCoords(localCoords);
		lineText->setPosition(screenCoords);

		return lineText;
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

		auto line = position.getLine();
		while(index != std::string::npos)
		{
			// create a new line
			auto lineText = createNewLine(Flags::After, line++);
			
			// if the new line character is at the very end, then no extra characters to add,
			// i.e. it is an empty new line
			if(index == (str.size() - 1))
				return;

			// character just after '\n'
			++index;

			// determine the substring to be added into the just created new line
			// i.e. the substring is sandwitched with either '\n' characters on both the sides (left and right),
			// or '\n' character on left and end of the string on the right.
			auto end = str.find_first_of('\n', index);

			// if no new line character on the right side, then append rest of the string
			if(end == std::string::npos)
				lineText->append(str.substr(index, std::string::npos));
			// otherwise, append a substring surrounded by '\n' charactesr from both the sides (left and right)
			else
				lineText->append(str.substr(index, end - index));
			index = end;
		}
	}

	void Text::remove(const CursorPosition<LineCountType>& position, LineCountType numChars) noexcept
	{

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

	void Text::onContainerResize(Rect2Df newRect, bool isPositionChanged, bool isSizeChanged) noexcept
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
		if(m_isClippingEnabled)
			recalculateClipRect();
	}
}