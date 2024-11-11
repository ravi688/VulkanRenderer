#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> /* for SUTK::Renderable */
#include <sutk/IColorable.hpp> // for SUTK::IColorable 

#include <string> /* for std::string */
#include <vector> /* for std::vector*/
#include <limits> /* for std::numeric_limits */
#include <ostream> // for std::ostream

#include <sutk/SmallText.hpp> // for SUTK::SmallText
#include <sutk/TextTraits.hpp>

namespace SUTK
{
	class LineText : public SmallText
	{
	public:
		LineText(UIDriver& driver, GfxDriverObjectHandleType textGroup, Color4 color = SUTK::Color4::white()) noexcept : SmallText(driver, NULL, textGroup, color) { }
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
		bool operator==(const CursorPosition& rhs) const noexcept
		{
			return (m_line == rhs.m_line) && (m_col == rhs.m_col);
		}
		bool operator <(const CursorPosition& rhs) const noexcept
		{
			return (m_line < rhs.m_line) || ((m_line == rhs.m_line) && (m_col < rhs.m_col));
		}
		bool operator <=(const CursorPosition& rhs) const noexcept
		{
			return (m_line < rhs.m_line) || ((m_line == rhs.m_line) && (m_col <= rhs.m_col));
		}
		bool operator >(const CursorPosition& rhs) const noexcept
		{
			return (m_line > rhs.m_line) || ((m_line == rhs.m_line) && (m_col > rhs.m_col));
		}
		bool operator >=(const CursorPosition& rhs) const noexcept
		{
			return (m_line > rhs.m_line) || ((m_line == rhs.m_line) && (m_col >= rhs.m_col));
		}

		void moveToNextLine(const T& max = std::numeric_limits<T>::max()) noexcept { if(m_line < max) m_line += 1; }
		void moveToPrevLine(const T& min = std::numeric_limits<T>::min()) noexcept { if(m_line > min) m_line -= 1; }
		void moveToNextColumn(const T& max = std::numeric_limits<T>::max()) noexcept { if(m_col < max) m_col += 1; }
		void moveToPrevColumn(const T& min = std::numeric_limits<T>::min()) noexcept { if(m_col > min) m_col -= 1; }
		void moveToLine(T line) noexcept { m_line = line; }
		void moveToColumn(T col) noexcept { m_col = col; }
		T getLine() const noexcept { return m_line; }
		T getColumn() const noexcept { return m_col; }

		static CursorPosition StartOfText();
		static CursorPosition StartOfLine(T line);
		static CursorPosition EndOfText();
		static CursorPosition EndOfLine(T line);
	};

	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::StartOfText();
	template<> CursorPosition<LineCountType> CursorPosition<LineCountType>::StartOfLine(LineCountType line);
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

		// Clip Rect in global coords set by the user of Text
		std::optional<Rect2Df> m_clipRect;

		// this can only be called by SUTK::UIDriver
		Text(UIDriver& driver, RenderableContainer* container) noexcept;
		~Text() noexcept;

		friend class UIDriver;

		void recalculateClipRect() noexcept;
		f32 calculateMaxWidth() const noexcept;


		std::pair<s32, s32> getUnclippedLineRange() noexcept;
		void updateLinePositions() noexcept;
		LineCountType getNumChars(const CursorPosition<LineCountType>& position) noexcept;
		void addColorRange(const SelectionRange<LineCountType>& range, Color4 color) noexcept;

	protected:
		// Overrides of Renderable::onContainerResize
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;
		virtual void updateNormalizedDrawOrder(f32 normalizedDrawOrder) override;
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

		virtual void setActive(com::Bool isActive) noexcept override;

		void clear() noexcept;

		void setFont(UIDriver::FontReference font) noexcept;

		// Implementation of IColorable::setColor() and IColorable::getColor()
		virtual void setColor(Color4 color) noexcept override;
		virtual Color4 getColor() const noexcept override;

		void setClipRect(Rect2Df rect) noexcept;
		Rect2Df getClipRect() const noexcept;

		// NOTE: Followings are the modification functions (which modify the text content and eventually bound size)
		//		1. createNewLine
		//		2. removeLine
		//		3. removeLineRange
		//		5. append
		//		6. insert
		//		7. removeRange
		//		8. setFontSize
		Vec2Df getBoundSize() const noexcept;

		void setColorRanges(const std::vector<std::pair<SelectionRange<LineCountType>, Color4>>& ranges) noexcept;



		LineText* createNewLine(Flags flags = Flags::After, LineCountType line = END_OF_TEXT) noexcept;
		void removeLine(LineCountType line) noexcept;
		// NOTE: end is exclusive here, while start is inclusive
		void removeLineRange(LineCountType start, LineCountType end) noexcept;
		LineText* getOrCreateLastLine() noexcept;
		void append(const std::string& str) noexcept { insert(CursorPosition<LineCountType>::EndOfText(), str); }
		LineText* getLine(LineCountType line) noexcept;
		const LineText* getLine(LineCountType line) const noexcept { return const_cast<Text*>(this)->getLine(line); }
		CursorPosition<LineCountType> insert(CursorPosition<LineCountType> position, const std::string& str) noexcept;
		void removeRange(CursorPosition<LineCountType> start, CursorPosition<LineCountType> end) noexcept;
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

		void serialize(std::ostream& stream) const noexcept;
		void serializeToFile(const std::string& filePath) const noexcept;
		void deserialize(std::istream& stream) noexcept;
		void deserializeFromFile(const std::string& filePath, CursorPosition<LineCountType> pos = CursorPosition<LineCountType>::EndOfText()) noexcept;
	};
}
