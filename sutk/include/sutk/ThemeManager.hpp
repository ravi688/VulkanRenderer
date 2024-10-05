#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp> // for UIDriverObject
#include <common/assert.h> // for _com_assert
#include <common/debug.h> // for DEBUG_LOG_ERROR, and debug_log_warning
#include <common/Event.hpp> // for com::Event<>

#include <string_view> // for std::string_view
#include <type_traits> // for std::is_same_v

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
			Font
		};
		typedef com::Event<com::no_publish_ptr_t> EventType;
		typedef EventType::SubscriptionID EventHandlerID;
		typedef EventHandlerID BindID;
		static constexpr BindID InvalidBindID = EventType::InvalidSubscriptionID;
		typedef com::Event<com::no_publish_ptr_t> OnThemeChangeEvent;
	private:
		com::unordered_map<KeyType, std::pair<Type, EventType>, KeyViewType> m_defs;
		Theme<KeyType, KeyViewType>* m_currentTheme;
		OnThemeChangeEvent m_onThemeChange;
	public:
		ThemeInterface() noexcept;
		template<typename ValueType>
		void define(const KeyViewType& key) noexcept;
		void undefine(const KeyViewType& key) noexcept;
		Type getType(const KeyViewType& key) noexcept;
		template<typename ValueType>
		EventHandlerID bind(const KeyViewType& key, std::function<void(ValueType&)> callback) noexcept;
		void unbind(const KeyViewType& key, EventHandlerID id) noexcept;
		template<typename ValueType>
		ValueType& getValue(Type type, const KeyViewType& key) noexcept;
		void applyTheme(Theme<KeyType, KeyViewType>* theme) noexcept;
		Theme<KeyType, KeyViewType>* getCurrentTheme() noexcept { return m_currentTheme; }

		// Published after publishing events for every key
		OnThemeChangeEvent& getOnThemeChangeEvent() noexcept { return m_onThemeChange; }

		void dump() const noexcept;
	};

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	class SUTK_API Theme : public UIDriverObject
	{
	public:
		using ThemeInterfaceType = ThemeInterface<KeyType, KeyViewType>;
	private:
		com::unordered_map<KeyType, Color4, KeyViewType> m_colors;
		com::unordered_map<KeyType, UIDriver::ImageReference, KeyViewType> m_images;
		// com::unordered_map<KeyType, UIDriver::FontReference, KeyViewType> m_fonts;
		ThemeInterfaceType* m_interface;
	public:
		Theme(UIDriver& driver, ThemeInterfaceType* interface) noexcept : UIDriverObject(driver), m_interface(interface) { }

		ThemeInterfaceType* getInterface() noexcept { return m_interface; }

		template<typename ValueType>
		void add(const KeyViewType& key, ValueType&& value) noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
			{
				if(m_interface->getType(key) != ThemeInterfaceType::Type::Color)
					DEBUG_LOG_ERROR("Type mismatch");
				else
					m_colors.insert({ KeyType { key }, std::forward<Color4&&>(value) });
			}
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
			{
				if(m_interface->getType(key) != ThemeInterfaceType::Type::Image)
					DEBUG_LOG_ERROR("Type mismatch");
				else
					m_images.insert({ KeyType { key }, std::forward<UIDriver::ImageReference&&>(value) });
			}
			else
				static_assert(false, "Type not supported");
		}
		template<typename ValueType>
		void add(const KeyViewType& key, std::string_view view) noexcept
		{
			bool result = view.ends_with(".png") || view.ends_with(".jpg") || view.ends_with(".bmp");
			if(result)
			{
				UIDriver::ImageReference image = getUIDriver().loadImage(view);
				m_images.insert({ KeyType { key }, image });
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
			else
				static_assert(false, "Type not supported");
			_com_assert(result);
		}
		template<typename ValueType>
		ValueType& getValue(const KeyViewType& key) noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
				return com::find_value(m_colors, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
				return com::find_value(m_images, key);
			else
				static_assert(false, "Type not supported");
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
	ThemeInterface<KeyType, KeyViewType>::Type ThemeInterface<KeyType, KeyViewType>::getType(const KeyViewType& key) noexcept
	{
		return com::find_value(m_defs, key).first;
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	template<typename ValueType>
	ThemeInterface<KeyType, KeyViewType>::EventHandlerID ThemeInterface<KeyType, KeyViewType>::bind(const KeyViewType& key, std::function<void(ValueType&)> callback) noexcept
	{
		auto& value = com::find_value(m_defs, key);
		auto fn = [_callback = std::move(callback), key, this]()
		{
			_callback(this->m_currentTheme->template getValue<ValueType>(key));
		};
		if(m_currentTheme)
			fn();
		return value.second.subscribe(std::move(fn));
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::unbind(const KeyViewType& key, EventHandlerID id) noexcept
	{
		auto& value = com::find_value(m_defs, key);
		value.second.unsubscribe(id);
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	template<typename ValueType>
	ValueType& ThemeInterface<KeyType, KeyViewType>::getValue(Type type, const KeyViewType& key) noexcept
	{
		return m_currentTheme->template getValue<ValueType>(key);
	}

	template<typename KeyType, com::ViewType<KeyType> KeyViewType>
	void ThemeInterface<KeyType, KeyViewType>::applyTheme(Theme<KeyType, KeyViewType>* theme) noexcept
	{
		if(m_currentTheme == theme)
		{
			debug_log_warning("You're trying to apply the same theme again, ignored");
			return;
		}
		m_currentTheme = theme;
		for(auto& keyValuePair : m_defs)
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

	public:
		ThemeManager(UIDriver& driver) noexcept : UIDriverObject(driver) { }
		~ThemeManager() noexcept
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
		bool containsThemeInterface(const KeyViewType& key) const noexcept
		{
			return m_themeInterfaces.contains(key);
		}
		ThemeInterfaceType* getThemeInterface(const KeyViewType& key) noexcept
		{
			return com::find_value(m_themeInterfaces, key);
		}
		void destroyThemeInterface(const KeyViewType& key) noexcept
		{
			bool result = com::find_erase(m_themeInterfaces, key);
			_com_assert(result);
		}

		ThemeType* createTheme(const KeyViewType& key, ThemeInterfaceType* interface) noexcept
		{
			_com_assert(!m_themes.contains(key));
			ThemeType* theme = new ThemeType(getUIDriver(), interface);
			m_themes.insert({ KeyType { key }, theme });
			return theme;
		}
		ThemeType* loadTheme(const std::string_view filePath) noexcept;
		ThemeType* getTheme(const KeyViewType& key) noexcept
		{
			return com::find_value(m_themes, key);
		}
		void destroyTheme(const KeyViewType& key) noexcept
		{
			bool result = com::find_erase(m_themes, key);
			_com_assert(result);
		}
	};
}