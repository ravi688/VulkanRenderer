#pragma once

#include <sge/system/display.h>

/* returns display resolution in pixels { number of pixels across width, number of pixels across height } */
SGE_API iextent2d_t display_get_resolution();
/* returns aspect ratio in terms of separated { nominator (represents "primitive or reduced or factored out" width), denominator "primitive..." height }
 * for example, { 16, 9 } */
SGE_API iextent2d_t display_get_aspect_ratio();
/* returns diagonal size of the display in inches */
SGE_API f32 display_get_diagonal_size();


/* BELOW FUNCTIONS ARE DERIVED FROM THE ABOVE FUNCTIONS */

/* returns number of pixels per inch across width and height as { width_dpi, height_dpi } */
SGE_API extent2d_t display_get_dpi();
/* returns physical width and height of display in inches */
SGE_API extent2d_t display_get_inches();


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
	};
}