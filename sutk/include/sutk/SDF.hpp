#pragma once

#include <sutk/defines.hpp>

#include <variant> // for std::variant (C++17)
#include <string> // for std::string
#include <vector> // for std::vector
#include <variant> // for std::variant (C++17)

#include <common/assert.h> // for _com_assert

namespace SUTK
{
	struct InvariantLength
	{
		f32 value;
		constexpr InvariantLength() noexcept : value(0) { }
		constexpr InvariantLength(f32 _value) noexcept : value(_value) { }
		constexpr operator f32() const noexcept { return value; }
		InvariantLength constexpr operator =(f32 _value) noexcept { value = _value; return *this; }
	};

	struct InvariantWidth : public InvariantLength
	{
		constexpr InvariantWidth() noexcept : InvariantLength() { }
		constexpr InvariantWidth(f32 _value) noexcept : InvariantLength(_value) { }
		constexpr operator f32() const noexcept { return value; }
		InvariantWidth constexpr operator =(f32 _value) noexcept { value = _value; return *this; }
	};

	struct InvariantHeight : public InvariantLength
	{
		constexpr InvariantHeight() noexcept : InvariantLength() { }
		constexpr InvariantHeight(f32 _value) noexcept : InvariantLength(_value) { }
		constexpr operator f32() const noexcept { return value; }
		InvariantHeight constexpr operator =(f32 _value) noexcept { value = _value; return *this; }
	};

	struct InvariantSize
	{
		InvariantWidth width;
		InvariantHeight height;

		constexpr InvariantSize() noexcept : width(0), height(0) { }
		constexpr InvariantSize(f32 _width, f32 _height) noexcept : width(_width), height(_height) { }
		constexpr InvariantSize(Vec2D<f32> size) noexcept : width(size.x), height(size.y) { }
	};

	class SUTK_API SDF
	{
	public:
		typedef u32 ParameterID;
		typedef std::variant<f32, Vec2Df, u32, Vec2D<u32>, s32, Vec2D<s32>, InvariantWidth, InvariantHeight, InvariantSize> ParameterValue;
		enum class ParameterType : u8
		{
			Float32,
			Vector2Float32,
			Unsigned32,
			Vector2Unsigned32,
			Signed32,
			Vector2Signed32,
			InvariantWidth,
			InvariantHeight,
			InvariantSize
		};
		struct ParameterData
		{
			std::string name;
			ParameterType type;
			ParameterValue value;
			bool isModified;
		};

		template<typename T>
		static ParameterType GetParameterType();

	private:
		std::string m_code;
		bool m_isCodeModified;
		std::vector<ParameterData> m_params;
		bool m_isParamsModified;

	public:
		SDF() noexcept : m_isCodeModified(false) { }

		const std::vector<ParameterData>& getParams() const noexcept { return m_params; }

		void setCode(const char* str) noexcept;
		void setCode(const std::string& str) noexcept;
		const std::string& getCode() const noexcept { return const_cast<SDF*>(this)->getCode(); }
		std::string& getCode() noexcept { return m_code; }

		template<typename T>
		ParameterID addParameter(const char* str, T defaultValue = { }) noexcept;
		ParameterID getParameterID(const char* str) noexcept;
		template<typename T>
		void setParameterValue(ParameterID id, T value) noexcept;
		template<typename T>
		void setParameterValue(const char* str, T value) noexcept;

		bool isCodeModified() const noexcept { return m_isCodeModified; }
		bool isParamsModified() const noexcept { return m_isParamsModified; }

		void resetModificationFlags() noexcept;
	};

	template<typename T>
	SDF::ParameterID SDF::addParameter(const char* str, T defaultValue) noexcept
	{
		SDF::ParameterID id = static_cast<SDF::ParameterID>(m_params.size());
		m_params.push_back({ std::string(str), SDF::GetParameterType<T>(), defaultValue, true });
		m_isParamsModified = true;
		return id;
	}

	template<typename T>
	void SDF::setParameterValue(ParameterID id, T value) noexcept
	{
		_com_assert(id < static_cast<u32>(m_params.size()));
		auto& param = m_params[id];
		param.value = value;
		param.type = SDF::GetParameterType<T>();
		param.isModified = true;
		m_isParamsModified = true;
	}

	template<typename T>
	void SDF::setParameterValue(const char* str, T value) noexcept
	{
		setParameterValue<T>(getParameterID(str), value);
	}
}
