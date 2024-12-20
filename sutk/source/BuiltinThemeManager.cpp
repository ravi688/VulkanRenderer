#include <sutk/BuiltinThemeManager.hpp>
#include <common/debug.hpp>

#include <sutk/EmbeddedFiles.hpp> // for SUTK::EmbeddedFiles

namespace SUTK
{
	BuiltinThemeManager::BuiltinThemeManager(UIDriver& driver) noexcept : ThemeManager<std::string, std::string_view>(driver)
	{
		auto data = EmbeddedFiles::getData("/themes/dark_light_theme_test/sutk.tmdl");
		COM_ASSERT(COM_DESCRIPTION(data), "Failed to load theme model themes/dark_light_theme_test/sutk.tmdl");
		auto* themeModel = loadThemeInterfaceStr(com::span_to_string_view(*data));
		COM_ASSERT(COM_DESCRIPTION(themeModel), "Faild to parse theme model themes/dark_light_theme_test/sutk.tmdl");
		com::BinaryFileLoadCallback fileLoadCallbackHandler = [](const std::string_view filePath) noexcept -> com::BinaryFileLoadResult
		{
			std::string str = "/";
			str.append(filePath);
			std::optional<std::span<const std::byte>> data = EmbeddedFiles::getData(filePath);
			if(!data)
				return { };
			std::span _data { data->data(), data->size() };
			return { _data , nullptr, nullptr };
		};
		data = EmbeddedFiles::getData("/themes/dark_light_theme_test/dark.theme");
		if(data)
		{
			auto* theme = loadThemeStr(com::span_to_string_view(*data), fileLoadCallbackHandler);
			if(!theme)
				DEBUG_LOG_ERROR("Dark theme parsing failed");
		}
		else DEBUG_LOG_ERROR("Dark theme file themes/dark_light_theme_test/dark.theme load failed");
		data = EmbeddedFiles::getData("/themes/dark_light_theme_test/light.theme");
		if(data)
		{
			auto* theme = loadThemeStr(com::span_to_string_view(*data), fileLoadCallbackHandler);
			if(theme)
				theme->apply();
			else
				DEBUG_LOG_ERROR("Light theme parsing failed");
		}
		else DEBUG_LOG_ERROR("Light theme file themes/dark_light_theme_test/light.theme load failed");
	}

	void BuiltinThemeManager::applyTheme(const std::string_view themeName) noexcept
	{
		auto* theme = getTheme(themeName);
		if(theme)
			theme->apply();
		else DEBUG_LOG_ERROR("applyTheme() failed, no theme exists with name \"%*.s\"", themeName.length(), themeName.data());
	}
}