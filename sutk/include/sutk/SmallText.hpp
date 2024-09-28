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
		bool m_isPosDirty;
		bool m_isDataDirty;
		bool m_isPointSizeDirty;
		bool m_isColorDirty;
		bool m_isColorRangesDirty;
		bool m_isActiveDirty;
		SmallTextData m_data;
		Color4 m_color;
		std::vector<ColorRange> m_colorRanges;
		Vec2Df m_pos;
		f32 m_pointSize;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		f32 m_normalizedDrawOrder;



		friend class Text;

		void updatePointSize() noexcept;

	protected:
		virtual void onGlobalCoordDirty() noexcept override;
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept;
	public:

		SmallText(UIDriver& driver, RenderableContainer* container, GfxDriverObjectHandleType textGroup, Color4 color = SUTK::Color4::white()) noexcept;

		// Implementation of Renderable::isDirty() and Renderable::update()
		virtual bool isDirty() override;
		virtual void update() override;
		virtual void updateNormalizedDrawOrder(f32 normalizedDrawOrder) override;

		// Implementation of IColorable::setColor() and IColorable::getColor()
		virtual void setColor(Color4 color) noexcept override;
		virtual Color4 getColor() const noexcept override;

		// Override of Renderable::setActive()
		virtual void setActive(bool isActive) noexcept override;

		virtual void destroy() override;

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
		f32 getBaselineHeight() noexcept;

		// NOTE: The below two set of functions should only be used when the SmallText is active
		// Otherwise, the actual Text object in IGfxDriver backend might return zero in case the text object is inactive.
		Vec2Df getBoundingRectSize() noexcept;
		Vec2Df getAlignedPosition(Vec2Df pos) noexcept;

		void setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept;
	};
}