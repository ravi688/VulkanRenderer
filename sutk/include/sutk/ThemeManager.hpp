#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp> // for UIDriverObject
#include <common/assert.h> // for _com_assert
#include <common/debug.h> // for DEBUG_LOG_ERROR, and debug_log_warning
#include <common/Event.hpp> // for com::Event<>
#include <common/Utility.hpp> // for com::BinaryFileLoadCallback, com::BinaryFileLoadResult and com::LoadBinaryFile

#include <string_view> // for std::string_view
#include <type_traits> // for std::is_same_v
#include <optional> // for std::optional<>

typedef struct v3d_generic_node_t v3d_generic_node_t;

namespace SUTK
{
	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class Theme;

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class SUTK_API ThemeInterface
	{
	public:
		enum class Type
		{
			Color,
			Image,
			Font,
			Float,
			String
		};
		typedef com::Event<com::no_publish_ptr_t> EventType;
		typedef EventType::SubscriptionID EventHandlerID;
		typedef EventHandlerID BindID;
		static constexpr BindID InvalidBindID = EventType::InvalidSubscriptionID;
		typedef com::Event<com::no_publish_ptr_t> OnThemeChangeEvent;
		template<typename ValueType>
		using ValueChangeCallback = std::function<void(const ValueType&)>;
	private:
		com::unordered_map<KeyType, std::pair<Type, EventType>, KeyViewType> m_defs;
		Theme<KeyType, KeyViewType>* m_currentTheme;
		OnThemeChangeEvent m_onThemeChange;
	public:
		ThemeInterface() noexcept;
		template<typename ValueType>
		void define(const KeyViewType& key) noexcept;
		void undefine(const KeyViewType& key) noexcept;
		std::optional<Type> getKeyType(const KeyViewType& key) noexcept;
		template<typename ValueType>
		BindID bind(const KeyViewType& key, ValueChangeCallback<ValueType> callback) noexcept;
		void unbind(const KeyViewType& key, BindID id) noexcept;
		template<typename ValueType>
		const com::OptionalReference<ValueType> getValue(Type type, const KeyViewType& key) noexcept;
		void applyTheme(Theme<KeyType, KeyViewType>* theme) noexcept;
		Theme<KeyType, KeyViewType>* getCurrentTheme() noexcept { return m_currentTheme; }
		com::Bool verifyKey(const KeyViewType& key, Type type) noexcept
		{
			auto value = try_find_value(m_defs, key);
			return com::Bool { value && (value->first == type) };
		}

		// Published after publishing events for every key
		OnThemeChangeEvent& getOnThemeChangeEvent() noexcept { return m_onThemeChange; }

		void dump() const noexcept;
	};

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class SUTK_API ThemeManager;

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class SUTK_API Theme : public UIDriverObject
	{
	public:
		using ThemeInterfaceType = ThemeInterface<KeyType, KeyViewType>;
		using ThemeInterfaceList = std::vector<ThemeInterfaceType*>;
		using Colors = com::unordered_map<KeyType, Color4, KeyViewType>;
		using Images = com::unordered_map<KeyType, UIDriver::ImageReference, KeyViewType>;
		using Fonts = com::unordered_map<KeyType, UIDriver::FontReference, KeyViewType>; 
		using Floats = com::unordered_map<KeyType, f32>;
		using Strings = com::unordered_map<KeyType, std::string>;
	private:
		Colors m_colors;
		Images m_images;
		Fonts m_fonts;
		Floats m_floats;
		Strings m_strings;
		ThemeInterfaceList m_interfaces;

		friend class ThemeManager<KeyType, KeyViewType>;

		const Colors& getColors() const noexcept { return m_colors; }
		const Images& getImages() const noexcept { return m_images; }
		const Fonts& getFonts() const noexcept { return m_fonts; }
		const Floats& getFloats() const noexcept { return m_floats; }
		const Strings& getStrings() const noexcept { return m_strings; }

	public:
		Theme(UIDriver& driver, std::vector<ThemeInterfaceType*>& interfaces) noexcept : UIDriverObject(driver), m_interfaces(interfaces) { }
		Theme(UIDriver& driver, std::vector<ThemeInterfaceType*>&& interfaces) noexcept : UIDriverObject(driver), m_interfaces(std::move(interfaces)) { }
		Theme(UIDriver& driver, ThemeInterfaceType* interface) noexcept : Theme(driver, std::vector { interface }) { }

		ThemeInterfaceList& getInterfaces() noexcept { return m_interfaces; }

		void dump() noexcept
		{
			std::cout << "theme dump: \n";
			std::cout << "\tcolors: \n";
			for(auto& pair : m_colors)
				std::cout << "\t\t" << pair.first << "\n";
			std::cout << "\timages: \n";
			for(auto& pair : m_images)
				std::cout << "\t\t" << pair.first << "\n";
			std::cout << "\tfonts: \n";
			for(auto& pair : m_fonts)
				std::cout << "\t\t" << pair.first << "\n";
			std::cout << "\tfloats: \n";
			for(auto& pair : m_floats)
				std::cout << "\t\t" << pair.first << "\n";
			std::cout << "\tstrings: \n";
			for(auto& pair : m_strings)
				std::cout << "\t\t" << pair.first << "\n";
		}

		void apply() noexcept
		{
			for(auto& interface : m_interfaces)
				interface->applyTheme(this);
		}

		com::Bool hasKey(const KeyViewType& key) noexcept
		{
			return com::Bool { m_colors.contains(key) || m_images.contains(key) || m_fonts.contains(key) || m_floats.contains(key) || m_strings.contains(key) };
		}

		com::Bool verifyKey(const KeyViewType& key, ThemeInterfaceType::Type type) noexcept
		{
			for(auto& interface : m_interfaces)
				if(interface->verifyKey(key, type))
					return com::True;
			DEBUG_LOG_ERROR("Either the key doesn't exist or type mismatch has been occurred");
			return com::False;
		}

		std::optional<typename ThemeInterfaceType::Type> getKeyType(const KeyViewType& key) noexcept
		{
			for(auto& interface : m_interfaces)
				if(auto value = interface->getKeyType(key))
					return value;
			return { };
		}

		template<typename ValueType>
		constexpr ThemeInterfaceType::Type getTypeEnum() noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
				return ThemeInterfaceType::Type::Color;
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
				return ThemeInterfaceType::Type::Image;
			else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
				return ThemeInterfaceType::Type::Font;
			else if constexpr(std::is_same_v<ValueType, f32>)
				return ThemeInterfaceType::Type::Float;
			else if constexpr(std::is_same_v<ValueType, std::string>)
				return ThemeInterfaceType::Type::String;
			else
				static_assert(false, "Type not supported");
		}

