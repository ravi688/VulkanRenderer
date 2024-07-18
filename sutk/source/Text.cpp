#include <sutk/Text.hpp>

#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IGfxDriver.hpp> /* for SUTK::IGfxDriver::createText() */
#include <sutk/TextContainer.hpp> /* for SUTK::TextContainer::getLocalCoordsToScreenCoords() */

#include <common/assert.h> /* for _assert() */

namespace SUTK
{
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfText() { return { END_OF_TEXT, END_OF_LINE }; }
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfLine(LineCountType line) { return { line, END_OF_LINE }; }

	LineText::LineText(UIDriver& driver) noexcept : UIDriverObject(driver), m_isPosDirty(true), m_isDataDirty(false)
	{
		m_handle = getGfxDriver().createText();
	}
	bool LineText::isDirty()
	{
		return m_isPosDirty || m_isDataDirty;
	}
	void LineText::update()
	{
		if(m_isPosDirty)
		{
			getGfxDriver().setTextPosition(m_handle, m_pos);
			m_isPosDirty = false;
		}
		if(m_isDataDirty)
		{
			getGfxDriver().setTextData(m_handle, m_data);
			m_isDataDirty = false;
		}
	}
	void LineText::setData(const std::string& data) noexcept
	{
		if(m_data == data)
			return;
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
	void LineText::setPosition(Vec2D<DisplaySizeType> pos) noexcept
	{
		if(m_pos == pos)
			return;
		m_pos = pos;
		m_isPosDirty = true;
	}
	void LineText::addPosition(Vec2D<DisplaySizeType> pos) noexcept
	{
		if(pos == Vec2D<DisplaySizeType>::zero())
			return;
		m_pos += pos;
		m_isPosDirty = true;
	}
	void LineText::subPosition(Vec2D<DisplaySizeType> pos) noexcept
	{
		if(pos == Vec2D<DisplaySizeType>::zero())
			return;
		m_pos -= pos;
		m_isPosDirty = true;
	}
	void LineText::clear() noexcept
	{
		m_data.clear();
	}

	Text::Text(UIDriver& driver, TextContainer* container) noexcept : UIDriverObject(driver), m_container(container), m_isDirty(false)
	{
		m_baselineHeight = getGfxDriver().getBaselineHeightInPixels();
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
	}

	Vec2D<DisplaySizeType> Text::getLocalPositionFromCursorPosition(const CursorPosition<LineCountType>& cursor) noexcept
	{
		return { 0, static_cast<DisplaySizeType>(m_baselineHeight * cursor.getLine()) };
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
			m_lines[i]->addPosition({ 0, static_cast<DisplaySizeType>(m_baselineHeight) });

		// insert the line at which the cursor points to
		LineText* lineText = new LineText(getUIDriver());
		m_lines.insert(com::GetIteratorFromIndex<std::vector, LineText*>(m_lines, cursorPosition.getLine()), lineText);

		Vec2D<DisplaySizeType> localCoords = getLocalPositionFromCursorPosition(cursorPosition);
		Vec2D<DisplaySizeType> screenCoords = getContainer()->getLocalCoordsToScreenCoords(localCoords);
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

	void Text::onContainerResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		CursorPosition<LineCountType> cursorPosition { 0u, 0u };
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			Vec2D<DisplaySizeType> localCoords = getLocalPositionFromCursorPosition(cursorPosition);
			Vec2D<DisplaySizeType> screenCoords = getContainer()->getLocalCoordsToScreenCoords(localCoords);
			LineText* lineText = m_lines[i];
			lineText->setPosition(screenCoords);
			cursorPosition.moveToNextLine();
		}
	}
}