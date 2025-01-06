#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	namespace Constants
	{
		namespace Defaults
		{
			namespace Button
			{
				static constexpr Vec2Df Size { 3.0f, 0.8f };
				static constexpr f32 LeftMargin { 0.2f };
				static constexpr f32 RightMargin { 0.2f };

				static constexpr Vec2Df TotalSize { Size.getWidth() + LeftMargin + RightMargin, Size.getHeight() };
			}

			namespace DynamicVListContainer
			{
				static constexpr f32 Width { 2.0f };
				static constexpr f32 Height { 5.0f };
			}

			namespace ButtonListView
			{
				static constexpr f32 ButtonHeight { 0.8f };
			}

			namespace ToggleButton
			{
				static constexpr Vec2Df Size { 4.0f, 0.8f };
				static constexpr f32 CircleThickness { 0.1f };
				static constexpr f32 CircleTopBottomMargin { 0.2f };
				static constexpr f32 CircleLabelSpacing { 0.1f };
			}

			namespace Notebook
			{
				namespace Tab
				{
					static constexpr Color4 SelectColor { 0x749bafffu };
					static constexpr f32 CloseButtonSize { 0.5f };
					static constexpr f32 LeftMargin { 0.2f };
					static constexpr f32 RightMargin { 0.2f };
					static constexpr f32 LabelCloseSpacing { };
					static constexpr f32 Height { 0.8f }; // 0.8 centimeters
				}
			}
		}
	}
}