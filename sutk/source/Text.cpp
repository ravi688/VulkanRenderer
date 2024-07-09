#include <sutk/Text.hpp>

#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IGfxDriver.hpp> /* for SUTK::IGfxDriver::createText() */

#include <common/assert.h> /* for _assert() */

namespace SUTK
{
	void SubText::setData(const std::string& data) noexcept
	{

	}
	void SubText::setColor(const Color3 color) noexcept
	{

	}
	void SubText::setEmphasis(TPGEmphasis emphasis) noexcept
	{

	}
	void SubText::setFont(GfxDriverObjectHandleType font) noexcept
	{

	}
	void SubText::setSize(u32 pointSize) noexcept
	{

	}

	Text::Text(UIDriver& driver, TextContainer* container) : m_driver(driver), m_container(container)
	{
		m_handle = driver.getGfxDriver().createText();
	}

	void Text::update()
	{
		_assert(isDirty());
	}

	void Text::set(const std::string& str)
	{
		
	}

	void Text::set(const std::string& str, LineCountType lineNo)
	{

	}

	void Text::set(std::string&& str)
	{

	}

	void Text::set(std::string&& str, LineCountType lineNo)
	{
		
	}

	void Text::append(const std::string& str)
	{

	}

	void Text::appendChar(const CharType ch)
	{

	}

	void Text::insert(const std::string& str, LineCountType lineNo, LineCountType columnNo)
	{

	}

	void Text::insertChar(const CharType ch, LineCountType lineNo, LineCountType columnNo)
	{

	}

	void Text::setColor(const Color3 color) noexcept
	{

	}
	void Text::setEmphasis(const TPGEmphasis style) noexcept
	{

	}
	void Text::setFont(GfxDriverObjectHandleType font) noexcept
	{

	}
	void Text::setSize(u32 pointSize) noexcept
	{

	}
	void Text::setHorizontalAlignment(const HorizontalAlignment alignment) noexcept
	{

	}
	void Text::setVerticalAlignment(const VerticalAlignment alignment) noexcept
	{

	}

	LineCountType Text::getLineCount() const
	{
		return 0u;
	}

	LineCountType Text::getColumnCount(LineCountType lineNo) const
	{
		return 0u;
	}
}