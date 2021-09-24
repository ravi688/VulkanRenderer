
/* Documentation:
 * Exception System
 *
 * Version History: 
 * Version 1.0 (Announced)
 * Version 1.1
 */

#include <exception/exception.h>
#include <stdio.h>
#include <stdlib.h>

#include <exception/exception_definitions.h>

#ifndef NO_EXCEPTION
/*Changes: 
 * Version 1.0: __throw
 * Version 1.1: __throw_exception
 */
void __throw_exception(exception_t exception, uint64_t line_no, const char* method_name, const char* file_name)
{
	printf("Exception: %s, thrown at %lu, %s, %s\n", exception, line_no, method_name, file_name);
	exit(0);
}
#endif