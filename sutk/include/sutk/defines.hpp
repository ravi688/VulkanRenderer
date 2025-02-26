
#pragma once

#include <common/defines.hpp>
#include <common/Bool.hpp> // It might not be used in this header file, but I placed it here so that every source file will get this when it includes this header.

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
#include <type_traits> // for std::is_constant_evaluated()

namespace SUTK
{
	/* For now, we will only support ASCII set of characters,
	 * but in future we may increase its bit-width and display more
	 * complex characters. */
	typedef u8 CharType;

	#define CHAR_TYPE_MAX std::numeric_limits<SUTK::CharType>::max()

	/* For now, we can represent number of lines up to U32_MAX,
	 * but in future we may increase its bit-width to let users to 
	 * work with gigantic text files. */
	typedef u32 LineCountType;

	#define LINE_COUNT_TYPE_MAX std::numeric_limits<SUTK::LineCountType>::max()
	/* should be used in place of 'column number' */
	#define END_OF_LINE LINE_COUNT_TYPE_MAX
	/* should be used in place of 'line number' in a multiline text */
	#define END_OF_TEXT LINE_COUNT_TYPE_MAX

	/* For now, we can support display resolutions upto U16_MAX x U16_MAX pixels,
	 * but in future we may increase its bit-width to support gigantic displays
	 * with larger resolutions. However, it would never be needed I guess. */
	typedef u16 DisplaySizeType;

	#define DISPLAY_SIZE_TYPE_MAX std::numeric_limits<SUTK::DisplaySizeType>::max()


	#define F32_INFINITY std::numeric_limits<f32>::max()
	#define F32_NEG_INFINITY std::numeric_limits<f32>::lowest()

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

	// Enumeration to represent cardination directions or sides of a rectangular object
	enum class CardinalDir : u8
	{
		Left,
		Right,
		Up,
		Down,
		Top = Up,
		Bottom = Down,
		West = Left,
		East = Right,
		North = Up,
		South = Down
	};

	enum class CardinalDirBits : u8
	{
		Left = BIT8(com::EnumClassToInt(CardinalDir::Left)),
		Right = BIT8(com::EnumClassToInt(CardinalDir::Right)),
		Up = BIT8(com::EnumClassToInt(CardinalDir::Up)),
		Down = BIT8(com::EnumClassToInt(CardinalDir::Down)),
		Top = BIT8(com::EnumClassToInt(CardinalDir::Top)),
		Bottom = BIT8(com::EnumClassToInt(CardinalDir::Bottom)),
		West = BIT8(com::EnumClassToInt(CardinalDir::West)),
		East = BIT8(com::EnumClassToInt(CardinalDir::East)),
		North = BIT8(com::EnumClassToInt(CardinalDir::North)),
		South = BIT8(com::EnumClassToInt(CardinalDir::South))
	};

	// Enumeration to represent sides of a rectangular object
	using RectEdge = CardinalDir;
	using RectEdgeBits = CardinalDirBits;

	template<typename T>
	struct Compass4D
	{
		T left;
		T right;
		union
		{
			struct
			{
				T top;
				T bottom;
			};
			struct
			{
				T up;
				T down;
			};
		};
	};

	SUTK_API std::ostream& operator<<(std::ostream& stream, CardinalDir cd) noexcept;

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

		constexpr T getWidth() const noexcept { return x; }
		constexpr T getHeight() const noexcept { return y; }

		// static methods
		static Vec2D<T> zero();
		static Vec2D<T> left();
		static Vec2D<T> right();
		static Vec2D<T> up();
		static Vec2D<T> down();

		// operator overloads
		bool operator ==(const Vec2D<T>& v) const noexcept { return (x == v.x) && (y == v.y); }
		bool operator !=(const Vec2D<T>& v) const noexcept { return !operator==(v); }

		Vec2D<T>& operator=(const Vec2D<T> v) noexcept { x = v.x; y = v.y; return *this; }

