#ifndef __PRINT_H__
#define __PRINT_H__

#define PRINT_FRIST_VERSION (1 << 0)

#define PRINT_VERSION (1 << 0)


/*
	Version History: 

	1.0 (this)

	Dependencies: 
	stdint.h
*/

/*macro switches to be defined here*/
#include "config.h"

#include <stdint.h>

typedef void (*printer_t)(void* value);


#define PRIVATE static

#ifdef PRINT_REQUIRE_ARRAY_PRINT
#ifndef PRINT_ARRAY_PRINT_REQUIREMENT_SATIFIED
#define PRINT_ARRAY_PRINT_REQUIREMENT_SATIFIED
#define print_array(array, element_size, num, __print__) __print_array(array, element_size, num, (printer_t)(__print__))

PRIVATE void __print_array(void* array, uint32_t element_size, uint32_t num, printer_t __print__) 
{ 
	uint8_t* bytes = (uint8_t*)array; 
	uint32_t offset = 0;
	for(int i = 0; i < num; i++, offset += element_size) 
		__print__(array + offset);
}

#endif
#endif

#endif