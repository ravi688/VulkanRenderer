#pragma once

#include <sutk/defines.hpp>
#include <sutk/IRenderable.hpp> /* for SUTK::IRenderable */
#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */

#include <string> /* for std::string */
#include <vector> /* for std::vector*/
#include <limits> /* for std::numeric_limits */

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
		bool operator ==(const LineTextData& data) { return m_data == data.m_data; }
		bool operator ==(const std::string& data) { return m_data == data; }

		// concatenation operator
		LineTextData& operator +=(const LineTextData& data) { m_data += data.m_data; return *this; }
		LineTextData& operator +=(const std::string& data) { m_data += data; return *this; }

		ColumnIterator begin() { return ColumnIterator(); }
		ColumnIterator end() { return ColumnIterator(); }
	
		void clear() { m_data.clear(); }
		void append(const std::string& chars) { insert(END_OF_LINE, chars); }
		void insert(LineCountType col, const std::string& chars)
		{ 
			if(col == END_OF_LINE)
					m_data += chars;
			else
				m_data.insert(col, chars); 
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
		void insert(LineCountType col, const std::string& data) noexcept;
		void setPosition(Vec2D<DisplaySizeType> pos) noexcept;
		void addPosition(Vec2D<DisplaySizeType> pos) noexcept;
		void subPosition(Vec2D<DisplaySizeType> pos) noexcept;
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
	
	template<typename T>
	class CursorPosition
	{
	private:
		T m_line;
		T m_col;

	public:
		CursorPosition() noexcept : m_line(0), m_col(0) { }
		CursorPosition(T line, T col = 0) noexcept : m_line(line), m_col(col) { }
		CursorPosition(const CursorPosition&) = default;
		CursorPosition& operator=(const CursorPosition&) = default;

		void moveToNextLine(const T& max = std::numeric_limits<T>::max()) noexcept { if(m_line < max) m_line += 1; }
		void moveToPrevLine(const T& min = std::numeric_limits<T>::min()) noexcept { if(m_line > min) m_line -= 1; }
		void moveToLine(T line) noexcept { m_line = line; }
		void moveToColumn(T col) noexcept { m_col = col; }
		T getLine() const noexcept { return m_line; }
		T getColumn() const noexcept { return m_col; }
	};


	// One text object is responsible for rendering a small to medium sized sub-text
	// This usually translates to a single Gfx API specific buffer object. For example, it is VkBuffer (and VkDeviceMemory) in vulkan.
	// This is to ensure fast manipulation of larget text data consisting of multiple such 'Text' objects.
	class Text : public IRenderable, public UIDriverObject
	{
	private:
		TextContainer* m_container;
		
		std::vector<LineText*> m_lines;
		
		// distance between two consecutive base lines in screen pixel coordinate
		u32 m_baselineHeight;

		bool m_isDirty;

		// data and rendering attributes
		Color3 m_color;
		TPGEmphasis m_emphasis;
		GfxDriverObjectHandleType m_font;
		u32 m_pointSize;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;

		// this can only be called by SUTK::UIDriver
		Text(UIDriver& driver, TextContainer* container) noexcept;

		friend class UIDriver;
		friend class TextContainer;

		Vec2D<DisplaySizeType> getLocalPositionFromCursorPosition(const CursorPosition<DisplaySizeType>& cursor) noexcept;
		void onContainerResize(const Rect2D<DisplaySizeType>& rect, bool isPositionChanged, bool isSizeChanged) noexcept;

	public:

		enum class Flags
		{
			Before,
			After
		};

		virtual bool isDirty() override;
		virtual void update() override;

		void clear() noexcept;

		LineText* createNewLine(Flags flags = Flags::After, LineCountType line = END_OF_TEXT) noexcept;
		LineText* getOrCreateLastLine() noexcept;
		void append(const std::string& str) noexcept { insert(END_OF_TEXT, END_OF_LINE, str); }
		LineText* getLine(LineCountType line) noexcept;
		void insert(LineCountType line, LineCountType col, const std::string& str) noexcept;
		void set(const std::string& str) noexcept;

		TextContainer* getContainer() noexcept { return m_container; }
	};
}
