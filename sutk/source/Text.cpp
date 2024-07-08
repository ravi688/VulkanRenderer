#include <sutk/Text.hpp>

namespace SUTK
{
	Text::Text(UIDriver& driver, Container* container) : m_driver(driver), m_container(container)
	{

	}

	void Text::set(const std::string& str)
	{

	}

	void Text::set(const std::string& str, LineCountType lineNo)
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

	LineCountType Text::getLineCount() const
	{
		return 0u;
	}

	LineCountType Text::getColumnCount(LineCountType lineNo) const
	{
		return 0u;
	}
}