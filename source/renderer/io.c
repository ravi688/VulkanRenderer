
#include <renderer/io.h>

BUFFER directory_search_files(const char* directory, const char* filter, directory_search_type_t search_type)
{
	BUFFER files = buf_create(sizeof(file_info_t), 0, 0);
	return files;
}

