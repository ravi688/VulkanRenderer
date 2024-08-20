#include <sutk/SDF.hpp>
#include <algorithm>

namespace SUTK
{
	template<>
	SDF::ParameterType SDF::GetParameterType<f32>() { return SDF::ParameterType::Float32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<Vec2D<f32>>() { return SDF::ParameterType::Vector2Float32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<u32>() { return SDF::ParameterType::Unsigned32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<Vec2D<u32>>() { return SDF::ParameterType::Vector2Unsigned32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<s32>() { return SDF::ParameterType::Signed32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<Vec2D<s32>>() { return SDF::ParameterType::Vector2Signed32; }
	template<>
	SDF::ParameterType SDF::GetParameterType<InvariantWidth>() { return SDF::ParameterType::InvariantWidth; }
	template<>
	SDF::ParameterType SDF::GetParameterType<InvariantHeight>() { return SDF::ParameterType::InvariantHeight; }
	template<>
	SDF::ParameterType SDF::GetParameterType<InvariantSize>() { return SDF::ParameterType::InvariantSize; }

	void SDF::setCode(const char* str) noexcept
	{
		m_code.clear();
		m_code.append(str);
		m_isCodeModified = true;
	}
	void SDF::setCode(const std::string& str) noexcept
	{
		setCode(str.c_str());
	}
	SDF::ParameterID SDF::getParameterID(const char* str) noexcept
	{
		auto it = std::find_if(m_params.begin(), m_params.end(), [str](ParameterData& value) { return value.name.compare(str) == 0; });
		_com_assert(it != m_params.end());
		return static_cast<ParameterID>(std::distance(m_params.begin(), it));
	}

	void SDF::resetModificationFlags() noexcept
	{
		m_isCodeModified = false;
		m_isParamsModified = false;
		for(ParameterData& data : m_params)
			data.isModified = false;
	}
}
