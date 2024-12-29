#include <sutk/Text.hpp>

#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IGfxDriver.hpp> /* for SUTK::IGfxDriver::createText() */
#include <sutk/RenderableContainer.hpp> /* for SUTK::RenderableContainer::getLocalCoordsToScreenCoords() */

#include <common/assert.h> /* for _assert() */
#include <common/debug.h> // for debug_log_error()
#include <disk_manager/file_reader.h> // for load_text_from_file()

#include <algorithm> // for std::min and std::clamp
#include <fstream> // for std::ofstream

namespace SUTK
{
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::StartOfText() { return { 0, 0 }; }
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::StartOfLine(LineCountType line) { return { line, 0 }; }
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfText() { return { END_OF_TEXT, END_OF_LINE }; }
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfLine(LineCountType line) { return { line, END_OF_LINE }; }

	Text::Text(UIDriver& driver, RenderableContainer* container) noexcept : Renderable(driver, container), m_textGroup(GFX_DRIVER_OBJECT_NULL_HANDLE), m_baselineHeight(0), m_isClippingEnabled(false), m_color(SUTK::Color4::yellow()), m_pointSize(12)
	{
		m_textGroup = getGfxDriver().createTextGroup(RenderMode::Transparent);
		m_baselineHeight = getGfxDriver().getTextGroupBaselineHeightInCentimeters(m_textGroup, m_pointSize);
	}

	Text::~Text() noexcept
	{
		auto& driver = getUIDriver();
		for(LineText* line : m_lines)
			driver.destroyObject<LineText>(line);
		driver.getGfxDriver().destroyTextGroup(m_textGroup);
	}

	f32 Text::calculateMaxWidth() const noexcept
	{
		// TODO: We can find maximum in 3 * n / 2 time remember? check CLRS, mean and order statistics,
		// However, for production ready builds, we can optimize this further,
		// 	by grouping the lines into fixed number of lines in each group,
		// 	this way we can limit the input size into this algo to a known size and optimal size.
		f32 maxWidth = 0;
		for(LineText* line : m_lines)
		{
			f32 width = line->getBoundingRectSize().width;
			if(width > maxWidth)
				maxWidth = width;
		}
		return maxWidth;
	}

	bool Text::isDirty()
	{
		if(Renderable::isDirty())
			return true;

		for(std::size_t i = 0; i < m_lines.size(); i++)
			if(m_lines[i]->isDirty())
				return true;

		return false;
	}

