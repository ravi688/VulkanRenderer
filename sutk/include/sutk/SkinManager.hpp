#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriverObject

namespace SUTK
{
	class Skin
	{

	};

	class SUTK_API SkinManager : public UIDriverObject
	{
	public:
		SkinManager(UIDriver& driver) noexcept;

		Skin& getCurrentSkin() noexcept;
	};
}