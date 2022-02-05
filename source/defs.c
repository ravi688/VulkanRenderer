#include <renderer/defs.h>
#include <buffer.h>

RENDERER_API int __sizeof(SerializedProperty* property)
{
	if(property->is_pointer)
		return sizeof(intptr_t);
	switch(property->type)
	{
		case TYPE_SIGNED_INT8: return sizeof(int8_t); break;
		case TYPE_SIGNED_INT16: return sizeof(int16_t); break;
		case TYPE_SIGNED_INT32: return sizeof(int32_t); break;
		case TYPE_SIGNED_INT64: return sizeof(int64_t); break;
		case TYPE_UNSIGNED_INT8: return sizeof(uint8_t); break;
		case TYPE_UNSIGNED_INT16: return sizeof(uint16_t); break;
		case TYPE_UNSIGNED_INT32: return sizeof(uint32_t); break;
		case TYPE_UNSIGNED_INT64: return sizeof(uint64_t); break;
		case TYPE_FLOAT: return sizeof(float); break;
		case TYPE_DOUBLE: return sizeof(double); break;
		case TYPE_LONG_DOUBLE: return sizeof(long double); break;
		default: throw_error("type not found in _sizeof function ", __LINE__, ""); 
	}
	return 0; //bytes
}
RENDERER_API bool isstorage(const char* string, _storage_specifiers* storage)
{
	if(strlen(string) < strlen("static"))
	{
		*storage = STSP_NONE; 
		return false; 
	}
	else if(strcmp(string, "extern") == 0)
	{
		*storage = STSP_EXTERN; ; 
		return true; 
	}
	else if(strcmp(string, "static") == 0)
	{
		*storage = STSP_STATIC; ; 
		return true;
	} 
	else if(strcmp(string, "register") == 0)
	{
		*storage = STSP_REGISTER;
		return true; 
	}
	else
	{
		*storage = STSP_NONE; 
		return false; 
	}
}

RENDERER_API bool istype(const char* string, _type_specifiers* type)
{
	if(strlen(string) < strlen("int"))
	{	
		*type = TYPE_NONE; 
		return false; 
	}
	if(!strcmp(string, "void"))
	{
		*type = TYPE_VOID; 
		return true;
	}
	if(!strcmp(string, "int8_t"))
	{
		*type = TYPE_SIGNED_INT8; 
		return true; 
	}
	else if(!strcmp(string, "int16_t"))
	{
		*type = TYPE_SIGNED_INT16; 
		return true; 
	}
	else if(!strcmp(string, "int32_t"))
	{
		*type = TYPE_SIGNED_INT32; 
		return true; 
	}
	else if(!strcmp(string, "int64_t"))
	{
		*type = TYPE_SIGNED_INT64; 
		return true; 
	}
	else if(!strcmp(string, "uint8_t"))
	{
		*type = TYPE_UNSIGNED_INT8; 
		return true; 
	}
	else if(!strcmp(string, "uint16_t"))
	{
		*type = TYPE_UNSIGNED_INT16; 
		return true; 
	}
	else if(!strcmp(string, "uint32_t"))
	{
		*type = TYPE_UNSIGNED_INT32; 
		return true; 
	}
	else if(!strcmp(string, "uint64_t"))
	{
		*type = TYPE_UNSIGNED_INT64; 
		return true; 
	}
	else if(!strcmp(string, "float"))
	{
		*type = TYPE_FLOAT; 
		return true; 
	}
	else if(!strcmp(string, "double"))
	{
		*type = TYPE_DOUBLE; 
		return true; 
	}
	else if(!strcmp(string, "long double"))
	{
		*type = TYPE_LONG_DOUBLE; 
		return true; 
	}
	else if(!strcmp(string, "signed") || !strcmp(string, "unsigned") || !strcmp(string, "long"))
	{
		*type = TYPE_INCOMPLETE; 
		return true; 
	}
	else if(!strcmp(string, "char") || !strcmp(string, "signed char"))
	{
		*type = TYPE_SIGNED_INT8; 
		return true; 
	}
	else if(!strcmp(string, "short") || !strcmp(string, "signed short") || !strcmp(string, "short int") || !strcmp(string,"signed short int"))
	{
		*type = TYPE_SIGNED_INT16; 
		return true; 
	}
	else if(!strcmp(string, "int") || !strcmp(string, "signed int") || !strcmp(string, "long") || !strcmp(string, "long int"))
	{
		*type = TYPE_SIGNED_INT32; 
		return true; 
	}
	else if(!strcmp(string, "long long int") || !strcmp(string, "long long"))
	{
		*type = TYPE_SIGNED_INT64; 
		return true; 
	}
	else if(!strcmp(string, "unsigned char"))
	{
		*type = TYPE_UNSIGNED_INT8; 
		return true; 
	}
	else if(!strcmp(string, "unsigned short") || !strcmp(string, "unsigned short int"))
	{
		*type = TYPE_UNSIGNED_INT16; 
		return true; 
	}
	else if(!strcmp(string, "unsigned int") || !strcmp(string, "unsigned long") || !strcmp(string, "unsigned long int"))
	{
		*type = TYPE_UNSIGNED_INT32; 
		return true; 
	}
	else if(!strcmp(string, "unsigned long long") || !strcmp(string, "unsigned long long int"))
	{
		*type = TYPE_UNSIGNED_INT64; 
		return true; 
	}
}

RENDERER_API char* defs_load_text_from_file(const char* file_name)
{
	FILE* file = fopen(file_name, "r");
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

RENDERER_API char* defs_load_text_from_file_exclude_comments(const char* file_name)
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