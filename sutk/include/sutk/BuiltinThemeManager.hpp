#pragma once

#include <sutk/ThemeManager.hpp> // for SUTK::ThemeManager<>

#include <string> // for std::string
#include <string_view> // for std::string_view

namespace SUTK
{
	class SUTK_API BuiltinThemeManager : public ThemeManager<std::string, std::string_view>
	{
		friend class UIDriver;
	private:
		BuiltinThemeManager(UIDriver& driver) noexcept;

	public:
		void applyTheme(const std::string_view themeName) noexcept;
	};
}