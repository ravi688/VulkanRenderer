#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	// Typographical Emphasises
	enum TPGEmphasis : u8
	{
		Bold,
		Italics,
		Underlined,
		Strikethrough
	};

	enum class HorizontalAlignment : u8
	{
		Invalid,
		Left,
		Right,
		Middle
	};

	enum class VerticalAlignment : u8
	{
		Invalid,
		Top,
		Bottom,
		Middle
	};
};
