
#include <renderer/time.h>


#ifdef _USE_32BIT_TIME_T
#		undef _USE_32BIT_TIME_T
#endif
#include <sys/time.h>
#include <stdio.h>

time_handle_t time_get_handle()
{
	return clock();
}

float time_get_seconds(time_handle_t handle)
{
	return (float)(clock() - handle) / CLOCKS_PER_SEC;
}

uint32_t time_get_milliseconds(time_handle_t handle)
{
	return (float)(clock() - handle) * (1000 / CLOCKS_PER_SEC);
}


float time_get_delta_time(time_handle_t* handle)
{
	float delta_time = time_get_seconds(*handle);
	*handle = time_get_handle();
	return delta_time;
}