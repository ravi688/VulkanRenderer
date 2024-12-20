#include <sutk/ThemeManager.hpp>

#include <common/string.h> // for com_safe_strncmp()
#include <common/debug.h> // for DEBUG_LOG_ERROR()
#include <common/utility.h> // for com_verify_float_str()
#include <disk_manager/file_reader.h> // for load_text_from_file()
#include <phymac_parser/v3d_generic.h> // for ppsr_v3d_generic_parse()

namespace SUTK
{
	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeInterfaceType* ThemeManager<std::string, std::string_view>::loadThemeInterfaceStr(const std::string_view themeInterfaceStr) noexcept
	{
		const char* str = themeInterfaceStr.data();
		ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(NULL, themeInterfaceStr.data(), themeInterfaceStr.length());
		ThemeInterfaceType* interface = NULL;
		std::string_view strView { };
		v3d_generic_node_t* themeModelNode = NULL;
		if(result.result != PPSR_SUCCESS)
			goto HANDLE_FAILURE;
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
					typeAttr = node_find_attribute(child, str, "Font");
					if(typeAttr)
					{
						interface->define<UIDriver::FontReference>(nameSV);
						continue;
					}
					typeAttr = node_find_attribute(child, str, "Float");
					if(typeAttr)
					{
						interface->define<f32>(nameSV);
						continue;
					}
					typeAttr = node_find_attribute(child, str, "String");
					if(typeAttr)
					{
						interface->define<std::string>(nameSV);
						continue;
					}
					DEBUG_LOG_WARNING("Type either isn't recognized or not given, skipping \"%.*s\"", nameSV.length(), nameSV.data());
				}
				ppsr_v3d_generic_parse_result_destroy(NULL, result);
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
		return NULL;
	}

	// NOTE: specialization must always come before instantiation otherwise we will get 'specialization after instantiation error'
	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeInterfaceType* ThemeManager<std::string, std::string_view>::loadThemeInterface(const std::string_view filePath) noexcept
	{
		BUFFER* text = NULL;
		{
			std::string str { filePath };
			text = load_text_from_file(str.c_str());
		}
		char* str = reinterpret_cast<char*>(buf_get_ptr(text));
		auto* themeInterface = loadThemeInterfaceStr(std::string_view  { str, strlen(str) });
		if(!themeInterface)
			DEBUG_LOG_ERROR("Unable to parse the file at %.*s", filePath.length(), filePath.data());
		buf_free(text);
		return themeInterface;
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
		char buf[U32_PAIR_DIFF(pair) + 1];
		memcpy(buf, pair.start + str, U32_PAIR_DIFF(pair));
		buf[U32_PAIR_DIFF(pair)] = 0;
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

	static com::BinaryFileLoadResult InvokeFileLoadCallbackDBG(const std::string_view filePath, com::BinaryFileLoadCallback loadHandler) noexcept
	{
		com::BinaryFileLoadResult result = loadHandler(filePath);
		if(!result)
			DEBUG_LOG_ERROR("Failed to load file \"%*.s\"", filePath.length(), filePath.data());
		return result;
	}

	template<>
	template<>
	UIDriver::ImageReference ThemeManager<std::string, std::string_view>::deriveValue<UIDriver::ImageReference>(v3d_generic_node_t* node, const char* str, std::optional<com::BinaryFileLoadCallback> fileLoadCallback) noexcept
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
				if(fileLoadCallback)
				{
					if(com::BinaryFileLoadResult result = InvokeFileLoadCallbackDBG(filePath, *fileLoadCallback))
					{
						image = getUIDriver().loadImage(std::move(result));
						result.destroy();
					}
				}
				else image = getUIDriver().loadImage(filePath);
			}
		}
		else
			DEBUG_LOG_ERROR("Neither file path nor \"default\" is provided");
		return image;
	}

	template<>
	template<>
	UIDriver::FontReference ThemeManager<std::string, std::string_view>::deriveValue<UIDriver::FontReference>(v3d_generic_node_t* node, const char* str, std::optional<com::BinaryFileLoadCallback> fileLoadCallback) noexcept
	{
		// Followings are the possibilies:
		// 1. "path/to/a/file.ttf"
		// 2. default
		UIDriver::FontReference font = UIDriver::InvalidFont;
		if(node->qualifier_count == 1)
		{
			u32_pair_t pair = node->qualifiers[0];
			if(com_safe_strncmp(pair.start + str, "default", U32_PAIR_DIFF(pair)) == 0)
			{
				// The Gfx Driver will automatically load its default font
				font = UIDriver::InvalidFont;
			}
			else
			{
				auto filePath = std::string_view { pair.start + str, U32_PAIR_DIFF(pair) };
				if(fileLoadCallback)
				{
					if(com::BinaryFileLoadResult result = InvokeFileLoadCallbackDBG(filePath, *fileLoadCallback))
					{
						font = getUIDriver().loadFont(std::move(result));
						result.destroy();
					}
				}
				else font = getUIDriver().loadFont(filePath);
			}
		}
		else
			DEBUG_LOG_ERROR("Neither file path nor \"default\" is provided");
		return font;
	}

	template<>
	template<>
	f32 ThemeManager<std::string, std::string_view>::deriveValue<f32>(v3d_generic_node_t* node, const char* str) noexcept
	{
		// Followings are the possibilies:
		// 1. 2.4f
		// 2. 2.4
		// 2. 24
		f32 flt = 0;
		if(node->qualifier_count == 1)
		{
			u32_pair_t pair = node->qualifiers[0];
			auto len = U32_PAIR_DIFF(pair);
			char buffer[len + 1];
			memcpy(buffer, pair.start + str, len);
			buffer[len] = 0;
			if(!com_verify_float_str(buffer))
			{
				DEBUG_LOG_ERROR("Float value %*.s seems invalid, using 0 as default", len, pair.start + str);
				flt = 0;
			}
			else
				flt = std::strtof(buffer, NULL);
		}
		else
			DEBUG_LOG_ERROR("No value is provided");
		return flt;
	}

	template<>
	template<>
	std::string ThemeManager<std::string, std::string_view>::deriveValue<std::string>(v3d_generic_node_t* node, const char* str) noexcept
	{
		// Followings are the possibilies:
		// 1. "some string here"
		// 2. ""
		if(node->qualifier_count == 1)
		{
			u32_pair_t pair = node->qualifiers[0];
			return std::string { std::string_view { pair.start + str, U32_PAIR_DIFF(pair) } };
		}
		else
			DEBUG_LOG_ERROR("No value is provided");
		return std::string { };
	}

	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeType* ThemeManager<std::string, std::string_view>::loadThemeStr(const std::string_view themeStr, std::optional<com::BinaryFileLoadCallback> fileLoadCallback) noexcept
	{
		const char* str = themeStr.data();
		ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(NULL, themeStr.data(), themeStr.length());
		ThemeType* theme = NULL;
		std::string_view strView { };
		v3d_generic_node_t* themeNode = NULL;
		if(result.result != PPSR_SUCCESS)
			goto HANDLE_FAILURE;
		if(result.root->child_count == 0)
		{
			DEBUG_LOG_ERROR("Parse is empty, perhaps the file was empty?");
			goto HANDLE_FAILURE;
		}

		themeNode = result.root->childs[0];
		if(themeNode->qualifier_count == 0)
		{
			DEBUG_LOG_ERROR("Anonymous root block is not allowed, it must be named \"Theme\"");
			goto HANDLE_FAILURE;
		}

		{
			u32_pair_t name = themeNode->qualifiers[0];
			if(com_safe_strncmp(name.start + str, "Theme", U32_PAIR_DIFF(name)))
			{
				DEBUG_LOG_ERROR("Expected \"Theme\" but given \"%*.s\"", U32_PAIR_DIFF(name), name.start + str);
				goto HANDLE_FAILURE;
			}
		}

		{
			v3d_generic_attribute_t* nameAttr = node_find_attribute(themeNode, str, "Name");
			if(!nameAttr)
			{
				DEBUG_LOG_ERROR("[Name] attribute is expected on \"Theme\" block, but not given");
				goto HANDLE_FAILURE;
			}
			std::vector<ThemeInterfaceType*> interfaces;
			using UserData = struct { std::vector<ThemeInterfaceType*>* interfaces; ThemeManager<std::string, std::string_view>* thisPtr; };
			UserData userData = { &interfaces, this };
			node_foreach_attribute_name(themeNode, str, "Model", [](v3d_generic_attribute_t* modelAttr, const char* start, void* user_data) noexcept -> bool
			{
				auto* ud = static_cast<UserData*>(user_data);
				if(modelAttr->argument_count == 1)
				{
					auto themeInterfaceName = std::string_view { modelAttr->arguments[0].start + start, U32_PAIR_DIFF(modelAttr->arguments[0]) };
					if(ud->thisPtr->containsThemeInterface(themeInterfaceName))
						ud->interfaces->push_back(ud->thisPtr->getThemeInterface(themeInterfaceName));
					else
						DEBUG_LOG_ERROR("No Theme interface/model with name \"%.*s\" is found", themeInterfaceName.length(), themeInterfaceName.data());
				}
				else
					DEBUG_LOG_ERROR("Arguments mismatch, \"Model\" attribute accepts one argument (string), but either not given or more provided");
				return true;
			}, &userData);
			if(nameAttr->argument_count == 1)
			{
				v3d_generic_attribute_t* baseAttr = node_find_attribute(themeNode, str, "Base");
				ThemeType* baseTheme = com::null_pointer<ThemeType>();
				if(baseAttr)
				{
					auto baseThemeName = std::string_view { baseAttr->arguments[0].start + str, U32_PAIR_DIFF(baseAttr->arguments[0]) };
					dumpThemes();
					std::cout << "Base Theme: " << baseThemeName << std::endl;
					if((baseTheme = getTheme(baseThemeName)))
					{
						auto& baseInterfaces = baseTheme->getInterfaces();
						for(auto& baseInterface : baseInterfaces)
							interfaces.push_back(baseInterface);
					}
					else DEBUG_LOG_ERROR("The theme derives from a non-existent theme %*.s as base theme", baseThemeName.length(), baseThemeName.data());
				}
				else if(!interfaces.size())
				{
					DEBUG_LOG_ERROR("Either [Model] attribute is expected on \"Theme\" block or it must be derived from another Theme, using [Base] attribute, which implements a Theme Model");
					goto HANDLE_FAILURE;
				}
				strView = std::string_view { nameAttr->arguments[0].start + str, U32_PAIR_DIFF(nameAttr->arguments[0]) };
				theme = createTheme(strView, interfaces);
				if(baseTheme)
				{
					for(auto& keyValuePair : baseTheme->getColors())
						theme->add<Color4>(keyValuePair.first, keyValuePair.second);
					for(auto& keyValuePair : baseTheme->getImages())
						theme->add<UIDriver::ImageReference>(keyValuePair.first, keyValuePair.second);
					for(auto& keyValuePair : baseTheme->getFonts())
						theme->add<UIDriver::FontReference>(keyValuePair.first, keyValuePair.second);
					for(auto& keyValuePair : baseTheme->getFloats())
						theme->add<f32>(keyValuePair.first, keyValuePair.second);
					for(auto& keyValuePair : baseTheme->getStrings())
						theme->add<std::string>(keyValuePair.first, keyValuePair.second);
				}
				for(u32 i = 0; i < themeNode->child_count; ++i)
				{
					v3d_generic_node_t* child = themeNode->childs[i];
					if(!child->qualifier_count)
					{
						DEBUG_LOG_ERROR("Anonymous (empty names) aren't allowed");
						goto HANDLE_FAILURE;
					}
					u32_pair_t name = child->qualifiers[0];
					std::string_view nameSV = std::string_view { name.start + str, U32_PAIR_DIFF(name) };
					if(!child->has_value)
					{
						DEBUG_LOG_ERROR("No value has been assigned to \"%.*s\", skipped", nameSV.length(), nameSV.data());
						continue;
					}
					std::optional<ThemeInterfaceType::Type> type = theme->getKeyType(nameSV);
					if(!type)
						DEBUG_LOG_WARNING("Couldn't find a key into any of the theme models which the theme implements, skipping \"%.*s\"", nameSV.length(), nameSV.data());
					else
					{
						switch(*type)
						{
							case ThemeInterfaceType::Type::Color:
							{
								Color4 color = deriveValue<Color4>(child->value, str);
								theme->addOrSet<Color4>(nameSV, std::move(color));
								break;
							}
							case ThemeInterfaceType::Type::Image:
							{
								UIDriver::ImageReference image = deriveValue<UIDriver::ImageReference>(child->value, str, fileLoadCallback);
								theme->addOrSet<UIDriver::ImageReference>(nameSV, std::move(image));
								break;
							}
							case ThemeInterfaceType::Type::Font:
							{
								UIDriver::FontReference font = deriveValue<UIDriver::FontReference>(child->value, str, fileLoadCallback);
								theme->addOrSet<UIDriver::FontReference>(nameSV, std::move(font));
								break;
							}
							case ThemeInterfaceType::Type::Float:
							{
								f32 flt = deriveValue<f32>(child->value, str);
								theme->addOrSet<f32>(nameSV, std::move(flt));
								break;
							}
							case ThemeInterfaceType::Type::String:
							{
								std::string stdstr = deriveValue<std::string>(child->value, str);
								theme->addOrSet<std::string>(nameSV, std::move(stdstr));
								break;
							}
						}
					}
				}
				ppsr_v3d_generic_parse_result_destroy(NULL, result);
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
		return NULL;
	}

	// NOTE: We can't put this function 'loadTheme' before 'loadThemeStr', otherwise we will get 'specialization after instantiation error'
	template<>
	typename ThemeManager<std::string, std::string_view>::ThemeType* ThemeManager<std::string, std::string_view>::loadTheme(const std::string_view filePath, std::optional<com::BinaryFileLoadCallback> fileLoadCallback) noexcept
	{
		BUFFER* text = NULL;
		{
			std::string str { filePath };
			text = load_text_from_file(str.c_str());
		}
		char* str = reinterpret_cast<char*>(buf_get_ptr(text));
		auto* theme = loadThemeStr(std::string_view { str, strlen(str) }, fileLoadCallback);
		if(!theme)
			DEBUG_LOG_ERROR("Unable to parse the file at %.*s", filePath.length(), filePath.data());
		buf_free(text);
		return theme;
	}
}