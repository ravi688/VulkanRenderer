#include <sutk/ThemeManager.hpp>

#include <common/string.h> // for com_safe_strncmp()
#include <common/debug.h> // for DEBUG_LOG_ERROR()
#include <disk_manager/file_reader.h> // for load_text_from_file()
#include <phymac_parser/v3d_generic.h> // for ppsr_v3d_generic_parse()

namespace SUTK
{
	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeInterfaceType* ThemeManager<std::string, std::string_view>::loadThemeInterface(const std::string_view filePath) noexcept
	{
		BUFFER* text = NULL;
		{
			std::string str { filePath };
			text = load_text_from_file(str.c_str());
		}
		char* str = reinterpret_cast<char*>(buf_get_ptr(text));
		ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(NULL, str, buf_get_element_count(text) - 1);
		ThemeInterfaceType* interface = NULL;
		std::string_view strView { };
		v3d_generic_node_t* themeModelNode = NULL;
		if(result.result != PPSR_SUCCESS)
		{
			DEBUG_LOG_ERROR("Unable to parse the file at %.*s", filePath.length(), filePath.data());
			goto HANDLE_FAILURE;
		}
		if(result.root->child_count == 0)
		{
			DEBUG_LOG_ERROR("Parse is empty, perhaps the file was empty?");
			goto HANDLE_FAILURE;
		}

		themeModelNode = result.root->childs[0];
		if(themeModelNode->qualifier_count == 0)
		{
			DEBUG_LOG_ERROR("Anonymous root block is not allowed, it must be named \"ThemeModel\"");
			goto HANDLE_FAILURE;
		}

		{
			u32_pair_t name = themeModelNode->qualifiers[0];
			if(com_safe_strncmp(name.start + str, "ThemeModel", U32_PAIR_DIFF(name)))
			{
				DEBUG_LOG_ERROR("Expected \"ThemeModel\" but given \"%*.s\"", U32_PAIR_DIFF(name), name.start + str);
				goto HANDLE_FAILURE;
			}
		}

		{
			v3d_generic_attribute_t* nameAttr = node_find_attribute(themeModelNode, str, "Name");
			if(!nameAttr)
			{
				DEBUG_LOG_ERROR("[Name] attribute is expected on \"ThemeModel\" block, but not given");
				goto HANDLE_FAILURE;
			}
			if(nameAttr->argument_count == 1)
			{
				strView = std::string_view { nameAttr->arguments[0].start + str, U32_PAIR_DIFF(nameAttr->arguments[0]) };
				interface = createThemeInterface(strView);
				for(u32 i = 0; i < themeModelNode->child_count; ++i)
				{
					v3d_generic_node_t* child = themeModelNode->childs[i];
					if(!child->qualifier_count)
					{
						DEBUG_LOG_ERROR("Anonymous (empty names) aren't allowed");
						goto HANDLE_FAILURE;
					}
					u32_pair_t name = child->qualifiers[0];
					std::string_view nameSV = std::string_view { name.start + str, U32_PAIR_DIFF(name) };
					v3d_generic_attribute_t* typeAttr = node_find_attribute(child, str, "Color");
					if(typeAttr)
					{
						interface->define<Color4>(nameSV);
						continue;
					}
					typeAttr = node_find_attribute(child, str, "Image");
					if(typeAttr)
					{
						interface->define<UIDriver::ImageReference>(nameSV);
						continue;
					}
					DEBUG_LOG_WARNING("Type either isn't recognized or not given, skipping \"%.*s\"", nameSV.length(), nameSV.data());
				}
				ppsr_v3d_generic_parse_result_destroy(NULL, result);
				buf_free(text);
				return interface;
			}
			else
			{
				DEBUG_LOG_ERROR("Arguments mismatch, \"Name\" attribute accepts one argument (string), but either not given or more provided");
				goto HANDLE_FAILURE;
			}
		}

	HANDLE_FAILURE:
		if(interface)
			destroyThemeInterface(strView);
		ppsr_v3d_generic_parse_result_destroy(NULL, result);
		buf_free(text);
		return NULL;
	}

	static const char* gColorMnemonics[] =
	{
		"green",
		"red",
		"white",
		"yellow",
		"pink",
		"grey",
		"black",
		"blue",
		"brown"
	};

	static const Color4 gColorPresets[SIZEOF_ARRAY(gColorMnemonics)] = 
	{
		Color4::green(),
		Color4::red(),
		Color4::white(),
		Color4::yellow(),
		Color4::pink(),
		Color4::grey(),
		Color4::black(),
		Color4::blue(),
		Color4::brown()
	};

