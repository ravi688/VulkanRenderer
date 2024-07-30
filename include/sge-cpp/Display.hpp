#pragma once

#include <sge/system/display.h>

namespace SGE
{
	class Display
	{
	public:
		static iextent2d_t GetResolution() noexcept { return display_get_resolution(); }
		static iextent2d_t GetAspectRatio() noexcept { return display_get_aspect_ratio(); }
		static f32 GetDiagonalSize() noexcept { return display_get_diagonal_size(); }
		static extent2d_t GetDPI() noexcept { return display_get_dpi(); }
		static extent2d_t GetInches() noexcept { return display_get_inches(); }
		static extent2d_t ConvertInchesToPixels(extent2d_t inches) { return display_convert_inches_to_pixels(inches); }
		static extent2d_t ConvertPixelsToInches(extent2d_t pixels) { return display_convert_pixels_to_inches(pixels); }
	};
}