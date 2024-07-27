
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
#include <sstream>

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

		constexpr Vec2D() noexcept : x(0), y(0) { }
		constexpr Vec2D(T _x, T _y) noexcept : x(_x), y(_y) { }

		// static methods
		static Vec2D<T> zero();
		static Vec2D<T> left();
		static Vec2D<T> right();
		static Vec2D<T> up();
		static Vec2D<T> down();

		// operator overloads
		bool operator ==(const Vec2D<T>& v) noexcept { return (x == v.x) && (y == v.y); }
		bool operator !=(const Vec2D<T>& v) noexcept { return !operator==(v); }

		Vec2D<T>& operator=(const Vec2D<T> v) noexcept { x = v.x; y = v.y; return *this; }

		// arithmetic operator overloads
		Vec2D<T> operator +(const Vec2D<T>& v) const noexcept { return { x + v.x, y + v.y }; }
		Vec2D<T> operator -(const Vec2D<T>& v) const noexcept { return { x - v.x, y - v.y }; }
		Vec2D<T> operator *(const Vec2D<T>& v) const noexcept { return { x * v.x, y * v.y }; }
		Vec2D<T> operator *(T s) const noexcept { return { x * s, y * s }; }
		Vec2D<T>& operator +=(const Vec2D<T>& v) noexcept { x += v.x; y += v.y; return *this; }
		Vec2D<T>& operator -=(const Vec2D<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }

		// implicit conversion operator overloads
		template<typename U>
		operator Vec2D<U>()
		{
			return { static_cast<U>(x), static_cast<U>(y) };
		}
	};

	template<typename T>
	Vec2D<T> operator *(T s, const Vec2D<T>& v) noexcept { return { s * v.x, s * v.y }; }

	template<typename T>
	struct NegativeSign { };

	template<>
	struct NegativeSign<s64> { static constexpr s64 value = -1; };
	template<>
	struct NegativeSign<s32> { static constexpr s32 value = -1; };
	template<>
	struct NegativeSign<s16> { static constexpr s16 value = -1; };
	template<>
	struct NegativeSign<s8> { static constexpr s8 value = -1; };
	template<>
	struct NegativeSign<f32> { static constexpr f32 value = -1.0f; };
	template<>
	struct NegativeSign<f64> { static constexpr f64 value = -1.0; }; 

	// Coordinate System
	//  ______________ x
	// |
	// |
	// |  Downwards is +ve
	// y
	template<typename T>
	Vec2D<T> Vec2D<T>::zero() { return { 0, 0 }; }
	template<typename T>
	Vec2D<T> Vec2D<T>::left() { return { NegativeSign<T>::value, 0 }; }
	template<typename T>
	Vec2D<T> Vec2D<T>::right() { return { 1, 0 }; }
	template<typename T>
	Vec2D<T> Vec2D<T>::up() { return { 0, NegativeSign<T>::value }; }
	template<typename T>
	Vec2D<T> Vec2D<T>::down() { return { 0, 1 }; }

	template<typename T>
	struct Rect2D
	{
		T x;
		T y;
		T width;
		T height;

		constexpr Rect2D() noexcept : x(0), y(0), width(0), height(0) { }
		constexpr Rect2D(T _x, T _y, T _width, T _height) noexcept : x(_x), y(_y), width(_width), height(_height) { }

		Vec2D<T> getPosition() const noexcept { return { x, y }; }
		Vec2D<T> getSize() const noexcept { return { width, height }; }

		Vec2D<T> getCenter() const noexcept
		{
			return static_cast<Vec2D<f32>>(getPosition()) + static_cast<Vec2D<f32>>(getSize()) * 0.5;
		}

		Vec2D<T> getTopLeft() const noexcept
		{
			return getPosition();
		}

		Vec2D<T> getTopRight() const noexcept
		{
			return getPosition() + Vec2D<T>::right() * width;
		}

		Vec2D<T> getBottomRight() const noexcept
		{
			return getPosition() + Vec2D<T> { width, height };
		}

		Vec2D<T> getBottomLeft() const noexcept
		{
			return getPosition() + Vec2D<T>::down() * height;
		}

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

		std::string toString() const noexcept
		{
			// NOTE: we are creating here r-value std::ostringstream object
			// because in C++20, it has r-value overload for str() function
			// which returns std::move(underlying str object), this way we are only
			// allocating one std::string and that is being returned at the end out 
			// of this toString() function.
			return operator <<(std::ostringstream(), *this).str();
		}
	};

	template<typename T>
	std::ostream&& operator <<(std::ostream&& stream, const Rect2D<T>& rect)
	{
		auto& _stream = operator << <T>(stream, rect);
		return std::move(_stream);
	}
	
	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const Rect2D<T>& rect)
	{
		stream << "{ " << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << " }";
		return stream;
	}

	typedef u64 GfxDriverObjectHandleType;
	#define GFX_DRIVER_OBJECT_NULL_HANDLE U64_MAX

	struct Color3
	{
		u8 r;
		u8 g;
		u8 b;

		constexpr Color3() noexcept : r(0), g(0), b(0) { }
		constexpr Color3(u8 _r, u8 _g, u8 _b) noexcept : r(_r), g(_g), b(_b) { }

		bool operator ==(Color3& rhs) const noexcept
		{
			return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
		}

		bool operator !=(Color3& rhs) const noexcept
		{
			return !Color3::operator==(rhs);
		}

		static constexpr Color3 red() noexcept { return { 255, 0, 0 }; }
		static constexpr Color3 blue() noexcept { return { 0, 255, 0 }; }
		static constexpr Color3 green() noexcept { return { 0, 0, 255 }; }
		static constexpr Color3 white() noexcept { return { 255, 255, 255 }; }
		static constexpr Color3 black() noexcept { return { 0, 0, 0 }; }
	};
}
