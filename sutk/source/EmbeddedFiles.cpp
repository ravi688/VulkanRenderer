
/***This is computer generated file - Do not modify it***/

/* This is auto generated file (by generate_embedded_files.py python script). Do not modify it directly.
 * Time & Date (yy/mm/yyy) of Generation: 18h:6m:34s, 20/12/2024
 */

#include <sutk/EmbeddedFiles.hpp>
#include <common/assert.h>
#include <common/debug.h>
#include <string.h>

namespace SUTK
{
	namespace EmbeddedFiles
	{
static const char* __THEMES_MODEL_TMDL = 
"[Name(\"GlobalThemeModel\")]\n"
"ThemeModel\n"
"{\n"
"	[Color]\n"
"	LargeButton.IdleColor;\n"
"	[Color]\n"
"	LargeButton.HoverColor;\n"
"	[Color]\n"
"	LargeButton.PressColor;\n"
"	[Color]\n"
"	SmallButton.IdleColor;\n"
"	[Color]\n"
"	SmallButton.HoverColor;\n"
"	[Color]\n"
"	SmallButton.PressColor;\n"
"	[Image]\n"
"	Folder.Open;\n"
"	[Image]\n"
"	Folder.Open.VG;\n"
"	[Image]\n"
"	Folder.Close;\n"
"	[Float]\n"
"	AnotherButton.AlphaValue;\n"
"	[String]\n"
"	Label.Value;\n"
"}";
static const char* __THEMES_THEME1_THEME = 
"[Name(\"My Theme1\")]\n"
"[Model(\"GlobalThemeModel\")]\n"
"Theme\n"
"{\n"
"    LargeButton.IdleColor = green { };\n"
"    LargeButton.HoverColor = red { };\n"
"    LargeButton.PressColor = grey { };\n"
"    SmallButton.IdleColor = green { };\n"
"    SmallButton.HoverColor = red { 1.0f };\n"
"    SmallButton.PressColor = grey { 1.0f };\n"
"    Folder.Open = \"../textures/Thalita-Torres-Office-Archive-folders.512.png\";\n"
"    Folder.Open.VG = \"svg_files/close-cross-symbol-in-a-circle-svgrepo-com.svg\";\n"
"    Folder.Close = \"icons/Folder-Close.png\";\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.7f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 1\";\n"
"}\n"
"";
static const char* __THEMES_THEME2_THEME = 
"[Name(\"My Theme2\")]\n"
"[Model(\"GlobalThemeModel\")]\n"
"Theme\n"
"{\n"
"    LargeButton.IdleColor = green { };\n"
"    LargeButton.HoverColor = red { 0.5f };\n"
"    LargeButton.PressColor = grey { };\n"
"    SmallButton.IdleColor = green { };\n"
"    SmallButton.HoverColor = red { 1.0f };\n"
"    SmallButton.PressColor = grey { 1.0f };\n"
"    Folder.Open = \"../textures/Smile.bmp\";\n"
"    Folder.Open.VG = \"svg_files/apple-svgrepo-com.svg\";\n"
"    Folder.Close = \"icons/Folder-Close.png\";\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.05f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 2\";\n"
"}\n"
"";
static const char* __THEMES_DARK_LIGHT_THEME_TEST_DARK_THEME = 
"[Name(\"sutk.dark\")]\n"
"[Model(\"sutk\")]\n"
"Theme\n"
"{\n"
"\n"
"}\n"
"";
static const char* __THEMES_DARK_LIGHT_THEME_TEST_LIGHT_THEME = 
"[Name(\"sutk.light\")]\n"
"[Model(\"sutk\")]\n"
"Theme\n"
"{\n"
"\n"
"}\n"
"";
static const char* __THEMES_DARK_LIGHT_THEME_TEST_SUTK_TMDL = 
"[Name(\"sutk\")]\n"
"ThemeModel\n"
"{\n"
"\n"
"}";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL1_TMDL = 
"[Name(\"Theme Model 1\")]\n"
"ThemeModel\n"
"{\n"
"	[Color]\n"
"	LargeButton.IdleColor;\n"
"	[Color]\n"
"	LargeButton.HoverColor;\n"
"	[Color]\n"
"	LargeButton.PressColor;\n"
"	[Color]\n"
"	SmallButton.IdleColor;\n"
"	[Color]\n"
"	SmallButton.HoverColor;\n"
"	[Color]\n"
"	SmallButton.PressColor;\n"
"	[Image]\n"
"	Folder.Open;\n"
"	[Image]\n"
"	Folder.Open.VG;\n"
"	[Image]\n"
"	Folder.Close;\n"
"}";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL2_TMDL = 
"[Name(\"Theme Model 2\")]\n"
"ThemeModel\n"
"{\n"
"	[Float]\n"
"	AnotherButton.AlphaValue;\n"
"	[String]\n"
"	Label.Value;\n"
"}";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL3_TMDL = 
"[Name(\"Theme Model 3\")]\n"
"ThemeModel\n"
"{\n"
"	[Float]\n"
"	AnotherLabel.Color.r;\n"
"	[Float]\n"
"	AnotherLabel.Color.g;\n"
"	[Float]\n"
"	AnotherLabel.Color.b;\n"
"	[Float]\n"
"	AnotherLabel.Color.a;\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME1_THEME = 
"[Name(\"My Theme 1\")]\n"
"[Model(\"Theme Model 1\")]\n"
"[Model(\"Theme Model 2\")]\n"
"[Model(\"Theme Model 3\")]\n"
"Theme\n"
"{\n"
"    LargeButton.IdleColor = green { };\n"
"    LargeButton.HoverColor = red { };\n"
"    LargeButton.PressColor = grey { };\n"
"    SmallButton.IdleColor = green { };\n"
"    SmallButton.HoverColor = red { 1.0f };\n"
"    SmallButton.PressColor = grey { 1.0f };\n"
"    Folder.Open = \"../textures/Thalita-Torres-Office-Archive-folders.512.png\";\n"
"    Folder.Open.VG = \"svg_files/close-cross-symbol-in-a-circle-svgrepo-com.svg\";\n"
"    Folder.Close = \"icons/Folder-Close.png\";\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.7f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 1\";\n"
"    [Float]\n"
"    AnotherLabel.Color.r = 1.0f;\n"
"    [Float]\n"
"    AnotherLabel.Color.g = 0.0f;\n"
"    [Float]\n"
"    AnotherLabel.Color.b = 1.0f;\n"
"    [Float]\n"
"    AnotherLabel.Color.a = 1.0f;\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME2_THEME = 
"[Name(\"My Theme 2\")]\n"
"[Model(\"Theme Model 1\")]\n"
"[Model(\"Theme Model 2\")]\n"
"Theme\n"
"{\n"
"    LargeButton.IdleColor = green { };\n"
"    LargeButton.HoverColor = red { 0.5f };\n"
"    LargeButton.PressColor = grey { };\n"
"    SmallButton.IdleColor = green { };\n"
"    SmallButton.HoverColor = red { 1.0f };\n"
"    SmallButton.PressColor = grey { 1.0f };\n"
"    Folder.Open = \"../textures/Smile.bmp\";\n"
"    Folder.Open.VG = \"svg_files/apple-svgrepo-com.svg\";\n"
"    Folder.Close = \"icons/Folder-Close.png\";\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.05f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 2\";\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME3_THEME = 
"[Name(\"My Theme 3\")]\n"
"[Model(\"Theme Model 1\")]\n"
"Theme\n"
"{\n"
"    LargeButton.IdleColor = green { };\n"
"    LargeButton.HoverColor = blue { 0.7f };\n"
"    LargeButton.PressColor = grey { };\n"
"    SmallButton.IdleColor = green { };\n"
"    SmallButton.HoverColor = red { 1.0f };\n"
"    SmallButton.PressColor = grey { 1.0f };\n"
"    Folder.Open = \"../textures/Smile.bmp\";\n"
"    Folder.Open.VG = \"svg_files/apple-svgrepo-com.svg\";\n"
"    Folder.Close = \"icons/Folder-Close.png\";\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.5f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 3\";\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME4_THEME = 
"[Name(\"My Theme 4\")]\n"
"[Base(\"My Theme 1\")]\n"
"Theme\n"
"{\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME5_THEME = 
"[Name(\"My Theme 5\")]\n"
"[Base(\"My Theme 1\")]\n"
"Theme\n"
"{\n"
"    [Float]\n"
"    AnotherButton.AlphaValue = 0.8f;\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 5\";\n"
"}\n"
"";
static const char* __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME6_THEME = 
"[Name(\"My Theme 6\")]\n"
"[Base(\"My Theme 5\")]\n"
"[Model(\"Theme Model 3\")]\n"
"Theme\n"
"{\n"
"    [String]\n"
"    Label.Value = \"Hello World From Theme 6\";\n"
"\n"
"    [Float]\n"
"    AnotherLabel.Color.b = 0.5f;\n"
"    AnotherLabel.Color.r = 0.0f;\n"
"    AnotherLabel.Color.g = 1.0f;\n"
"}\n"
"";

