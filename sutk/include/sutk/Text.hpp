#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> /* for SUTK::Renderable */
#include <sutk/UIDriver.hpp> /* for SUTK::UIDriver::getGfxDriver() */
#include <sutk/IColorable.hpp> // for SUTK::IColorable 

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

		std::size_t size() const noexcept { return m_data.size(); }
	};

	class LineText : public GfxDriverRenderable, public IColorable
	{
	private:
		bool m_isPosDirty;
		bool m_isDataDirty;
		bool m_isPointSizeDirty;
		bool m_isColorDirty;
		bool m_isColorRangesDirty;
		LineTextData m_data;
		Color4 m_color;
		std::vector<ColorRange> m_colorRanges;
		Vec2Df m_pos;
		f32 m_pointSize;

		LineText(UIDriver& driver, GfxDriverObjectHandleType textGroup, Color4 color = SUTK::Color4::white()) noexcept;

		friend class Text;

		// LineText is used only inside the Text class (Multi-line Text)
		// and it should not be allowed to set different font sizes for two or more LineText(s)
		void setFontSize(const f32 pointSize) noexcept;

		friend class Text;

		void updatePointSize() noexcept;

	public:

		// Implementation of Renderable::isDirty() and Renderable::update()
		virtual bool isDirty() override;
		virtual void update() override;

		// Implementation of IColorable::setColor() and IColorable::getColor()
		virtual void setColor(Color4 color) noexcept override;
		virtual Color4 getColor() const noexcept override;

		virtual void destroy() override;

		void clearColorRanges() noexcept;
		void addColorRange(std::size_t pos, std::size_t len, const Color4 color) noexcept;
		
		// returns column (index of a glyph) given a coordinate (absisca) along the line length.
		LineCountType getColPosFromCoord(f32 coord) noexcept;
		f32 getCoordFromColPos(LineCountType col) noexcept;
		void setData(const std::string& data) noexcept;
		const LineTextData& getData() const noexcept { return m_data; }
		void removeRange(std::size_t pos, std::size_t len = std::string::npos) noexcept;
		void append(const std::string& data) noexcept;
		std::string substr(std::size_t pos, std::size_t len) noexcept { return m_data.substr(pos, len); }
		void insert(LineCountType col, const std::string& data) noexcept;
		void setPosition(Vec2Df pos) noexcept;
		Vec2Df getPosition() const noexcept { return m_pos; }
		void addPosition(Vec2Df pos) noexcept;
		void subPosition(Vec2Df pos) noexcept;
		void clear() noexcept;

		std::size_t getColumnCount() const noexcept { return static_cast<const std::string&>(m_data).size(); }

		f32 getFontSize() noexcept;
		f32 getBaselineHeight() noexcept;
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
		bool operator <(const CursorPosition& rhs) const noexcept
		{
			return (m_line < rhs.m_line) && (m_col < rhs.m_col);
		}
		bool operator <=(const CursorPosition& rhs) const noexcept
		{
			return (m_line <= rhs.m_line) && (m_col <= rhs.m_col);
		}
		bool operator >(const CursorPosition& rhs) const noexcept
		{
			return (m_line > rhs.m_line) && (m_col > rhs.m_col);
		}
		bool operator >=(const CursorPosition& rhs) const noexcept
		{
			return (m_line >= rhs.m_line) && (m_col >= rhs.m_col);
		}

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

	template<typename T>
	struct SelectionRange
	{
		CursorPosition<T> begin;
		CursorPosition<T> end;
	};

	// One text object is responsible for rendering a small to medium sized sub-text
	// This usually translates to a single Gfx API specific buffer object. For example, it is VkBuffer (and VkDeviceMemory) in vulkan.
	// This is to ensure fast manipulation of larget text data consisting of multiple such 'Text' objects.
	class Text : public Renderable, public IColorable
	{
	private:
		GfxDriverObjectHandleType m_textGroup;
		std::vector<LineText*> m_lines;
		
		// distance between two consecutive base lines in centimeters
		f32 m_baselineHeight;

		bool m_isDirty;
		bool m_isClippingEnabled;

		// data and rendering attributes
		Color4 m_color;
		TPGEmphasis m_emphasis;
		GfxDriverObjectHandleType m_font;
		f32 m_pointSize;
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
		void updateLinePositions() noexcept;
		LineCountType getNumChars(const CursorPosition<LineCountType>& position) noexcept;
		void addColorRange(const SelectionRange<LineCountType>& range, Color4 color) noexcept;

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

		// Implementation of IColorable::setColor() and IColorable::getColor()
		virtual void setColor(Color4 color) noexcept override;
		virtual Color4 getColor() const noexcept override;

		void setColorRanges(const std::vector<std::pair<SelectionRange<LineCountType>, Color4>>& ranges) noexcept;

		LineText* createNewLine(Flags flags = Flags::After, LineCountType line = END_OF_TEXT) noexcept;
		void removeLine(LineCountType line) noexcept;
		LineText* getOrCreateLastLine() noexcept;
		void append(const std::string& str) noexcept { insert(CursorPosition<LineCountType>::EndOfText(), str); }
		LineText* getLine(LineCountType line) noexcept;
		void insert(const CursorPosition<LineCountType>& position, const std::string& str) noexcept;
		void removeRange(const CursorPosition<LineCountType>& start, const CursorPosition<LineCountType>& end) noexcept;
		void set(const std::string& str) noexcept;
		void enableClipping(bool isEnable = true) noexcept;
		void setScrollDelta(Vec2Df delta) noexcept;
		void addScrollDelta(Vec2Df delta) noexcept;
		Vec2Df getScrollDelta() const noexcept { return m_scrollDelta; }

		std::size_t getLineCount() const noexcept { return m_lines.size(); }
		// TODO: Optimize it, maintain a monolithic std::string and pass lightweight slices (pair of indices) to LineText class,
		// and returns (that std::string).size() when getCharCount() is called.
		// That's because, at the end SGE caches the set character array internally and there is no need to maintain std::string objects for each line.
		// For now, it iterates through all the lines and sums the char count of each line.
		std::size_t getCharCount() const noexcept;

		void setFontSize(const f32 pointSize) noexcept;
		f32 getFontSize() const noexcept;
		f32 getBaselineHeight() noexcept;

		CursorPosition<LineCountType> begin() const noexcept;
		CursorPosition<LineCountType> end() const noexcept;
	};
}
