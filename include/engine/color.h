#ifndef __COLOR_H__
#define __COLOR_H__

typedef struct color_t
{
	float r;
	float g;
	float b;
	float a;
} color_t;

#define color_assign(color_ptr, _r, _g, _b, _a) (color_ptr)->r = (_r), (color_ptr)->g = (_g), (color_ptr)->b = (_b), (color_ptr)->a = (_a)

#endif/*__COLOR_H__*/