		typedef struct file_path_and_data_mapping_t
		{
			const char* file_path;
			const char* data;
			long long data_size;
		} file_path_and_data_mapping_t;
		
		#define G_FILE_MAPPING_COUNT 15
		
		static file_path_and_data_mapping_t g_shader_mappings[G_FILE_MAPPING_COUNT];
		static bool g_is_file_mappings_populated = false;
		
		static __attribute__((unused)) const file_path_and_data_mapping_t* g_get_file_mappings()
		{
			if(!g_is_file_mappings_populated)
			{
		g_shader_mappings[0] = (file_path_and_data_mapping_t) { "./themes/model.tmdl", __THEMES_MODEL_TMDL, 376 };
		g_shader_mappings[1] = (file_path_and_data_mapping_t) { "./themes/theme1.theme", __THEMES_THEME1_THEME, 613 };
		g_shader_mappings[2] = (file_path_and_data_mapping_t) { "./themes/theme2.theme", __THEMES_THEME2_THEME, 558 };
		g_shader_mappings[3] = (file_path_and_data_mapping_t) { "./themes/dark_light_theme_test/dark.theme", __THEMES_DARK_LIGHT_THEME_TEST_DARK_THEME, 47 };
		g_shader_mappings[4] = (file_path_and_data_mapping_t) { "./themes/dark_light_theme_test/light.theme", __THEMES_DARK_LIGHT_THEME_TEST_LIGHT_THEME, 48 };
		g_shader_mappings[5] = (file_path_and_data_mapping_t) { "./themes/dark_light_theme_test/sutk.tmdl", __THEMES_DARK_LIGHT_THEME_TEST_SUTK_TMDL, 30 };
		g_shader_mappings[6] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/model1.tmdl", __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL1_TMDL, 313 };
		g_shader_mappings[7] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/model2.tmdl", __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL2_TMDL, 98 };
		g_shader_mappings[8] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/model3.tmdl", __THEMES_MULTIPLE_THEME_MODEL_TEST_MODEL3_TMDL, 167 };
		g_shader_mappings[9] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme1.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME1_THEME, 841 };
		g_shader_mappings[10] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme2.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME2_THEME, 581 };
		g_shader_mappings[11] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme3.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME3_THEME, 556 };
		g_shader_mappings[12] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme4.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME4_THEME, 52 };
		g_shader_mappings[13] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme5.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME5_THEME, 160 };
		g_shader_mappings[14] = (file_path_and_data_mapping_t) { "./themes/multiple_theme_model_test/theme6.theme", __THEMES_MULTIPLE_THEME_MODEL_TEST_THEME6_THEME, 248 };

				g_is_file_mappings_populated = true;
			}
			return g_shader_mappings;
		}

		static bool str_ends_with(const char* str, const char* suffix)
		{
			u32 str_len = strlen(str);
			u32 sfx_len = strlen(suffix);
		
			/* screening case */
			if(str_len < sfx_len)
				return false;
		
			return strncmp(str + (str_len - sfx_len), suffix, sfx_len) == 0;
		}

		static const char* get_builtin_file_data(const char* virtual_file_path, u64 OUT data_size)
		{
			_com_assert(strlen(virtual_file_path) > 0);
			if(virtual_file_path[0] != '/')
			{
				debug_log_error("Invalid root directory in %s", virtual_file_path);
				return NULL;
			}
		
			/* skip first '/' (possibly multiple adjacent) characters */
			while(*virtual_file_path == '/')
				virtual_file_path += 1;
		
			/* find a match */
			u32 match_index = U32_MAX;
			const file_path_and_data_mapping_t* mappings = g_get_file_mappings();
			for(u32 i = 0; i < G_FILE_MAPPING_COUNT; i++)
			{
				if(str_ends_with(mappings[i].file_path, virtual_file_path))
				{
					if(match_index != U32_MAX)
					{
						debug_log_error("Multiple matches were found, returning the first match: %s", mappings[i].file_path);
						break;
					}
					else match_index = i;
				}
			}
		
			if(match_index == U32_MAX)
			{
				debug_log_warning("No match found for virtual file path: %s, returning NULL", virtual_file_path);
				return NULL;
			}
		
			_com_assert(match_index < G_FILE_MAPPING_COUNT);
		
			if(data_size != NULL)
				OUT data_size = CAST_TO(u64, mappings[match_index].data_size);
		
			return mappings[match_index].data;
		}

		std::optional<std::span<const std::byte>> getData(const std::string_view filePath) noexcept
		{
			char buffer[filePath.length() + 1];
			memcpy(buffer, filePath.data(), filePath.length());
			buffer[filePath.length()] = 0;
			u64 data_size = 0;
			const char* data = get_builtin_file_data(buffer, &data_size);
			if(!data)
				return { };
			return { std::span<const std::byte> { reinterpret_cast<const std::byte*>(data), data_size } };
		}
	}
}
