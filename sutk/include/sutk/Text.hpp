#pragma once

#include <sutk/defines.hpp>
#include <sutk/IRenderable.hpp> /* for SUTK::IRenderable */

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

	class SubText : public IRenderable
	{
	private:
		UIDriver& m_driver;
		GfxDriverObjectHandleType m_handle;
		bool m_isDirty;

		// data and rendering attributes 
		std::string m_data;
		Color3 m_color;
		TPGEmphasis m_emphasis;
		GfxDriverObjectHandleType m_font;
		u32 m_pointSize;

		SubText(UIDriver& driver, GfxDriverObjectHandleType handle) noexcept : m_driver(driver), m_handle(handle) { }

	public:

		virtual bool isDirty() const override { return m_isDirty; }
		virtual void update() override;

		// getters
		Color3 getColor() const noexcept { return m_color; }
		TPGEmphasis getEmphasis() const noexcept { return m_emphasis; }
		GfxDriverObjectHandleType getFont() const noexcept { return m_font; }

		// below set of functions mark this 'SubText' as dirty, i.e. set m_isDirty to true
		void setData(const std::string& data) noexcept;
		void setColor(const Color3 color) noexcept;
		void setEmphasis(TPGEmphasis emphasis) noexcept;
		void setFont(GfxDriverObjectHandleType font) noexcept;
		void setSize(u32 pointSize) noexcept;
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
	class Text : public IRenderable
	{
	private:
		UIDriver& m_driver;
		TextContainer* m_container;
		GfxDriverObjectHandleType m_handle;
		std::vector<GfxDriverObjectHandleType> m_subHandles;
		bool m_isDirty;

		// data and rendering attributes
		std::vector<std::string> m_lines;
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

		virtual bool isDirty() const override { return m_isDirty; }
		virtual void update() override;

		void set(const std::string& str);
		void set(const std::string& str, LineCountType lineNo);
		void set(std::string&& str);
		void set(std::string&& str, LineCountType lineNo);
		void append(const std::string& str);
		void appendChar(const CharType ch);
		void insert(const std::string& str, LineCountType lineNo, LineCountType columnNo);
		void insertChar(const CharType ch, LineCountType lineNo, LineCountType columnNo);

		// below set of functions markt this 'Text' is 'dirty', i.e. set m_isDirty to true
		void setColor(const Color3 color) noexcept;
		void setEmphasis(const TPGEmphasis style) noexcept;
		void setFont(GfxDriverObjectHandleType font) noexcept;
		void setSize(u32 pointSize) noexcept;
		void setHorizontalAlignment(const HorizontalAlignment alignment) noexcept;
		void setVerticalAlignment(const VerticalAlignment alignment) noexcept;

		LineCountType getLineCount() const;
		LineCountType getColumnCount(LineCountType lineNo) const;

		TextContainer* getContainer() noexcept { return m_container; }
	};
}
