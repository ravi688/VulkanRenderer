#pragma once

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class Text;
	class Container;

	class Container
	{
	private:
		UIDriver& m_driver;
	public:
		Container(SUTK::UIDriver& driver);

		void add(Container* container);
		void add(Text* text);
	};
}
