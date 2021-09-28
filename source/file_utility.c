
#include <utilities/file_utility.h>

#include <buffer.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>


static char* load_file(const char* file_name, const char* mode);

char* load_binary_from_file(const char* file_name)
{
	return load_file(file_name, "rb");
}

char* load_text_from_file(const char* file_name)
{
 	return load_file(file_name, "r");
}

static char* load_file(const char* file_name, const char* mode)
{
	FILE* file = fopen(file_name, mode);
	if(file == NULL) 
	{
		#ifdef DEBUG
		printf("[Error] File loading failed, file_name: \"%s\", from load_text_from_file\n", file_name);
		#endif
		return NULL; 
	}
	char bytes[BUF_BUFFER_OBJECT_SIZE]; 
	BUFFER* memory_buffer = BUFcreate_object(bytes); 
	BUFcreate(memory_buffer, sizeof(char), 1, 0);
	BUFbind(memory_buffer); 
	char ch = getc(file); 
	while(ch != EOF)
	{
		BUFpush(&ch); 
		ch = getc(file); 
	}
	BUFpush_char(0); 
	BUFfit();  
	fclose(file);
	return (char*)BUFget_ptr(); 
}

char* load_text_from_file_exclude_comments(const char* file_name)
{
	FILE* file = fopen(file_name, "r"); 
	if(file == NULL)
	{
		#ifdef DEBUG
		printf("[Error] File loading failed, file_name: \"%s\", from load_text_from_file_exclude_comments\n", file_name);
		#endif
		return NULL;
	}

	char bytes[BUF_BUFFER_OBJECT_SIZE];
	BUFFER* memory_buffer = BUFcreate_object(bytes);
	BUFcreate(memory_buffer, sizeof(char), 1, 0);
	BUFbind(memory_buffer);
	char ch = getc(file);
	char previous_ch = ch;
	bool single_line_comment_begin = false; 
	bool multiple_line_comment_begin = false;

	while(ch != EOF)
	{
		if(!single_line_comment_begin && !multiple_line_comment_begin)
			BUFpush(&ch);
		previous_ch = ch;  
		ch = getc(file);

		if(multiple_line_comment_begin && (previous_ch == '*') && (ch == '/'))
		{
			multiple_line_comment_begin = false;
			BUFpop(NULL);
			ch = getc(file); 
			ch = getc(file);
			continue;
		}
		else if(single_line_comment_begin && (ch == '\n'))
		{
			single_line_comment_begin = false; 
			ch = getc(file);
			continue;
		}

		if(!multiple_line_comment_begin && !single_line_comment_begin && (previous_ch == '/') && (ch == '/'))
		{
			single_line_comment_begin = true;
			BUFpop(NULL);
			ch = getc(file); 
			continue;
		}
		if(!single_line_comment_begin && !multiple_line_comment_begin && (previous_ch == '/') && (ch == '*'))
		{
			multiple_line_comment_begin = true;
			ch = getc(file); 
			continue;
		}
	}
	BUFpush_char(0); 
	BUFfit(); 
	fclose(file); 
	return (char*)BUFget_ptr();
}