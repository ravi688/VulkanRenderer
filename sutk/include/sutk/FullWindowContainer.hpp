#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class FullWindowContainer : public Container
	{
	private:

		friend class UIDriver;

		FullWindowContainer(UIDriver& driver) noexcept;

	};
}