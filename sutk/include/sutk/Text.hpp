#pragma once

#include <sutk/defines.hpp>

#include <string> /* for std::string */

namespace SUTK
{
	class Container;
	class UIDriver;
	
	class Text
	{
	private:
		UIDriver& m_driver;
		Container* m_container;

	public:
		Text(UIDriver& driver, Container* container = NULL);

		void set(const std::string& str);
		void set(const std::string& str, LineCountType lineNo);
		void append(const std::string& str);
		void appendChar(const CharType ch);
		void insert(const std::string& str, LineCountType lineNo, LineCountType columnNo);
		void insertChar(const CharType ch, LineCountType lineNo, LineCountType columnNo);

		LineCountType getLineCount() const;
		LineCountType getColumnCount(LineCountType lineNo) const;
	};
}