		template<typename ValueType>
		void add(const KeyViewType& key, ValueType value) noexcept
		{
			if(!verifyKey(key, getTypeEnum<ValueType>()))
			{
				DEBUG_LOG_ERROR("Failed to add value into the theme");
				return;
			}
			if constexpr(std::is_same_v<ValueType, Color4>)
					m_colors.insert({ KeyType { key }, std::move(value) });
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
					m_images.insert({ KeyType { key }, std::move(value) });
			else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
					m_fonts.insert({ KeyType { key }, std::move(value) });
			else if constexpr(std::is_same_v<ValueType, f32>)
					m_floats.insert({ KeyType { key }, std::move(value) });
			else if constexpr(std::is_same_v<ValueType, std::string>)
					m_strings.insert({ KeyType { key }, std::move(value) });
			else
				static_assert(false, "Type not supported");
		}
		template<typename ValueType>
		void addOrSet(const KeyViewType& key, ValueType value) noexcept
		{
			if(!verifyKey(key, getTypeEnum<ValueType>()))
			{
				DEBUG_LOG_ERROR("Failed to add value into the theme");
				return;
			}
			if constexpr(std::is_same_v<ValueType, Color4>)
					com::insert_or_set(m_colors, KeyType { key }, std::move(value));
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
					com::insert_or_set(m_images, KeyType { key }, std::move(value));
			else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
					com::insert_or_set(m_fonts, KeyType { key }, std::move(value));
			else if constexpr(std::is_same_v<ValueType, f32>)
					com::insert_or_set(m_floats, KeyType { key }, std::move(value));
			else if constexpr(std::is_same_v<ValueType, std::string>)
					com::insert_or_set(m_strings, KeyType { key }, std::move(value));
			else
				static_assert(false, "Type not supported");
		}
		template<typename ValueType> requires(com::SameAsAny<ValueType, UIDriver::ImageReference, UIDriver::FontReference>)
		void add(const KeyViewType& key, std::string_view view) noexcept
		{
			if(view.ends_with(".png") || view.ends_with(".jpg") || view.ends_with(".bmp"))
			{
				UIDriver::ImageReference image = getUIDriver().loadImage(view);
				m_images.insert({ KeyType { key }, image });
			} else if(view.ends_with(".ttf"))
			{
				UIDriver::FontReference font = getUIDriver().loadFont(view);
				m_fonts.insert({ KeyType { key }, font });
			}
			else
				DEBUG_LOG_ERROR("Unrecognized image file extension in path: %.*s", view.length(), view.data());
		}
		template<typename ValueType> requires(com::SameAsAny<ValueType, UIDriver::ImageReference, UIDriver::FontReference>)
		void addOrSet(const KeyViewType& key, std::string_view view) noexcept
		{
			if(view.ends_with(".png") || view.ends_with(".jpg") || view.ends_with(".bmp"))
			{
				UIDriver::ImageReference image = getUIDriver().loadImage(view);
				com::insert_or_set(m_images, KeyType { key }, image);
			} else if(view.ends_with(".ttf"))
			{
				UIDriver::FontReference font = getUIDriver().loadFont(view);
				com::insert_or_set(m_fonts, KeyType { key }, font);
			}
			else
				DEBUG_LOG_ERROR("Unrecognized image file extension in path: %.*s", view.length(), view.data());
		}
		template<typename ValueType>
		void remove(const KeyViewType& key) noexcept
		{
			bool result = false;
			if constexpr(std::is_same_v<ValueType, Color4>)
				result = com::find_erase(m_colors, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
				result = com::find_erase(m_images, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
				result = com::find_erase(m_fonts, key);
			else if constexpr(std::is_same_v<ValueType, f32>)
				result = com::find_erase(m_floats, key);
			else if constexpr(std::is_same_v<ValueType, std::string>)
				result = com::find_erase(m_strings, key);
			else
				static_assert(false, "Type not supported");
			_com_assert(result);
		}
		template<typename ValueType>
		const com::OptionalReference<ValueType> getValue(const KeyViewType& key) noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
				return com::try_find_value(m_colors, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
				return com::try_find_value(m_images, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
				return com::try_find_value(m_fonts, key);
			else if constexpr(std::is_same_v<ValueType, f32>)
				return com::try_find_value(m_floats, key);
			else if constexpr(std::is_same_v<ValueType, std::string>)
				return com::try_find_value(m_strings, key);
			else
				static_assert(false, "Type not supported");
			return { };
		}
	};

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	ThemeInterface<KeyType, KeyViewType>::ThemeInterface() noexcept : m_currentTheme(NULL) { }
	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	template<typename ValueType>
	void ThemeInterface<KeyType, KeyViewType>::define(const KeyViewType& key) noexcept
	{
		if constexpr(std::is_same_v<ValueType, Color4>)
			m_defs.insert({ KeyType { key }, { Type::Color, { } } });
		else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
			m_defs.insert({ KeyType { key }, { Type::Image, { } } });
		else if constexpr(std::is_same_v<ValueType, UIDriver::FontReference>)
			m_defs.insert({ KeyType { key }, { Type::Font, { } } });
		else if constexpr(std::is_same_v<ValueType, f32>)
			m_defs.insert({ KeyType { key }, { Type::Float, { } } });
		else if constexpr(std::is_same_v<ValueType, std::string>)
			m_defs.insert({ KeyType { key }, { Type::String, { } } });
		else
			static_assert(false, "Type not supported");
	}
	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::undefine(const KeyViewType& key) noexcept
	{
		bool result = com::find_erase(m_defs, key);
		_com_assert(result);
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	std::optional<typename ThemeInterface<KeyType, KeyViewType>::Type> ThemeInterface<KeyType, KeyViewType>::getKeyType(const KeyViewType& key) noexcept
	{
		auto value = com::try_find_value(m_defs, key);
		if(value)
			return { value->first };
		return { };
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	template<typename ValueType>
	ThemeInterface<KeyType, KeyViewType>::BindID ThemeInterface<KeyType, KeyViewType>::bind(const KeyViewType& key, ValueChangeCallback<ValueType> callback) noexcept
	{
		auto value = com::try_find_value(m_defs, key);
		if(value)
		{
			auto fn = [_callback = std::move(callback), key, this]()
			{
				// Only invoke the callback if the theme sets value for the key to which this callback binds to.
				auto value = this->m_currentTheme->template getValue<ValueType>(key);
				if(value)
					_callback(*value);
			};
			if(m_currentTheme)
				fn();
			return value->second.subscribe(std::move(fn));
		}
		else DEBUG_LOG_ERROR("Failed to bind, no such key exists");
		return ThemeInterface<KeyType, KeyViewType>::InvalidBindID;
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::unbind(const KeyViewType& key, BindID id) noexcept
	{
		auto value = com::try_find_value(m_defs, key);
		if(value)
			value->second.unsubscribe(id);
		else
			DEBUG_LOG_ERROR("Failed to unbind, no such key exists");
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	template<typename ValueType>
	const com::OptionalReference<ValueType> ThemeInterface<KeyType, KeyViewType>::getValue(Type type, const KeyViewType& key) noexcept
	{
		return m_currentTheme->template getValue<ValueType>(key);
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::applyTheme(Theme<KeyType, KeyViewType>* theme) noexcept
	{
		m_currentTheme = theme;
		for(auto& keyValuePair : m_defs)
			if(m_currentTheme->hasKey(keyValuePair.first))
				keyValuePair.second.second.publish();
		m_onThemeChange.publish();
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::dump() const noexcept
	{
		std::ostream& stream = std::cout;
		for(auto& keyValuePair : m_defs)
		{
			stream << "Key: " << keyValuePair.first << ", Value: ";
			switch(keyValuePair.second.first)
			{
				case Type::Color: stream << "Color"; break;
				case Type::Image: stream << "Image"; break;
				case Type::Font: stream << "Font"; break;
				case Type::Float: stream << "Float"; break;
				case Type::String: stream << "String"; break;
			}
			stream << "\n";
		}
		stream << std::endl;
	}

	// For now, the key type should be std::string_view
	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class SUTK_API ThemeManager : public UIDriverObject
	{
	public:
		using ThemeInterfaceType = ThemeInterface<KeyType, KeyViewType>;
		using ThemeType = Theme<KeyType, KeyViewType>;
		using ThemeInterfaceMap = com::unordered_map<KeyType, ThemeInterfaceType*, KeyViewType>;
		using ThemeMap = com::unordered_map<KeyType, ThemeType*, KeyViewType>;
		using ThemeInterfaceNameIterator = com::KeyIterator<typename ThemeInterfaceMap::const_iterator>;
		using ThemeInterfaceNameIteratable = com::Iteratable<ThemeInterfaceNameIterator>;
		using ThemeNameIterator = com::KeyIterator<typename ThemeMap::const_iterator>;
		using ThemeNameIteratable = com::Iteratable<ThemeNameIterator>;

	private:
		ThemeInterfaceMap m_themeInterfaces;
		ThemeMap m_themes;

		template<typename T>
		T deriveValue(v3d_generic_node_t* node, const char* str) noexcept;
		template<typename T>
		requires(com::SameAsAny<T, UIDriver::ImageReference, UIDriver::FontReference>)
		T deriveValue(v3d_generic_node_t* node, const char* str, std::optional<com::BinaryFileLoadCallback> fileLoadCallback = { }) noexcept;

	public:
		ThemeManager(UIDriver& driver) noexcept : UIDriverObject(driver) { }
		virtual ~ThemeManager() noexcept
		{
			for(auto& keyValuePair : m_themeInterfaces)
				delete keyValuePair.second;
			for(auto& keyValuePair : m_themes)
				delete keyValuePair.second;
		}

		ThemeInterfaceNameIteratable getThemeInterfaceNameIteratable() const noexcept
		{
			return { ThemeInterfaceNameIterator { m_themeInterfaces.cbegin() }, ThemeInterfaceNameIterator { m_themeInterfaces.cend() } };
		}

		ThemeNameIteratable getThemeNameIteratable() const noexcept
		{
			return { ThemeNameIterator { m_themes.cbegin() }, ThemeNameIterator { m_themes.cend() } };
		}

		const ThemeMap& getThemeMap() const noexcept { return m_themes; }

		ThemeInterfaceType* createThemeInterface(const KeyViewType& key) noexcept
		{
			_com_assert(!m_themeInterfaces.contains(key));
			ThemeInterfaceType* themeInterface = new ThemeInterfaceType();
			m_themeInterfaces.insert({ KeyType { key }, themeInterface });
			return themeInterface;
		}
		ThemeInterfaceType* loadThemeInterface(const std::string_view filePath) noexcept;
		// NOTE: themeIntefaceStr is different file path!
		// themeIntefaceStr is just an in-memory string
		ThemeInterfaceType* loadThemeInterfaceStr(const std::string_view themeIntefaceStr) noexcept;
		bool containsThemeInterface(const KeyViewType& key) const noexcept
		{
			return m_themeInterfaces.contains(key);
		}
		ThemeInterfaceType* getThemeInterface(const KeyViewType& key) noexcept
		{
			auto value = com::try_find_value(m_themeInterfaces, key);
			return value ? *value : com::null_pointer<ThemeInterfaceType>();
		}
		void destroyThemeInterface(const KeyViewType& key) noexcept
		{
			bool result = com::find_erase(m_themeInterfaces, key);
			_com_assert(result);
		}

		ThemeType* createTheme(const KeyViewType& key, ThemeInterfaceType* interface) noexcept
		{
			return createTheme(key, std::vector { interface });
		}
		template<typename InitVectorType> requires(com::SameAsAny<InitVectorType, std::vector<ThemeInterfaceType*>, std::vector<ThemeInterfaceType*>&>)
		ThemeType* createTheme(const KeyViewType& key, InitVectorType&& interfaces) noexcept
		{
			_com_assert(!m_themes.contains(key));
			ThemeType* theme = new ThemeType(getUIDriver(), std::forward<InitVectorType&&>(interfaces));
			m_themes.insert({ KeyType { key }, theme });
			return theme;
		}
		// The reason we don't provide a default value for file load callback here is that if we provide 'LoadBinaryFile' then it will be immediately
		// called for each file path in the theme file. But we want to allow the IGfxDriver's implementa to lazily load the imagse and fonts.
		ThemeType* loadTheme(const std::string_view filePath, std::optional<com::BinaryFileLoadCallback> fileLoadCallback = { }) noexcept;
		ThemeType* loadThemeStr(const std::string_view themeStr, std::optional<com::BinaryFileLoadCallback> fileLoadCallback = { }) noexcept;
		ThemeType* getTheme(const KeyViewType& key) noexcept
		{
			auto value = com::try_find_value(m_themes, key);
			return value ? *value : com::null_pointer<ThemeType>();
		}
		void destroyTheme(const KeyViewType& key) noexcept
		{
			bool result = com::find_erase(m_themes, key);
			_com_assert(result);
		}

		void dumpThemes() noexcept
		{
			std::cout << "Dump Theme Start: \n";
			for(auto& pair : m_themes)
				std::cout << "Theme: " << pair.first << "\n";
			std::cout << "Dump Theme End" << std::endl;
		}
	};
}