	void Text::update()
	{
		// Mandatory to be called in the overriding method
		Renderable::update();
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			LineText* line = m_lines[i];
			if(line->isDirty())
				line->update();
		}
	}

	void Text::updateNormalizedDrawOrder(f32 normalizedDrawOrder)
	{
		Renderable::updateNormalizedDrawOrder(normalizedDrawOrder);
		getGfxDriver().setTextGroupDepth(m_textGroup, normalizedDrawOrder);
	}

	LineCountType Text::getNumChars(const CursorPosition<LineCountType>& position) noexcept
	{
		std::size_t count = 0;

		// accumulate the num chars of each preceding lines
		for(std::size_t i = 0; i < std::min(m_lines.size(), static_cast<std::size_t>(position.getLine())); ++i)
			count += m_lines[i]->getColumnCount();
		// accumulate the num chars of the line at position.getLine() which preced position.getColumn()
		if(position.getLine() < m_lines.size())
			count += std::min(m_lines[position.getLine()]->getColumnCount(), static_cast<std::size_t>(position.getColumn()));
		return count;
	}

	void Text::addColorRange(const SelectionRange<LineCountType>& range, Color4 color) noexcept
	{
		std::size_t charCount = getCharCount();
		_com_assert(range.begin <= range.end);
		_com_assert((getNumChars(range.begin) <= charCount) && (getNumChars(range.end) <= charCount));
			
		auto& start = range.begin;
		auto& end = range.end;

		// color the intermediate lines if any
		LineCountType lineDiff = end.getLine() - start.getLine();
		for(LineCountType i = 1; i < lineDiff; i++)
			m_lines[start.getLine() + i]->addColorRange(0, std::string::npos, color);

		// if start and end are straddled across different lines
		if(start.getLine() != end.getLine())
		{
			// color characters from start.getColumn() to m_lines[start.getLine()].getColumnCount() exclusive
			LineText* startLineText = m_lines[start.getLine()];
			startLineText->addColorRange(start.getColumn(), std::string::npos, color);
	
			// color characters from 0 to end.getColumn() 
			LineText* endLineText = m_lines[end.getLine()];
			endLineText->addColorRange(0, std::min(end.getColumn(), static_cast<LineCountType>(endLineText->getColumnCount())), color);
		}
		// if start and end are in the same line
		else
		{
			LineText* lineText = m_lines[end.getLine()];
			std::size_t colDiff = end.getColumn() - start.getColumn();
			lineText->addColorRange(start.getColumn(), colDiff, color);
		}
	}

	void Text::setColorRanges(const std::vector<std::pair<SelectionRange<LineCountType>, Color4>>& ranges) noexcept
	{
		for(std::size_t i = 0; i < m_lines.size(); ++i)
			m_lines[i]->clearColorRanges();

		for(std::size_t i = 0; i < ranges.size(); i++)
		{
			const std::pair<SelectionRange<LineCountType>, Color4>& pair = ranges[i];
			addColorRange(pair.first, pair.second);
		}
	}

	void Text::setColor(Color4 color) noexcept
	{
		m_color = color;
		for(LineText* &lineText : m_lines)
			lineText->setColor(color);
	}

	Color4 Text::getColor() const noexcept
	{
		return m_color;
	}

	void Text::setClipRect(Rect2Df rect) noexcept
	{
		m_clipRect = rect;
		if(m_isClippingEnabled)
			recalculateClipRect();
	}

	Rect2Df Text::getClipRect() const noexcept
	{
		_com_assert(m_clipRect.has_value());
		return m_clipRect.value();
	}

		//		1. createNewLine
		//		2. removeLine
		//		3. removeLineRange
		//		5. append
		//		6. insert
		//		7. removeRange
		//		8. setFontSize

	Vec2Df Text::getBoundSize() const noexcept
	{
		f32 maxWidth = calculateMaxWidth();
		f32 height = (m_lines.size() > 0) ? (m_lines.back()->getPosition().y - m_lines.front()->getPosition().y + const_cast<Text*>(this)->getBaselineHeight()) : 0.0f;
		return { maxWidth, height };
	}

	void Text::setActive(com::Bool isActive) noexcept
	{
		Renderable::setActive(isActive);
		for(LineText* &lineText : m_lines)
			lineText->setActive(isActive);
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

	void Text::setFont(UIDriver::FontReference font) noexcept
	{
		getGfxDriver().setTextGroupFont(m_textGroup, font.getHandle());
	}

	Vec2Df Text::getLocalPositionFromCursorPosition(const CursorPosition<LineCountType>& cursor) noexcept
	{
		if(m_lines.size() == 0)
			return { m_scrollDelta.x, m_scrollDelta.y };
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
		return { static_cast<LineCountType>(lineNo), colNo };
	}

	// this creates a new line before line pointed by current cursor
	LineText* Text::createNewLine(Flags flags, LineCountType line) noexcept
	{
		CursorPosition<LineCountType> cursorPosition;

		if((line == 0) && (getLineCount() == 0))
			line = END_OF_TEXT;
		
		if(line == END_OF_TEXT)
		{
			line = m_lines.size();
			if(line >= 1u)
				line -= 1u;
		};
		cursorPosition.moveToLine(static_cast<LineCountType>(line));

		if(flags == Flags::After)
			cursorPosition.moveToNextLine(m_lines.size());

		// shift the line at which the cursor points to and the lines succeeding it
		for(std::size_t i = cursorPosition.getLine(); i < m_lines.size(); i++)
			m_lines[i]->addPosition({ 0, getBaselineHeight() });

		// insert the line at which the cursor points to
		LineText* lineText = getUIDriver().createObject<LineText>(m_textGroup);
		m_lines.insert(std::next(m_lines.begin(), cursorPosition.getLine()), lineText);

		lineText->setFontSize(m_pointSize);
		lineText->setColor(m_color);
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
		m_lines.erase(std::next(m_lines.begin(), line));

		// shift the line at which the cursor points to and the lines succeeding it
		for(std::size_t i = line; i < m_lines.size(); i++)
			m_lines[i]->addPosition({ 0, -getBaselineHeight() });
	}

	void Text::removeLineRange(LineCountType start, LineCountType end) noexcept
	{
		if(end > m_lines.size())
			end = static_cast<LineCountType>(m_lines.size());
		for(LineCountType i = start; i < end; ++i)
			m_lines[i]->setData("");
		if(start < end)
			m_lines.erase(std::next(m_lines.begin(), start), std::next(m_lines.begin(), end));
		// shift the line at which the cursor points to and the lines succeeding it
		for(std::size_t i = start; i < m_lines.size(); i++)
			m_lines[i]->addPosition({ 0, -getBaselineHeight() * (end - start) });
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

	CursorPosition<LineCountType> Text::insert(CursorPosition<LineCountType> position, const std::string& str) noexcept
	{
		// if there is nothing to write then return
		if(str.empty())
		{
			if(position > end())
				return end();
		}

		LineText* lineText = NULL;
		if(position >= end())
			position = CursorPosition<LineCountType>::EndOfText();
		 
		 lineText = getLine(position.getLine());

		// Take line-endings into account
		// Possible line endings are: '\r', '\r\n', and '\n'.
		bool isCarriageReturn = false;
		std::string::size_type index = str.find_first_of('\r');
		if(index == std::string::npos)
			index = str.find_first_of('\n');
		else
			isCarriageReturn = true;

		if(index == std::string::npos)
			lineText->insert(position.getColumn(), str);
		else
			lineText->insert(position.getColumn(), str.substr(0, index));

		LineText* newLineText = NULL;
		auto line = (position.getLine() == END_OF_LINE) ? (getLineCount() - 1) : position.getLine();
		auto saveIndex = index;
		while(index != std::string::npos)
		{
			// create a new line
			newLineText = createNewLine(Flags::After, line++);
			
			// character just after '\n'
			++index;
			if(isCarriageReturn && (str[index] == '\n'))
				++index;

			// if the new line character is at the very end, then no extra characters to add,
			// i.e. it is an empty new line
			if(index == str.size())
				break;

			// determine the substring to be added into the just created new line
			// i.e. the substring is sandwitched with either '\n' characters on both the sides (left and right),
			// or '\n' character on left and end of the string on the right.

			auto end = str.find_first_of('\r', index);
			if(end == std::string::npos)
			{
				end = str.find_first_of('\n', index);
				isCarriageReturn = false;
			}
			else isCarriageReturn = true;


			// if no new line character on the right side, then append rest of the string
			if(end == std::string::npos)
				newLineText->append(str.substr(index, std::string::npos));
			// otherwise, append a substring surrounded by '\n' charactesr from both the sides (left and right)
			else
			{
				_com_assert(end >= index);
				newLineText->append(str.substr(index, end - index));
			}
			index = end;
		}

		LineCountType column = 0;
		if(newLineText == NULL)
		{
			if(position.getColumn() == END_OF_LINE)
				column = lineText->getColumnCount();
			else 
			{
				if(index == std::string::npos)
					column = position.getColumn() + str.size();
				else
					column = position.getColumn() + index + 2;
			}
		}
		else
			column = newLineText->getColumnCount();

		if((newLineText != NULL) && (position.getColumn() != END_OF_LINE))
		{
			newLineText->append(lineText->substr(position.getColumn() + saveIndex, std::string::npos));
			lineText->removeRange(position.getColumn() + saveIndex);
		}

		return { static_cast<LineCountType>(line), column };
	}

	void Text::removeRange(CursorPosition<LineCountType> start, CursorPosition<LineCountType> end) noexcept
	{
		_com_assert(start.getLine() < m_lines.size());
		_com_assert(end.getLine() < m_lines.size());
		//  start line ----- 	 	lower line number
		//  ---------------- <-- intermediate line
		//  ---------------- <-- intermediate line
		/// ---------- end line  	higher line number
		_com_assert(start.getLine() <= end.getLine());

		// remove the intermediate lines if any
		removeLineRange(start.getLine() + 1, end.getLine());

		// shift the end upwards to start.getLine() + 1, as intermediate lines have been removed (if any)
		if(start.getLine() < end.getLine())
			end.moveToLine(start.getLine() + 1);

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
		Rect2Df clipRect;
		if(m_clipRect.has_value())
			clipRect = m_clipRect.value();
		else
		{
			RenderableContainer* container = getContainer();
			// convert the top-left corner of the container, in which this text is, to screen coordinates
			auto position = container->getLocalCoordsToScreenCoords({ 0u, 0u });
			clipRect = { position, container->getSize() };
		}
		
		for(std::size_t i = 0; i < m_lines.size(); i++)
		{
			// set the clip rect, note that width and height should remain as that of its text container
			m_lines[i]->setClipRectGlobalCoords(clipRect);
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

	std::size_t Text::getCharCount() const noexcept
	{
		std::size_t charCount = 0;
		for(LineText* const &lineText : m_lines)
			charCount += lineText->getData().size();
		return charCount;
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
		return m_baselineHeight;
	}

	CursorPosition<LineCountType> Text::begin() const noexcept
	{
		return { 0, 0 };
	}

	CursorPosition<LineCountType> Text::end() const noexcept
	{
		if(m_lines.size() == 0)
			return { 0, 0 };
		return { static_cast<LineCountType>(m_lines.size() - 1), static_cast<LineCountType>(m_lines.back()->getColumnCount()) };
	}

	void Text::serialize(std::ostream& stream) const noexcept
	{
		for(std::size_t i = 0; i < getLineCount(); i++)
		{
			const SUTK::LineText* lineText = getLine(i);
			const auto& str = static_cast<const std::string&>(lineText->getData());
			stream.write(reinterpret_cast<const char*>(str.data()), str.size());
			if((i + 1) < getLineCount())
				stream.write("\n", 1);
		}
	}

	void Text::serializeToFile(const std::string& filePath) const noexcept
	{
		std::ofstream file(filePath, std::ios::out | std::ios::trunc);
		if(!file.is_open())
		{
			debug_log_error("Failed to open file: %.*s", filePath.size(), filePath.data());
			return;
		}
		serialize(file);
		file.close();
	}

	void Text::deserialize(std::istream& stream) noexcept
	{
		std::streamsize beginPos = stream.tellg();
		stream.seekg(0, std::ios::end);
		std::streamsize endPos = stream.tellg();
		std::streamsize size = endPos - beginPos;
		stream.seekg(beginPos);
		if(size == 0)
			return;
		std::string buffer(size, '\0');
		char* data = reinterpret_cast<char*>(&buffer[0]);
		stream.read(data, size);
		std::streamsize readSize = stream.gcount();
		// ERROR: For some reason, this assertion always fails. Therefore use Text::deserializeFromFile() for now
		_com_assert(static_cast<std::streamsize>(readSize) == size);
		set(buffer);
	}

	void Text::deserializeFromFile(const std::string& filePath, CursorPosition<LineCountType> pos) noexcept
	{
		BUFFER* buffer = load_text_from_file_s(filePath.c_str());
		if(buffer == NULL)
		{
			debug_log_error("Failed to open file: %.*s", filePath.size(), filePath.data());
			return;
		}
		std::string str(reinterpret_cast<const char*>(buf_get_ptr(buffer)), buf_get_element_count(buffer));
		insert(pos, str);
		buf_free(buffer);
	}

	void Text::setFontSize(const f32 pointSize) noexcept
	{
		// update the point sizes for each line
		for(LineText*& lineText : m_lines)
			lineText->setFontSize(pointSize);
		m_pointSize = pointSize;

		// update the baseline height
		if(m_lines.size() > 0)
			m_baselineHeight = getGfxDriver().getTextGroupBaselineHeightInCentimeters(m_textGroup, m_pointSize);

		updateLinePositions();
	}

	f32 Text::getFontSize() const noexcept
	{
		return m_pointSize;
	}
}