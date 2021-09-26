
#include <stdarg.h>
#include <stdio.h>

#include <string/string.h>
#include <garbage_collector/garbage_collector.h>

const char* string(uint32_t length, const char* format, ...)
{
	char* buffer = GC_ALLOC(length);
	va_list args; 
	va_start(args, format);
	vsprintf(buffer, format, args); 
	va_end(args);
	return buffer;
}

const char* const* string_array(uint32_t count, ...)
{
	const char* *buffer = GC_NEWV(const char*, count);
	va_list args; 
	va_start(args, count);
	uint32_t i = 0;
	while(i < count)
	{
		const char* str = va_arg(args, const char*);
		buffer[i] = string(strlen(str), str);
		i++;
	}
	va_end(args);
	return buffer;
}