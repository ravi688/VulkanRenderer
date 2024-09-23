#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp> // for UIDriverObject
#include <common/assert.h> // for _com_assert
#include <common/debug.h> // for DEBUG_LOG_ERROR
#include <common/Event.hpp> // for com::Event<>

#include <string_view> // for std::string_view
#include <type_traits> // for std::is_same_v
#include <unordered_map> // for std::unordered_map

namespace SUTK
{
	template<typename KeyType>
	class Theme;

	template<typename KeyType>
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
	private:
		std::unordered_map<KeyType, std::pair<Type, EventType>> m_defs;
		Theme<KeyType>* m_currentTheme;
	public:
		ThemeInterface() noexcept;
		template<typename ValueType>
		void define(const KeyType& key) noexcept;
		void undefine(const KeyType& key) noexcept;
		Type getType(const KeyType& key) noexcept;
		template<typename ValueType>
		EventHandlerID bind(const KeyType& key, std::function<void(ValueType&)> callback) noexcept;
		void unbind(const KeyType& key, EventHandlerID id) noexcept;
		template<typename ValueType>
		ValueType& getValue(Type type, const KeyType& key) noexcept;
		void applyTheme(Theme<KeyType>* theme) noexcept;
	};

	template<typename KeyType>
	class SUTK_API Theme : public UIDriverObject
	{
	private:
		std::unordered_map<KeyType, Color4> m_colors;
		std::unordered_map<KeyType, UIDriver::ImageReference> m_images;
		// std::unordered_map<KeyType, UIDriver::FontReference> m_fonts;
		ThemeInterface<KeyType>* m_interface;
	public:
		Theme(UIDriver& driver, ThemeInterface<KeyType>* interface) noexcept : UIDriverObject(driver), m_interface(interface) { }
		template<typename ValueType>
		void add(const KeyType& key, ValueType&& value) noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
			{
				if(m_interface->getType(key) != ThemeInterface<KeyType>::Type::Color)
					DEBUG_LOG_ERROR("Type mismatch");
				else
					m_colors.insert({ key, std::forward<Color4&&>(value) });
			}
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
			{
				if(m_interface->getType(key) != ThemeInterface<KeyType>::Type::Image)
					DEBUG_LOG_ERROR("Type mismatch");
				else
					m_images.insert({ key, std::forward<UIDriver::ImageReference&&>(value) });
			}
			else
				static_assert(false, "Type not supported");
		}
		template<typename ValueType>
		void add(const KeyType& key, std::string_view view) noexcept
		{
			bool result = view.ends_with(".png") || view.ends_with(".jpg") || view.ends_with(".bmp");
			if(result)
			{
				UIDriver::ImageReference image = getUIDriver().loadImage(view);
				m_images.insert({ key, image });
			}
			else
				DEBUG_LOG_ERROR("Unrecognized image file extension in path: %.*s", view.length(), view.data());
		}
		template<typename ValueType>
		void remove(const KeyType& key) noexcept
		{
			bool result = false;
			if(std::is_same_v<ValueType, Color4>)
				result = com::find_erase(m_colors, key);
			else if(std::is_same_v<ValueType, UIDriver::ImageReference>)
				result = com::find_erase(m_images, key);
			else
				static_assert(false, "Type not supported");
			_com_assert(result);
		}
		template<typename ValueType>
		ValueType& getValue(const KeyType& key) noexcept
		{
			if constexpr(std::is_same_v<ValueType, Color4>)
				return com::find_value(m_colors, key);
			else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
				return com::find_value(m_images, key);
			else
				static_assert(false, "Type not supported");
		}
	};

	template<typename KeyType>
	ThemeInterface<KeyType>::ThemeInterface() noexcept : m_currentTheme(NULL) { }
	template<typename KeyType>
	template<typename ValueType>
	void ThemeInterface<KeyType>::define(const KeyType& key) noexcept
	{
		if constexpr(std::is_same_v<ValueType, Color4>)
			m_defs.insert({ key, { Type::Color, { } } });
		else if constexpr(std::is_same_v<ValueType, UIDriver::ImageReference>)
			m_defs.insert({ key, { Type::Image, { } } });
		else
			static_assert(false, "Type not supported");
	}
	template<typename KeyType>
	void ThemeInterface<KeyType>::undefine(const KeyType& key) noexcept
	{
		bool result = com::find_erase(m_defs, key);
		_com_assert(result);
	}

	template<typename KeyType>
	ThemeInterface<KeyType>::Type ThemeInterface<KeyType>::getType(const KeyType& key) noexcept
	{
		return com::find_value(m_defs, key).first;
	}

	template<typename KeyType>
	template<typename ValueType>
	ThemeInterface<KeyType>::EventHandlerID ThemeInterface<KeyType>::bind(const KeyType& key, std::function<void(ValueType&)> callback) noexcept
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

	template<typename KeyType>
	void ThemeInterface<KeyType>::unbind(const KeyType& key, EventHandlerID id) noexcept
	{
		auto& value = com::find_value(m_defs, key);
		value.second.unsubscribe(id);
	}

	template<typename KeyType>
	template<typename ValueType>
	ValueType& ThemeInterface<KeyType>::getValue(Type type, const KeyType& key) noexcept
	{
		return m_currentTheme->template getValue<ValueType>(key);
	}

	template<typename KeyType>
	void ThemeInterface<KeyType>::applyTheme(Theme<KeyType>* theme) noexcept
	{
		m_currentTheme = theme;
		for(auto& keyValuePair : m_defs)
			keyValuePair.second.second.publish();
	}

	template<typename KeyType>
	concept StringViewKeyType = std::is_same_v<std::string_view, KeyType>;

	// For now, the key type should be std::string_view
	template<StringViewKeyType KeyType>
	class SUTK_API ThemeManager : public UIDriverObject
	{
	public:
		using ThemeInterfaceType = ThemeInterface<KeyType>;
		using ThemeType = Theme<KeyType>;
	private:
		std::unordered_map<KeyType, ThemeInterfaceType*> m_themeInterfaces;
		std::unordered_map<KeyType, ThemeType*> m_themes;
	public:
		ThemeManager(UIDriver& driver) noexcept : UIDriverObject(driver) { }
		~ThemeManager() noexcept
		{
			for(auto& keyValuePair : m_themeInterfaces)
				delete keyValuePair.second;
			for(auto& keyValuePair : m_themes)
				delete keyValuePair.second;
		}

		ThemeInterfaceType* createThemeInterface(const KeyType& key) noexcept
		{
			_com_assert(!m_themeInterfaces.contains(key));
			ThemeInterfaceType* themeInterface = new ThemeInterfaceType();
			m_themeInterfaces.insert({ key, themeInterface });
			return themeInterface;
		}
		ThemeInterfaceType* loadThemeInterface(const std::string_view filePath) noexcept
		{
			return NULL;
		}
		ThemeInterfaceType* getThemeInterface(const KeyType& key) noexcept
		{
			return com::find_value(m_themeInterfaces, key);
		}
		void destroyThemeInterface(const KeyType& key) noexcept
		{
			bool result = com::find_erase(m_themeInterfaces, key);
			_com_assert(result);
		}

		ThemeType* createTheme(const KeyType& key, ThemeInterfaceType* interface) noexcept
		{
			_com_assert(!m_themes.contains(key));
			ThemeType* theme = new ThemeType(getUIDriver(), interface);
			m_themes.insert({ key, theme });
			return theme;
		}
		ThemeType* loadTheme(const std::string_view filePath) noexcept
		{
			return NULL;
		}
		ThemeType* getTheme(const KeyType& key) noexcept
		{
			return com::find_value(m_themes, key);
		}
		void destroyTheme(const KeyType& key) noexcept
		{
			bool result = com::find_erase(m_themes, key);
			_com_assert(result);
		}
	};
}