	static u32 strs_find_u32_pair(const char* str, const char* const* strs, u32 str_count, u32_pair_t pair)
	{
		for(u32 i = 0; i < str_count; i++)
			if(com_safe_strncmp(pair.start + str, strs[i], U32_PAIR_DIFF(pair)) == 0)
				return i;
		return U32_MAX;
	}

	static f32 u32_pair_to_f32(const char* str, u32_pair_t pair)
	{
		char buf[U32_PAIR_DIFF(pair) + 1] = { };
		memcpy(buf, pair.start + str, U32_PAIR_DIFF(pair));
		return std::strtof(buf, NULL);
	}

	template<>
	template<>
	Color4 ThemeManager<std::string, std::string_view>::deriveValue<Color4>(v3d_generic_node_t* node, const char* str) noexcept
	{
		// Followings are the possiblies:
		//	1. { }, { 0 }, { 0, 0 }, { 0, 0, 0 }, { 0, 0, 0, 0 }
		// 	2. green, red, blue, white, yellow, pink, grey, and black
		// 	3. green { }, red { }, blue { }, white { }, yellow { }, pink { }, grey { }, and black { }
		// 	4. green { 0.1f }, red { 0.3f }, blue { 3.0f }, white { 5.0f }, yellow { 4.0f }, pink { 65.0f }, grey { 5.0f }, and black { 0.23f }
		Color4 color;
		if(node->qualifier_count == 0)
		{
			static_assert(sizeof(color) == 4);
			u8* bytes = reinterpret_cast<u8*>(&color);
			for(u32 i = 0; i < node->child_count; ++i)
			{
				if(node->qualifier_count == 0)
					continue;
				u32_pair_t pair = node->qualifiers[0];
				f32 fltValue = u32_pair_to_f32(str, pair);
				if(!com::LiesBetweenInc(fltValue, 0.0f, 1.0f))
				{
					DEBUG_LOG_ERROR("Value %.2f at position %u exceeds its valid range, it must be in the range [0, 1]", fltValue, i);
					continue;
				}
				bytes[i] = static_cast<u8>(fltValue * 255);
			}
		}
		else
		{
			u32_pair_t pair = node->qualifiers[0];
			u32 index = strs_find_u32_pair(str, gColorMnemonics, SIZEOF_ARRAY(gColorMnemonics), pair);
			if(index != U32_MAX)
			{
				color = gColorPresets[index];
				if(node->is_block)
				{
					if((node->child_count == 1) && (node->childs[0]->qualifier_count == 1))
					{
						pair = node->childs[0]->qualifiers[0];
						f32 fltValue = u32_pair_to_f32(str, pair);
						if(!com::LiesBetweenInc(fltValue, 0.0f, 1.0f))
							DEBUG_LOG_ERROR("The scale value %.2f exceeds its valid range, it must be in the range [0, 1]", fltValue);
						else
						{
							color = Color4 
									{ 
										static_cast<u8>(color.r * fltValue), 
										static_cast<u8>(color.g * fltValue), 
										static_cast<u8>(color.b * fltValue), 
										static_cast<u8>(color.a * fltValue) 
									};
						}
					}
				}
			}
			else
				DEBUG_LOG_ERROR("Color Mnemonic \"%.*s\" doesn't match with any of recognized Color Presets", U32_PAIR_DIFF(pair), pair.start + str);
		}
		return color;
	}

