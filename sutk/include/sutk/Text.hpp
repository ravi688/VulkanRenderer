#pragma once

#include <sutk/defines.hpp>
#include <sutk/IRenderable.hpp> /* for SUTK::IRenderable */
#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */

#include <string> /* for std::string */
#include <vector> /* for std::vector*/

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

	class TextContainer;
	class UIDriver;
	class IGfxDriver;

	class ColumnIterator { };

	class LineTextData
	{
	private:
		std::string m_data;
	public:
		// constructors
		LineTextData() = default;
		LineTextData(LineTextData&& data) : m_data(std::move(data.m_data)) { }
		LineTextData(const LineTextData& data) : m_data(data.m_data) { }

		// assignment operators
		LineTextData& operator =(LineTextData&& data) { m_data = std::move(data.m_data); return *this; }
		LineTextData& operator =(const LineTextData& data) { m_data = data.m_data; return *this; }
		LineTextData& operator =(const std::string& data) { m_data = data; return *this; }
		LineTextData& operator =(std::string&& data) { m_data = std::move(data); return *this; }
	
		// implict conversion operators
		operator const std::string&() const { return m_data; }

		// equality comparison operators
		operator ==(const LineTextData& data) { return m_data == data.m_data; }
		operator ==(const std::string& data) { return m_data == data; }

		// concatenation operator
		LineTextData& operator +=(const LineTextData& data) { m_data += data.m_data; return *this; }
		LineTextData& operator +=(const std::string& data) { m_data += data; return *this; }

		ColumnIterator begin() { return ColumnIterator(); }
		ColumnIterator end() { return ColumnIterator(); }
	
		void clear() { m_data.clear(); }
		void append(const std::string& chars, std::string::size_type index = 0) { insert(U32_MAX, chars, index); }
		void insert(u32 col, const std::string& chars, std::string::size_type index = 0)
		{ 
			if(col == U32_MAX)
			{
				if(index == 0)
					m_data += chars;
				else
					m_data += chars.substr(index);
			}
			else
				m_data.insert(col, chars, index); 
		}
	};

	class LineText : public IRenderable, public UIDriverObject
	{
	private:
		GfxDriverObjectHandleType m_handle;
		bool m_isPosDirty;
		bool m_isDataDirty;
		LineTextData m_data;
		Vec2D<DisplaySizeType> m_pos;

		LineText(UIDriver& driver) noexcept;

		friend class Text;

	public:
		virtual bool isDirty() override;
		virtual void update() override;
		void setData(const std::string& data) noexcept;
		void append(const std::string& data) noexcept;
		void setPosition(Vec2D<DisplaySizeType> pos) noexcept;
		void addDisplacement(Vec2D<DisplaySizeType> pos) noexcept;
		void clear() noexcept;
	};

	enum class HorizontalAlignment : u8
	{
		Left,
		Right,
		Middle
	};

	enum class VerticalAlignment : u8
	{
		Top,
		Bottom,
		Middle
	};
	
	// One text object is responsible for rendering a small to medium sized sub-text
	// This usually translates to a single Gfx API specific buffer object. For example, it is VkBuffer (and VkDeviceMemory) in vulkan.
	// This is to ensure fast manipulation of larget text data consisting of multiple such 'Text' objects.
	class Text : public IRenderable, public UIDriverObject
	{
	private:
		TextContainer* m_container;
		
		std::vector<LineText*> m_lines;

		bool m_isDirty;

		// data and rendering attributes
		Color3 m_color;
		TPGEmphasis m_emphasis;
		GfxDriverObjectHandleType m_font;
		u32 m_pointSize;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;

		// this can only be called by SUTK::UIDriver
		Text(UIDriver& driver, TextContainer* container);

		friend class UIDriver;

	public:

		virtual bool isDirty() override;
		virtual void update() override;

		void clear();
		LineText* createNewLine();
		LineText* getLastLine();
		void append(const std::string& str);
		void set(const std::string& str);

		TextContainer* getContainer() noexcept { return m_container; }
	};
}