		// arithmetic operator overloads
		Vec2D<T> operator +(const Vec2D<T>& v) const noexcept { return { x + v.x, y + v.y }; }
		Vec2D<T> operator -(const Vec2D<T>& v) const noexcept { return { x - v.x, y - v.y }; }
		Vec2D<T> operator -() const noexcept { return { -x, -y }; }
		Vec2D<T> operator *(const Vec2D<T>& v) const noexcept { return { x * v.x, y * v.y }; }
		Vec2D<T> operator *(T s) const noexcept { return { x * s, y * s }; }
		Vec2D<T> operator /(const Vec2D<T>& v) const noexcept { return  { x / v.x, y / v.y }; }
		Vec2D<T>& operator +=(const Vec2D<T>& v) noexcept { x += v.x; y += v.y; return *this; }
		Vec2D<T>& operator -=(const Vec2D<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
		Vec2D<T>& operator *=(const Vec2D<T>& v) noexcept { x *= v.x; y *= v.y; return *this; }
		Vec2D<T>& operator *=(const T& v) noexcept { x *= v; y *= v; return *this; }
		Vec2D<T>& operator /=(const Vec2D<T>& v) noexcept { x /= v.x; y /= v.y; return *this; }
		Vec2D<T>& operator /=(const T& v) noexcept { x /= v; y /= v; return *this; }

		// relational operator overloads
		bool operator >=(const Vec2D<T>& v) const noexcept
		{
			return (x >= v.x) && (y >= v.y);
		}
		bool operator >(const Vec2D<T>& v) const noexcept
		{
			return (x > v.x) && (y > v.y);
		}
		bool operator <=(const Vec2D<T>& v) const noexcept
		{
			return (x <= v.x) && (y <= v.y);
		}
		bool operator <(const Vec2D<T>& v) const noexcept
		{
			return (x < v.x) && (y < v.y);
		}

		// implicit conversion operator overloads
		template<typename U>
		operator Vec2D<U>()
		{
			return { static_cast<U>(x), static_cast<U>(y) };
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
	Vec2D<T> operator *(T s, const Vec2D<T>& v) noexcept { return { s * v.x, s * v.y }; }

	template<typename T>
	std::ostream&& operator <<(std::ostream&& stream, const Vec2D<T>& v)
	{
		auto& _stream = operator << <T>(stream, v);
		return std::move(_stream);
	}
	
	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const Vec2D<T>& v)
	{
		stream << "{ " << v.x << ", " << v.y << " }";
		return stream;
	}

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
	union Vec3D
	{
		struct
		{
			T x;
			T y;
			T z;
		};

		struct
		{
			T width;
			T height;
			T depth;
		};

		Vec2D<T> xy;

		constexpr Vec3D() noexcept : x(0), y(0), z(0) { }
		constexpr Vec3D(T _x, T _y, T _z) noexcept : x(_x), y(_y), z(_z) { }

		// static methods
		static Vec3D<T> zero();
		static Vec3D<T> left();
		static Vec3D<T> right();
		static Vec3D<T> up();
		static Vec3D<T> down();
		static Vec3D<T> forward();
		static Vec3D<T> back();

		// operator overloads
		bool operator ==(const Vec3D<T>& v) noexcept { return (x == v.x) && (y == v.y) && (z == v.z); }
		bool operator !=(const Vec3D<T>& v) noexcept { return !operator==(v); }

		Vec3D<T>& operator=(const Vec3D<T> v) noexcept { x = v.x; y = v.y; z = v.z;return *this; }

		// arithmetic operator overloads
		Vec3D<T> operator +(const Vec3D<T>& v) const noexcept { return { x + v.x, y + v.y, z + v.z }; }
		Vec3D<T> operator -(const Vec3D<T>& v) const noexcept { return { x - v.x, y - v.y, z - v.z}; }
		Vec3D<T> operator -() const noexcept { return { -x, -y, -z }; }
		Vec3D<T> operator *(const Vec3D<T>& v) const noexcept { return { x * v.x, y * v.y, z * v.z }; }
		Vec3D<T> operator *(T s) const noexcept { return { x * s, y * s, z * s }; }
		Vec3D<T>& operator +=(const Vec3D<T>& v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
		Vec3D<T>& operator -=(const Vec3D<T>& v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
		Vec3D<T>& operator *=(const Vec3D<T>& v) noexcept { x *= v.x; y *= v.y; z *= v.z; return *this; }
		Vec3D<T>& operator *=(const T& v) noexcept { x *= v; y *= v; z *= v; return *this; }
		Vec3D<T>& operator /=(const Vec3D<T>& v) noexcept { x /= v.x; y /= v.y; z /= v.z; return *this; }
		Vec3D<T>& operator /=(const T& v) noexcept { x /= v; y /= v; z /= v; return *this; }

		// implicit conversion operator overloads
		template<typename U>
		operator Vec3D<U>()
		{
			return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
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
	Vec3D<T> operator *(T s, const Vec3D<T>& v) noexcept { return { s * v.x, s * v.y, s * v.z}; }

	template<typename T>
	std::ostream&& operator <<(std::ostream&& stream, const Vec3D<T>& v)
	{
		auto& _stream = operator << <T>(stream, v);
		return std::move(_stream);
	}
	
	template<typename T>
	std::ostream& operator <<(std::ostream& stream, const Vec3D<T>& v)
	{
		stream << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
		return stream;
	}

	// Coordinate System
	//  ______________ x
	// |
	// |
	// |  Downwards is +ve
	// y
	template<typename T>
	Vec3D<T> Vec3D<T>::zero() { return { 0, 0, 0 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::left() { return { NegativeSign<T>::value, 0, 0 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::right() { return { 1, 0, 0 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::up() { return { 0, NegativeSign<T>::value, 0 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::down() { return { 0, 1, 0 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::forward() { return { 0, 0, 1 }; }
	template<typename T>
	Vec3D<T> Vec3D<T>::back() { return { 0, 0, NegativeSign<T>::value }; }


	template<typename T>
	struct Rect2D
	{
		union
		{
			T x;
			T left;
		};
		union
		{
			T y;
			T right;
		};
		union
		{
			T width;
			T z;
			T top;
		};
		union
		{
			T height;
			T w;
			T bottom;
		};

		// getters
		T getWidth() const noexcept { return width; }
		T getHeight() const noexcept { return height; }

		constexpr Rect2D() noexcept : x(0), y(0), width(0), height(0) { }
		constexpr Rect2D(T _x, T _y, T _width, T _height) noexcept : x(_x), y(_y), width(_width), height(_height) { }
		constexpr Rect2D(Vec2D<T> pos, Vec2D<T> size) noexcept : x(pos.x), y(pos.y), width(size.width), height(size.height) { }


		Vec2D<T> getPosition() const noexcept { return { x, y }; }
		Vec2D<T> getSize() const noexcept { return { width, height }; }

		static Rect2D<T> zero();

		// (u + du) - Constraint
		// du + r
		// Constraint: u - r

		// this is being used in calculate constraints between two rects
		Rect2D<T> operator-(const Rect2D<T>& rect) const noexcept
		{
			constexpr auto neg = NegativeSign<T>::value;
			return { x +  neg * rect.x, y + neg * rect.y, width + neg * rect.width, height + neg * rect.height };
		}

		Rect2D<T> operator+(const Rect2D<T>& rect) const noexcept
		{
			return { x + rect.x, y + rect.y, width + rect.width, height + rect.height };
		}

		bool operator ==(const Rect2D<T>& r) noexcept { return (r.getSize() == getSize()) && (r.getPosition() == getPosition()); }
		bool operator !=(const Rect2D<T>& r) noexcept { return !operator==(r); }

		Vec2D<T> getCenter() const noexcept
		{
			return static_cast<Vec2D<f32>>(getPosition()) + static_cast<Vec2D<f32>>(getSize()) * 0.5;
		}

		f32 getRight() const noexcept
		{
			return width + x;
		}

		f32 getLeft() const noexcept
		{
			return x;
		}

		f32 getTop() const noexcept
		{
			return y;
		}

		f32 getBottom() const noexcept
		{
			return height + y;
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

		// returns true, if either of width and height is negative
		// otherwise false.
		bool isInverted() const noexcept
		{
			return ((NegativeSign<T>::value * width) > 0) || ((NegativeSign<T>::value * height) > 0);
		}

		void setTopLeft(Vec2D<T> point) noexcept
		{
			auto bottomRight = getBottomRight();
			x = point.x;
			y = point.y;
			width = bottomRight.x - point.x;
			height = bottomRight.y - point.y;
		}

		void setBottomRight(Vec2D<T> point) noexcept
		{
			width = point.x - x;
			height = point.y - y;
		}

		void setTopRight(Vec2D<T> point) noexcept
		{
			T bottomLeftYCoord = y + height;
			y = point.y;
			width = point.x - x;
			height = bottomLeftYCoord - point.y;
		}

		void setBottomLeft(Vec2D<T> point) noexcept
		{
			T bottomRightXCoord = x + width;
			x = point.x;
			height = point.y - y;
			width = bottomRightXCoord - point.x;
		}

		void extendTopLeft(Vec2D<T> disp) noexcept
		{
			extendTop(disp.y);
			extendLeft(disp.x);
		}

		void extendBottomRight(Vec2D<T> disp) noexcept
		{
			extendBottom(disp.y);
			extendRight(disp.x);
		}

		void extendTopRight(Vec2D<T> disp) noexcept
		{
			extendTop(disp.y);
			extendRight(disp.x);
		}

		void extendBottomLeft(Vec2D<T> disp) noexcept
		{
			extendBottom(disp.y);
			extendLeft(disp.x);
		}

		// +ve values means extend towards left
		void extendLeft(T value) noexcept
		{
			x += value * NegativeSign<T>::value;
			width += value;
		}

		// +ve value means extend towards right
		void extendRight(T value) noexcept
		{
			width += value;
		}

		// +ve value means extend towards top (up direction)
		void extendTop(T value) noexcept
		{
			y += value * NegativeSign<T>::value;
			height += value;
		}

		// +ve value means extend towards bottom (downward direction)
		void extendBottom(T value) noexcept
		{
			height += value;
		}

		bool contains(Vec2D<T> point) const noexcept
		{
			auto tl = getTopLeft();
			auto br = getBottomRight();
			return (point.x > tl.x) && (point.y > tl.y) && (point.x < br.x) && (point.y < br.y);
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
	using RectMargins = Rect2D<T>;

	template<typename T>
	Rect2D<T> Rect2D<T>::zero() { return { 0, 0, 0, 0 }; }

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

	typedef u32 Layer;
	static constexpr Layer InvalidLayer = U32_MAX;
	// Any layer should not exceed this layer
	// Therefore, always create layers by subdividing this value.
	static constexpr Layer MaxLayer = 1000000;

	struct Color3
	{
		u8 r;
		u8 g;
		u8 b;

		constexpr Color3() noexcept : r(255), g(255), b(255) { }
		constexpr Color3(u8 _r, u8 _g, u8 _b) noexcept : r(_r), g(_g), b(_b) { }
		constexpr Color3(u32 hex) noexcept : Color3(BIT32_UNPACK8(hex, 2), BIT32_UNPACK8(hex, 1), BIT32_UNPACK8(hex, 0)) { }

		bool operator ==(Color3& rhs) const noexcept
		{
			return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
		}

		bool operator !=(Color3& rhs) const noexcept
		{
			return !Color3::operator==(rhs);
		}

		static constexpr Color3 red() noexcept { return { 255, 0, 0 }; }
		static constexpr Color3 blue() noexcept { return { 0, 0, 255 }; }
		static constexpr Color3 pink() noexcept { return { 0xff, 0xc0, 0xcb }; }
		static constexpr Color3 brown() noexcept { return { 0x96, 0x4B, 0x00 }; }
		static constexpr Color3 green() noexcept { return { 0, 255, 0 }; }
		static constexpr Color3 white() noexcept { return { 255, 255, 255 }; }
		static constexpr Color3 black() noexcept { return { 0, 0, 0 }; }
		static constexpr Color3 yellow() noexcept { return { 255, 255, 0 }; }
	};

	static CAN_BE_UNUSED_FUNCTION std::ostream& operator <<(std::ostream& stream, Color3 value) noexcept
	{
		stream << "{ " << static_cast<u32>(value.r) << ", " << static_cast<u32>(value.g) << ", " << static_cast<u32>(value.b) << " }";
		return stream;
	}


	struct Color4
	{
		union
		{
			struct
			{
				u8 r;
				u8 g;
				u8 b;
			};
			Color3 color3;
		};
		u8 a;

		constexpr Color4() noexcept : r(255), g(255), b(255), a(255) { }
		constexpr Color4(u8 _r, u8 _g, u8 _b, u8 _a) noexcept : r(_r), g(_g), b(_b), a(_a) { }
		constexpr Color4(u8 _r, u8 _g, u8 _b) noexcept : Color4(_r, _g, _b, 255) { }
		constexpr Color4(u32 hex) noexcept : Color4(BIT32_UNPACK8(hex, 3), BIT32_UNPACK8(hex, 2), BIT32_UNPACK8(hex, 1), BIT32_UNPACK8(hex, 0)) { }

		bool operator ==(Color4& rhs) const noexcept
		{
			return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
		}

		bool operator !=(Color4& rhs) const noexcept
		{
			return !Color4::operator==(rhs);
		}

		Color4& alpha(u8 _alpha) & noexcept { a = _alpha; return *this; }
		Color4&& alpha(u8 _alpha) && noexcept { a = _alpha; return std::move(*this); }

		constexpr Color4 scale(f32 level = 0.5f) noexcept
		{ 
			return { static_cast<u8>(r * level), static_cast<u8>(g * level), static_cast<u8>(b * level), static_cast<u8>(a * level) }; 
		}

		constexpr bool equalsApprox(Color4 rhs) noexcept
		{
			// NOTE: we are using here std::max(<u8 value>, 1) - 1, 
			// and std::min(<u8 value>, 254) + 1 to avoid wrap around in edge cases.
			return com::LiesBetweenInc(r, static_cast<u8>(com::max(rhs.r, 1) - 1), static_cast<u8>(com::min(rhs.r, 254) + 1))
				&& com::LiesBetweenInc(g, static_cast<u8>(com::max(rhs.g, 1) - 1), static_cast<u8>(com::min(rhs.g, 254) + 1))
				&& com::LiesBetweenInc(b, static_cast<u8>(com::max(rhs.b, 1) - 1), static_cast<u8>(com::min(rhs.b, 254) + 1))
				&& com::LiesBetweenInc(a, static_cast<u8>(com::max(rhs.a, 1) - 1), static_cast<u8>(com::min(rhs.a, 254) + 1));
		}

		static constexpr Color4 Lerp(Color4 a, Color4 b, f32 t) noexcept
		{
			return { com::Lerp(a.r, b.r, t), com::Lerp(a.g, b.g, t), com::Lerp(a.b, b.b, t), com::Lerp(a.a, b.a, t) };
		}

		static constexpr Color4 red(f32 level = 1.0f) noexcept
		{ 
			return { static_cast<u8>(255 * level), 0, 0, 255 }; 
		}
		static constexpr Color4 blue(f32 level = 1.0f) noexcept
		{ 
			return { 0, 0, static_cast<u8>(255 * level), 255 }; 
		}
		static constexpr Color4 pink(f32 level = 1.0f) noexcept{ return Color4(0xff, 0xc0, 0xcb, 255).scale(level); }
		static constexpr Color4 brown(f32 level = 1.0f) noexcept { return Color4(0x96, 0x4B, 0x00, 255).scale(level); }
		static constexpr Color4 green(f32 level = 1.0f) noexcept
		{
			return { 0, static_cast<u8>(255 * level), 0, 255 }; 
		}
		static constexpr Color4 white() noexcept
		{
			return Color4::grey(1.0f);
		}
		static constexpr Color4 black() noexcept
		{
			return { 0, 0, 0, 255 }; 
		}
		static constexpr Color4 yellow(f32 level = 1.0f) noexcept
		{
			return { static_cast<u8>(255 * level), static_cast<u8>(255 * level), 0, 255 }; 
		}
		static constexpr Color4 grey(f32 level = 0.5f) noexcept
		{ 
			return { static_cast<u8>(255 * level), static_cast<u8>(255 * level), static_cast<u8>(255 * level), 255 }; 
		}
	};

	static CAN_BE_UNUSED_FUNCTION std::ostream& operator <<(std::ostream& stream, Color4 value) noexcept
	{
		stream << "{ " << static_cast<u32>(value.r) << ", " << static_cast<u32>(value.g) << ", " << static_cast<u32>(value.b) << ", " << static_cast<u32>(value.b) << " }";
		return stream;
	}

	typedef Rect2D<f32> Rect2Df;
	typedef RectMargins<f32> RectMarginsf;
	typedef Vec2D<f32> Vec2Df;
	typedef Vec3D<f32> Vec3Df;
	typedef Rect2D<f32> Vec4Df;

	template<std::integral T>
	struct Vec2DHash
	{
		constexpr std::size_t operator()(const Vec2D<T>& value) const noexcept
		{
			std::size_t hash1 = std::hash<T> { } (value.x);
			std::size_t hash2 = std::hash<T> { } (value.y);
			return hash1 ^ hash2;
		}
	};

	template<std::integral T>
	struct Vec2DEqualTo
	{
		constexpr bool operator()(const Vec2D<T>& value1, const Vec2D<T>& value2) const noexcept
		{
			return value1 == value2;
		}
	};
}
