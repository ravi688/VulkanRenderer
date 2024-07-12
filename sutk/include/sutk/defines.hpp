
#pragma once

#include <common/defines.hpp>

#ifdef SUTK_STATIC_LIBRARY
#	define SUTK_API
#elif SUTK_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllexport)
#else
#	define SUTK_API
#endif


#include <limits>

namespace SUTK
{
	/* For now, we will only support ASCII set of characters,
	 * but in future we may increase its bit-width and display more
	 * complex characters. */
	typedef u8 CharType;

	#define CHAR_TYPE_MAX std::numeric_limits<CharType>::max()

	/* For now, we can represent number of lines up to U32_MAX,
	 * but in future we may increase its bit-width to let users to 
	 * work with gigantic text files. */
	typedef u32 LineCountType;

	#define LINE_COUNT_TYPE_MAX std::numeric_limits<LineCountType>::max()
	/* should be used in place of 'column number' */
	#define END_OF_LINE LINE_COUNT_TYPE_MAX
	/* should be used in place of 'line number' in a multiline text */
	#define END_OF_TEXT LINE_COUNT_TYPE_MAX

	/* For now, we can support display resolutions upto U16_MAX x U16_MAX pixels,
	 * but in future we may increase its bit-width to support gigantic displays
	 * with larger resolutions. However, it would never be needed I guess. */
	typedef u16 DisplaySizeType;

	#define DISPLAY_SIZE_TYPE_MAX std::numeric_limits<DisplaySizeType>::max()

	template<typename T>
	union Vec2D
	{
		struct
		{
			T x;
			T y;
		};

		struct
		{
			T width;
			T height;
		};

		Vec2D() noexcept : x(0), y(0) { }
		Vec2D(T _x, T _y) noexcept : x(_x), y(_y) { }

		// static methods
		static Vec2D<T> zero();

		// operator overloads
		operator ==(const Vec2D<T>& v) noexcept { return (x == v.x) && (y == v.y); }
		operator !=(const Vec2D<T>& v) noexcept { return !operator==(v); }

		// arithmetic operator overloads
		Vec2D<T> operator +(const Vec2D<T>& v) const noexcept { return { x + v.x, y + v.y }; }
		Vec2D<T> operator -(const Vec2D<T>& v) const noexcept { return { x - v.x, y - v.y }; }
		Vec2D<T>& operator +=(const Vec2D<T>& v) noexcept { x += v.x; y += v.y; return *this; }
		Vec2D<T>& operator -=(const Vec2D<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
	};

	template<typename T>
	Vec2D<T> Vec2D<T>::zero() { return { 0, 0 }; }

	template<typename T>
	struct Rect2D
	{
		T x;
		T y;
		T width;
		T height;

		Rect2D() noexcept : x(0), y(0), width(0), height(0) { }
		Rect2D(T _x, T _y, T _width, T _height) noexcept : x(_x), y(_y), width(_width), height(_height) { }

		Vec2D<T> getPosition() const noexcept { return { x, y }; }
		Vec2D<T> getSize() const noexcept { return { width, height }; }

		void setPosition(Vec2D<T> pos) noexcept
		{
			x = pos.x;
			y = pos.y;
		}

		void setSize(Vec2D<T> size) noexcept
		{
			width = size.width;
			height = size.height;
		}
	};

	typedef u32 GfxDriverObjectHandleType;
	#define GFX_DRIVER_OBJECT_NULL_HANDLE U32_MAX

	struct Color3
	{
		u8 r;
		u8 g;
		u8 b;

		Color3() noexcept : r(0), g(0), b(0) { }
		Color3(u8 _r, u8 _g, u8 _b) noexcept : r(_r), g(_g), b(_b) { }

		operator ==(Color3& rhs) const noexcept
		{
			return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
		}

		operator !=(Color3& rhs) const noexcept
		{
			return !Color3::operator==(rhs);
		}
	};
}
