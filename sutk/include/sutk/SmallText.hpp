#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp> // for SUTK::GfxDriverRenderable
#include <sutk/IColorable.hpp> // for SUTK::IColorable
#include <sutk/TextTraits.hpp>

#include <string> // for std:string
#include <vector> // for std::vector

namespace SUTK
{
	class UIDriver;
	class RenderableContainer;

	class ColumnIterator { };

	class SmallTextData
	{
	private:
		std::string m_data;
	public:
		// constructors
		SmallTextData() = default;
		SmallTextData(SmallTextData&& data) : m_data(std::move(data.m_data)) { }
		SmallTextData(const SmallTextData& data) : m_data(data.m_data) { }

		// assignment operators
		SmallTextData& operator =(SmallTextData&& data) { m_data = std::move(data.m_data); return *this; }
		SmallTextData& operator =(const SmallTextData& data) { m_data = data.m_data; return *this; }
		SmallTextData& operator =(const std::string& data) { m_data = data; return *this; }
		SmallTextData& operator =(std::string&& data) { m_data = std::move(data); return *this; }
	
		// implict conversion operators
		operator const std::string&() const { return m_data; }
		operator const std::string_view() const { return m_data; }

		// equality comparison operators
		bool operator ==(const SmallTextData& data) { return m_data == data.m_data; }
		bool operator ==(const std::string& data) { return m_data == data; }

		// concatenation operator
		SmallTextData& operator +=(const SmallTextData& data) { m_data += data.m_data; return *this; }
		SmallTextData& operator +=(const std::string& data) { m_data += data; return *this; }

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

	class SUTK_API SmallText : public GfxDriverRenderable, public IColorable
	{
	private:
		struct DirtyTable
		{
			bool isPosDirty;
			bool isDataDirty;
			bool isPointSizeDirty;
			bool isColorDirty;
			bool isColorRangesDirty;
			void clear() noexcept
			{
				isPosDirty = false;
				isDataDirty = false;
				isPointSizeDirty = false;
				isColorDirty = false;
				isColorRangesDirty = false;
			}
			bool isAny() noexcept
			{
				return isPosDirty
				|| isDataDirty
				|| isPointSizeDirty
				|| isColorDirty
				|| isColorRangesDirty;
			}
		};
		DirtyTable m_dirtyTable {};
		SmallTextData m_data;
		Color4 m_color;
		std::vector<ColorRange> m_colorRanges;
		Vec2Df m_pos;
		// Automtically initialized to FontInvalid (a.k.a GFX_DRIVER_OBJECT_NULL_HANDLE)
		UIDriver::FontReference m_font {};
		f32 m_pointSize;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		f32 m_normalizedDrawOrder;



		friend class Text;

		void updatePointSize() noexcept;

	protected:
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;
		virtual void updateNormalizedDrawOrder(f32 normalizedDrawOrder) override;
		virtual void onActiveUpdate(com::Bool isActive) noexcept override;
	public:

		SmallText(UIDriver& driver, RenderableContainer* container, GfxDriverObjectHandleType textGroup, Color4 color = SUTK::Color4::white()) noexcept;
		~SmallText() noexcept;

		// Implementation of Renderable::isDirty() and Renderable::update()
		virtual bool isDirty() override;
		virtual void update() override;

		// Implementation of IColorable::setColor() and IColorable::getColor()
		virtual void setColor(Color4 color) noexcept override;
		virtual Color4 getColor() const noexcept override;

		void clearColorRanges() noexcept;
		void addColorRange(std::size_t pos, std::size_t len, const Color4 color) noexcept;
		
		// NOTE: The below two set of functions should only be used when the SmallText is active
		// Otherwise, the actual Text object in IGfxDriver backedn might return 0 in all cases.
		// returns column (index of a glyph) given a coordinate (absisca) along the line length.
		LineCountType getColPosFromCoord(f32 coord) noexcept;
		f32 getCoordFromColPos(LineCountType col) noexcept;


		void setData(const std::string_view data) noexcept;
		const SmallTextData& getData() const noexcept { return m_data; }
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
		void setFontSize(const f32 pointSize) noexcept;
		f32 getFontSize() noexcept;
		void setFont(UIDriver::FontReference font) noexcept;
		UIDriver::FontReference getFont() noexcept { return m_font; }
		f32 getBaselineHeight() noexcept;

		// NOTE: The below two set of functions should only be used when the SmallText is active
		// Otherwise, the actual Text object in IGfxDriver backend might return zero in case the text object is inactive.
		Vec2Df getBoundingRectSize() noexcept;
		Vec2Df getAlignedPosition(Vec2Df pos) noexcept;

		void setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept;
		std::pair<HorizontalAlignment, VerticalAlignment> getAlignment() const noexcept { return { m_horizontalAlignment, m_verticalAlignment }; }
		HorizontalAlignment getHorizontalAlignment() const noexcept { return m_horizontalAlignment; }
		VerticalAlignment getVerticalAlignment() const noexcept { return m_verticalAlignment; }
	};
}