	template<>
	template<>
	UIDriver::ImageReference ThemeManager<std::string, std::string_view>::deriveValue<UIDriver::ImageReference>(v3d_generic_node_t* node, const char* str) noexcept
	{
		// Followings are the possibilies:
		// 1. "path/to/a/file.png"
		// 2. default
		UIDriver::ImageReference image = UIDriver::InvalidImage;
		if(node->qualifier_count == 1)
		{
			u32_pair_t pair = node->qualifiers[0];
			if(com_safe_strncmp(pair.start + str, "default", U32_PAIR_DIFF(pair)) == 0)
			{
				// The Gfx Driver will automatically load its default texture
				image = UIDriver::InvalidImage;
			}
			else
			{
				auto filePath = std::string_view { pair.start + str, U32_PAIR_DIFF(pair) };
				image = getUIDriver().loadImage(filePath);
			}
		}
		else
			DEBUG_LOG_ERROR("Neither file path nor \"default\" is provided");
		return image;
	}

	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeType* ThemeManager<std::string, std::string_view>::loadTheme(const std::string_view filePath) noexcept
	{
		BUFFER* text = NULL;
		{
			std::string str { filePath };
			text = load_text_from_file(str.c_str());
		}
		char* str = reinterpret_cast<char*>(buf_get_ptr(text));
		ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(NULL, str, buf_get_element_count(text) - 1);
		ThemeType* theme = NULL;
		std::string_view strView { };
		v3d_generic_node_t* themeModelNode = NULL;
		if(result.result != PPSR_SUCCESS)
		{
			DEBUG_LOG_ERROR("Unable to parse the file at %.*s", filePath.length(), filePath.data());
			goto HANDLE_FAILURE;
		}
		if(result.root->child_count == 0)
		{
			DEBUG_LOG_ERROR("Parse is empty, perhaps the file was empty?");
			goto HANDLE_FAILURE;
		}

		themeModelNode = result.root->childs[0];
		if(themeModelNode->qualifier_count == 0)
		{
			DEBUG_LOG_ERROR("Anonymous root block is not allowed, it must be named \"ThemeModel\"");
			goto HANDLE_FAILURE;
		}

		{
			u32_pair_t name = themeModelNode->qualifiers[0];
			if(com_safe_strncmp(name.start + str, "Theme", U32_PAIR_DIFF(name)))
			{
				DEBUG_LOG_ERROR("Expected \"Theme\" but given \"%*.s\"", U32_PAIR_DIFF(name), name.start + str);
				goto HANDLE_FAILURE;
			}
		}

		{
			v3d_generic_attribute_t* nameAttr = node_find_attribute(themeModelNode, str, "Name");
			if(!nameAttr)
			{
				DEBUG_LOG_ERROR("[Name] attribute is expected on \"Theme\" block, but not given");
				goto HANDLE_FAILURE;
			}
			v3d_generic_attribute_t* modelAttr = node_find_attribute(themeModelNode, str, "Model");
			if(!modelAttr)
			{
				DEBUG_LOG_ERROR("[Model] attribute is expected on \"Theme\" block, but not given");
				goto HANDLE_FAILURE;
			}
			ThemeInterfaceType* interface = NULL;
			if(modelAttr->argument_count == 1)
			{
				auto themeInterfaceName = std::string_view { modelAttr->arguments[0].start + str, U32_PAIR_DIFF(modelAttr->arguments[0]) };
				if(!containsThemeInterface(themeInterfaceName))
				{
					DEBUG_LOG_ERROR("No Theme interface/model with name \"%.*s\" is found", themeInterfaceName.length(), themeInterfaceName.data());
					goto HANDLE_FAILURE;
				}
				interface = getThemeInterface(themeInterfaceName);
			}
			else
			{
				DEBUG_LOG_ERROR("Arguments mismatch, \"Model\" attribute accepts one argument (string), but either not given or more provided");
				goto HANDLE_FAILURE;
			}
			if(nameAttr->argument_count == 1)
			{
				strView = std::string_view { nameAttr->arguments[0].start + str, U32_PAIR_DIFF(nameAttr->arguments[0]) };
				theme = createTheme(strView, interface);
				for(u32 i = 0; i < themeModelNode->child_count; ++i)
				{
					v3d_generic_node_t* child = themeModelNode->childs[i];
					if(!child->qualifier_count)
					{
						DEBUG_LOG_ERROR("Anonymous (empty names) aren't allowed");
						goto HANDLE_FAILURE;
					}
					u32_pair_t name = child->qualifiers[0];
					std::string_view nameSV = std::string_view { name.start + str, U32_PAIR_DIFF(name) };
					if(!child->has_value)
					{
						DEBUG_LOG_ERROR("No value has been assigned to \"%.*s\"", nameSV.length(), nameSV.data());
						goto HANDLE_FAILURE;
					}
					ThemeInterfaceType::Type type = interface->getType(nameSV);
					switch(type)
					{
						case ThemeInterfaceType::Type::Color:
						{
							Color4 color = deriveValue<Color4>(child->value, str);
							theme->add<Color4>(nameSV, std::move(color));
							break;
						}
						case ThemeInterfaceType::Type::Image:
						{
							UIDriver::ImageReference image = deriveValue<UIDriver::ImageReference>(child->value, str);
							theme->add<UIDriver::ImageReference>(nameSV, std::move(image));
							break;
						}
						default:
						{
							DEBUG_LOG_WARNING("Type either isn't recognized or not given, skipping \"%.*s\"", nameSV.length(), nameSV.data());
						}
					}
				}
				ppsr_v3d_generic_parse_result_destroy(NULL, result);
				buf_free(text);
				return theme;
			}
			else
			{
				DEBUG_LOG_ERROR("Arguments mismatch, \"Name\" attribute accepts one argument (string), but either not given or more provided");
				goto HANDLE_FAILURE;
			}
		}

	HANDLE_FAILURE:
		if(theme)
			destroyTheme(strView);
		ppsr_v3d_generic_parse_result_destroy(NULL, result);
		buf_free(text);
		return NULL;
	}
}