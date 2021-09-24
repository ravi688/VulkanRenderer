

#include "buffer_test.h"
#include <stdio.h>
#include <stdlib.h>

bool equal_ptr(void* ptr1, void* ptr2)
{
	return (*(char**)ptr1) == (*(char**)ptr2);
}

void print_ptr(void* ptr)
{
	printf("%d ", *(int*)ptr); 
}

BUFFER* read_file(const char* file_name)
{
    BUFpush_binded();
    FILE* file = fopen(file_name, "r");
    BUF_ASSERT(file != NULL, "file opening failed!", NULL);
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file); 
    fseek(file, 0, SEEK_SET);
    BUFFER* read_buffer = BUFcreate(NULL, sizeof(char), size, 1); 
    BUFbind(read_buffer);
    BUFset_element_count(size);
    char null_value = 0;
    BUFset_offset_bytes(&null_value); 
    fread(BUFget_ptr(), sizeof(char), BUFget_element_count(), file);
    fclose(file);
    BUFpop_binded();
    return read_buffer;
}

bool cmp_float(void* f1, void* f2) { return (*(float*)f1) == (*(float*)f2); }

int main()
{
	bool result = BUFstart_default_testing();
	if(!result)
		puts("One or More Tests are Failed");
	else 
		puts("Passed");
	puts("\n");

	BUFFER* buffer = BUFcreate(NULL, sizeof(float), 0, 0); 
	BUFbind(buffer);
	float value = 100;
	BUFpush(&value); value += 1;
	BUFpush(&value); value += 1;
	BUFpush(&value); value += 1;
	BUFpush(&value); value += 1;
	BUFpush(&value); value += 1;
	BUFpush(&value);

	value = 101;
	if(!BUFremove(&value, cmp_float))
		printf("Not found: %f\n", value);
	// value = 102;
	// if(!BUFremove(&value, cmp_float))
	// 	printf("Not found: %f\n", value);

	BUFlog();
	float* array = BUFget_ptr();

	for(int i = 0; i < BUFget_element_count(); i++)
		printf("%f ", BUFget_value_at_typeof(float, i));
	puts("");

	BUFFER* clone = BUFget_clone();
	BUFbind(clone); 

	printf("Cloned: "); 

	for(int i = 0; i < BUFget_element_count(); i++)
		printf("%f ", BUFget_value_at_typeof(float, i));
	puts("");

	BUFfree();
	BUFbind(buffer); 
	BUFfree();

	BUFFER* text = read_file("Hello.txt");
	BUFbind(text);
	puts(BUFget_ptr());
	BUFlog();
	BUFfree();
	return 0;
}