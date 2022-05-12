
#pragma once

#include <renderer/defines.h>
#include <renderer/buffer.h>

typedef struct file_info_t
{
	char* path;
	u64 size;
} file_info_t;


typedef enum directory_search_type_t
{
	DIRECTORY_SERACH_TYPE_DEFAULT = 0,
	DIRECTORY_SEARCH_TYPE_RECURSIVE
} directory_search_type_t;

/*
	description: searches a list of files with filter in a directory
	params:
		directory: directory to search in
		filter: filter to be applied
		search_type: type of the search
	returns:
		BUFFER, list of file_info_t objects
 */
BUFFER directory_search_files(const char* directory, const char* filter, directory_search_type_t search_type);

