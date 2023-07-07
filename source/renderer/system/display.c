#include <renderer/system/display.h>
#include <math.h>

RENDERER_API iextent2d_t display_get_resolution()
{
	return iextent2d(1920, 1080);
}

RENDERER_API iextent2d_t display_get_dpi()
{
	AUTO res = display_get_resolution();
	AUTO inches = display_get_inches();
	return iextent2d(res.x / inches.x , res.y / inches.y);
}

RENDERER_API extent2d_t display_get_inches()
{
	f32 diagonal = 15.6f; // inches
	AUTO res = display_get_resolution();
	f32 angle = atan2(res.y, res.x);
	return extent2d(diagonal * cos(angle), diagonal * sin(angle));
}
