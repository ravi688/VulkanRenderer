#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> /* for SUTK::Renderable */
#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */

#include <string> /* for std::string */
#include <vector> /* for std::vector*/
#include <limits> /* for std::numeric_limits */
#include <ostream> // for std::ostream

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
		void removeRange(std::size_t pos, std::size_t len = std::string::npos) noexcept { m_data.erase(pos, len); }
		void append(const std::string& chars) { insert(END_OF_LINE, chars); }
		std::string substr(std::size_t pos, std::size_t len) noexcept { return m_data.substr(pos, len); }
		void insert(LineCountType col, const std::string& chars)
		{ 
			if(col == END_OF_LINE)
					m_data += chars;
			else
				m_data.insert(col, chars); 
		}
	};

	class LineText : public GfxDriverRenderable
	{
	private:
		bool m_isPosDirty;
		bool m_isDataDirty;
		LineTextData m_data;
		Vec2Df m_pos;

		LineText(UIDriver& driver, GfxDriverObjectHandleType textGroup) noexcept;

		friend class Text;

	public:

		// Implementation of Renderable::isDirty() and Renderable::update()
		virtual bool isDirty() override;
		virtual void update() override;
		
		// returns column (index of a glyph) given a coordinate (absisca) along the line length.
		LineCountType getColPosFromCoord(f32 coord) noexcept;
		f32 getCoordFromColPos(LineCountType col) noexcept;
		void setData(const std::string& data) noexcept;
		void removeRange(std::size_t pos, std::size_t len = std::string::npos) noexcept;
		void append(const std::string& data) noexcept;
		std::string substr(std::size_t pos, std::size_t len) noexcept { return m_data.substr(pos, len); }
		void insert(LineCountType col, const std::string& data) noexcept;
		void setPosition(Vec2Df pos) noexcept;
		Vec2Df getPosition() const noexcept { return m_pos; }
		void addPosition(Vec2Df pos) noexcept;
		void subPosition(Vec2Df pos) noexcept;
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
		void moveToNextColumn(const T& max = std::numeric_limits<T>::max()) noexcept { if(m_col < max) m_col += 1; }
		void moveToPrevColumn(const T& min = std::numeric_limits<T>::min()) noexcept { if(m_col > min) m_col -= 1; }
		void moveToLine(T line) noexcept { m_line = line; }
		void moveToColumn(T col) noexcept { m_col = col; }
		T getLine() const noexcept { return m_line; }
		T getColumn() const noexcept { return m_col; }

		static CursorPosition EndOfText();
		static CursorPosition EndOfLine(T line);
	};

	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfText();
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::EndOfLine(LineCountType line);


	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const CursorPosition<T>& pos)
	{
		stream << "{ line: " << pos.getLine() << ", col: " << pos.getColumn() << " }";
		return stream;
	}

	// One text object is responsible for rendering a small to medium sized sub-text
	// This usually translates to a single Gfx API specific buffer object. For example, it is VkBuffer (and VkDeviceMemory) in vulkan.
	// This is to ensure fast manipulation of larget text data consisting of multiple such 'Text' objects.
	class Text : public Renderable
	{
	private:
		GfxDriverObjectHandleType m_textGroup;
		std::vector<LineText*> m_lines;
		
		// distance between two consecutive base lines in centimeters
		f32 m_baselineHeight;

		bool m_isDirty;
		bool m_isClippingEnabled;

		// data and rendering attributes
		Color3 m_color;
		TPGEmphasis m_emphasis;
		GfxDriverObjectHandleType m_font;
		u32 m_pointSize;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;

		// m_scrollDelta.y represents scroll delta in y direction (vertical)
		// m_scrollDelta.x represents scroll delta in x direction (horizontal)
		Vec2Df m_scrollDelta;

		// this can only be called by SUTK::UIDriver
		Text(UIDriver& driver, RenderableContainer* container) noexcept;

		friend class UIDriver;

		void recalculateClipRect() noexcept;

		// Overrides of Renderable::onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;

		std::pair<s32, s32> getUnclippedLineRange() noexcept;
	public:

		Vec2Df getLocalPositionFromCursorPosition(const CursorPosition<LineCountType>& cursor) noexcept;
		CursorPosition<LineCountType> getCursorPosFromCoords(Vec2Df coords) noexcept;

		enum class Flags
		{
			Before,
			After
		};

		// Implementation of Renderable::isDirty() and Renderable::update()
		virtual bool isDirty() override;
		virtual void update() override;

		void clear() noexcept;

		LineText* createNewLine(Flags flags = Flags::After, LineCountType line = END_OF_TEXT) noexcept;
		LineText* getOrCreateLastLine() noexcept;
		void append(const std::string& str) noexcept { insert(CursorPosition<LineCountType>::EndOfText(), str); }
		LineText* getLine(LineCountType line) noexcept;
		void insert(const CursorPosition<LineCountType>& position, const std::string& str) noexcept;
		void remove(const CursorPosition<LineCountType>& position, LineCountType numChars) noexcept;
		void set(const std::string& str) noexcept;
		void enableClipping(bool isEnable = true) noexcept;
		void setScrollDelta(Vec2Df delta) noexcept;
		void addScrollDelta(Vec2Df delta) noexcept;
		Vec2Df getScrollDelta() const noexcept { return m_scrollDelta; }
	};
}
