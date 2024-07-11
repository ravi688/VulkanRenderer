#include <sutk/Text.hpp>

#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IGfxDriver.hpp> /* for SUTK::IGfxDriver::createText() */

#include <common/assert.h> /* for _assert() */

namespace SUTK
{
	LineText::LineText(UIDriver& driver) noexcept : UIDriverObject(driver), m_isPosDirty(false), m_isDataDirty(false)
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
	void LineText::setPosition(Vec2D<DisplaySizeType> pos) noexcept
	{
		if(m_pos == pos)
			return;
		m_pos = pos;
		m_isPosDirty = true;
	}
	void LineText::addDisplacement(Vec2D<DisplaySizeType> pos) noexcept
	{
		if(pos == Vec2D<DisplaySizeType>::zero())
			return;
		m_pos += pos;
		m_isPosDirty = true;
	}
	void LineText::clear() noexcept
	{
		m_data.clear();
	}

	Text::Text(UIDriver& driver, TextContainer* container) : UIDriverObject(driver), m_container(container), m_isDirty(false)
	{

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

	void Text::clear()
	{
		// clear existing data
		// NOTE: here clearing means, the CPU side data clear.
		// GPU side data will be cleared upon call to IRenderable::update as IRenderable::isDirty() will return true.
		// This is to ensure that we do not write to GPU side memory multiple times in a single frame.
		// In this case, we are (want to) first clearing the data and then write again, so there could have been double writes to GPU side storage!
		for(std::size_t i = 0; i < m_lines.size(); i++)
			m_lines[i]->clear();
	}

	LineText* Text::createNewLine()
	{
		LineText* lineText = new LineText(getUIDriver());
		m_lines.push_back(lineText);
		return lineText;
	}

	LineText* Text::getLastLine()
	{
		// if there are no lines
		if(m_lines.size() == 0)
			// create a new line (the very first one; it would also be the last line as it is the only line currently)
			return createNewLine();
		// otherwise, the last 
		return m_lines.back();
	}

	void Text::append(const std::string& str)
	{
		// if there is nothing to write then return
		if(str.empty())
			return;

		// append the first set of character preceding the first line break,
		// or all character if no line break exists.
		std::string::size_type index = str.find_first_of('\n');
		if(index == std::string::npos)
			getLastLine()->append(str);
		else
			getLastLine()->append(str.substr(0, index));


		while(index != std::string::npos)
		{
			// create a new line
			auto line = createNewLine();
			
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
				line->append(str.substr(index, std::string::npos));
			// otherwise, append a substring surrounded by '\n' charactesr from both the sides (left and right)
			else
				line->append(str.substr(index, end - index));
			index = end;
		}
	}

	void Text::set(const std::string& str)
	{
		// clear the exisiting data
		clear();
		// wring new data
		append(str);
	}
}