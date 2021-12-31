#include <renderer/Serialization.h>

static BUFFER* serialized_structs = NULL;
static const char* source_buffer = NULL; 
static char* freeable_source_buffer = NULL; 

//static SerializedStruct* serialized_struct_get(const char* name); 
static void __initialize_local_property_addresses(SerializedStruct* serialized_struct); 
static SerializedStruct __serialize_properties(const char* struct_name, char* string_buffer);
static SerializedProperty* __serialized_struct_get_property(SerializedStruct* serialized_struct, const char* property_name); 

void serialized_property_print(SerializedProperty* property)
{
	const char* format_string = 
	"	Property name: %s\n"
	"	         type: %d\n"
	"	      storage: %d\n"
	"	      address: %d\n"
	"	      pointer: %d\n"
	"	        const: %d\n";

	printf(format_string,
		property->name,
		property->type,
		property->storage,
		property->address,
		property->is_pointer,
		property->is_const); 
}

void serialized_struct_print(const char* name)
{
	BUFFER* previous_buffer = BUFget_binded_buffer();
	SerializedStruct* serialized_struct = serialized_struct_get(name); 
	if(serialized_struct == NULL)
	{
		#ifdef DEBUG
		printf("[ERROR] You are trying to print unserialized struct or unexisting struct \"%s\"\n", name); 
		#endif
		return; 
	}

	BUFbind(serialized_struct->properties); 

	printf("Struct name: %s\n"
		   "Number of properties: %d\n",
		   serialized_struct->name,
		   BUFget_element_count()
		); 

	for(int i =0 ;i < BUFget_element_count(); i++)
	{
		SerializedProperty* property = (SerializedProperty*)BUFgetptr_at(i);
		serialized_property_print(property);  
	}
	BUFbind(previous_buffer);
}

void* instantiate_object(const char* serialized_struct_name)
{
	SerializedStruct* object_definition = serialized_struct_get(serialized_struct_name); 
	if(object_definition == NULL)
		throw_error("There is no Serialized Struct present, please first serialized the struct ", __LINE__,  serialized_struct_name); 
	void* allocated_memory = malloc((object_definition->size) * sizeof(uint8_t)); 

	#ifdef DEBUG
	printf("[LOG] Object instantiated successfully of type \"%s\" and size \"%d\" bytse\n", object_definition->name, object_definition->size);
	#endif
	return allocated_memory; 
}
void set_serialization_source_buffer(const char* _buffer)
{
	if(freeable_source_buffer != NULL)
	{	
		free(freeable_source_buffer); 
		freeable_source_buffer = NULL; 
	}
	source_buffer = _buffer;  
}
void load_serialization_source_file(const char* file_name)
{
	if(freeable_source_buffer != NULL)
		free(freeable_source_buffer);
	freeable_source_buffer = defs_load_text_from_file_exclude_comments(file_name); 
	if(freeable_source_buffer != NULL)
	{
		#ifdef DEBUG
		printf("[LOG] serialization source file loaded successfully, file_name: \"%s\"\n", file_name);
		#endif
	}
	source_buffer = freeable_source_buffer;
} 

void destroy_serialization_data()
{
	BUFFER* previous_buffer = BUFget_binded_buffer();
	if(serialized_structs != NULL)
	{
		BUFbind(serialized_structs); 
		for(int i = 0; i < BUFget_element_count(); i++)
		{
			SerializedStruct* serialized_struct = (SerializedStruct*)BUFgetptr_at(i); 
			BUFbind(serialized_struct->properties); 
			BUFfree(); 
			BUFbind(serialized_structs); 
		}
		BUFfree();
	}
	if(freeable_source_buffer != NULL)
		free(freeable_source_buffer); 

	BUFbind(previous_buffer);
	#ifdef DEBUG
	printf("[LOG] serialization data successfully destroyed\n"); 
	#endif
}

void struct_serialize(const char* struct_name)
{
		if(source_buffer == NULL)
			throw_error("Please load a source file first for serializing any object", __LINE__, "");
		BUFFER* previous_buffer = BUFget_binded_buffer();
		if(serialized_struct_get(struct_name) != NULL)
		{
			#ifdef DEBUG
			puts("Already serialized"); 
			#endif
			return; 
		}
		const char* search_buffer = source_buffer; 
		int iteration_count = 0; 
start_search: 
		++iteration_count; 
		if(iteration_count > 10)
			throw_error("searching failed", __LINE__, ""); 
	
		char* struct_search = strstr(search_buffer, "struct"); 
	 	
	 	if(struct_search != NULL)
	 	{
	 		INCREMENT_CHAR_PTR(struct_search, strlen("struct")); 

	 		while(isspace(*struct_search))
	 			INCREMENT_CHAR_PTR(struct_search, 1);
	 	
	 		if(*struct_search == '{')
	 		{
	 			++struct_search; 
	 			char* close_brace_search = struct_search; 
	 			while(*close_brace_search != '}')
	 			{
	 				// if(strstr(close_brace_search, "struct") != NULL)
	 				// {
	 				// 	search_buffer = close_brace_search + strlen("struct"); 
	 				// 	if(strchr(close_brace_search, 0) > search_buffer)
	 				// 		GOTO(start_search);
	 				// 	else throw_error("struct definition is not found for", __LINE__, struct_name);
	 				// }
	 				INCREMENT_CHAR_PTR(close_brace_search, 1);  
	 			}  
				INCREMENT_CHAR_PTR(close_brace_search, 1); 
	 			while(isspace(*close_brace_search))
	 				INCREMENT_CHAR_PTR(close_brace_search, 1);

	 			if(strncmp(close_brace_search, struct_name, strlen(struct_name)) != 0)
	 			{
	 				while((*close_brace_search != 0) && !isspace(*close_brace_search) && !(*close_brace_search == ';'))
	 					INCREMENT_CHAR_PTR(close_brace_search, 1);
	 				if(*close_brace_search == 0)
	 					throw_error("struct definition is not found for", __LINE__, struct_name); 
	 				search_buffer = close_brace_search; 
	 				GOTO(start_search); 
	 			}

	 			while(isspace(*struct_search))
	 				INCREMENT_CHAR_PTR(struct_search, 1);

	 			if(*struct_search == 0)
	 				throw_error("structure definition incomplete for", __LINE__, struct_name); 

	 			//parse all the properties here;
				SerializedStruct serialized_struct = __serialize_properties(struct_name, struct_search);  
				__initialize_local_property_addresses(&serialized_struct);
				BUFbind(serialized_structs); 
				BUFpush(&serialized_struct); 

	 		}
	 		else if(strncmp(struct_search, struct_name, strlen(struct_name)) == 0)
	 		{
	 		
	 			INCREMENT_CHAR_PTR(struct_search, strlen(struct_name)); 
	 		
	 			while(isspace(*struct_search))
	 				INCREMENT_CHAR_PTR(struct_search, 1); 
	 		
	 			if(*struct_search == 0)
	 				throw_error("open brace not found for the struct definition for", __LINE__, struct_name);
	 		
	 			if(*struct_search != '{')
	 			{
	 				/*
	 				Homework: 
	 				Try to implement your own exception and error handling system.
	 				and also the errors and exceptions should be considered like objects, we can instantiation and then fire whenever we want.
	 				*/
	 				search_buffer = struct_search; 
	 				GOTO(start_search);
	 			} 
	 		
	 			INCREMENT_CHAR_PTR(struct_search, 1);
	 			char* close_brace_search = struct_search; 
	 			while(*close_brace_search != '}')
	 			{
	 				// if(strstr(close_brace_search, "struct") != 0)
	 				// {
	 				// 	search_buffer = strstr(close_brace_search, "struct") + strlen("struct"); 
	 				// 	if(strchr(close_brace_search, 0) > search_buffer)
	 				// 		GOTO(start_search);
	 				// 	else 
	 				// 		throw_error("closing brace not found for the struct definition for", __LINE__, struct_name); 
	 				// }
	 			    INCREMENT_CHAR_PTR(close_brace_search, 1); 
	 			}
	 		
	 		 
	 			while(isspace(*struct_search))
	 				INCREMENT_CHAR_PTR(struct_search, 1); 
 			
 				//parse all the properties; 
	 		  	SerializedStruct serialized_struct =  __serialize_properties(struct_name, struct_search); 
	 		  	__initialize_local_property_addresses(&serialized_struct); 
	 		  	BUFbind(serialized_structs); 
	 		  	BUFpush(&serialized_struct); 
	 		  
	 		}
	 		else
	 		{
	 			 while(*struct_search != '{')
	 				 	INCREMENT_CHAR_PTR(struct_search, 1); 
	 			INCREMENT_CHAR_PTR(struct_search, 1); 
				search_buffer = struct_search;
				GOTO(start_search);
			}
	 	}
	 	else
	 	{
	 		 #ifdef DEBUG
	 		 printf("Iteration count: %d\n" , iteration_count); 
	 		 #endif
	 	 	 throw_error("no struct definition found in the source file", __LINE__, __FILE__);
	 	}  

	 	BUFbind(previous_buffer);
}

void* serialized_struct_get_property_value(const char* struct_name, const char* property_name, void* object_ptr)
{
	SerializedStruct* object_definition = serialized_struct_get(struct_name); 
	if(object_definition == NULL)
		throw_error("No serialized struct definition is found named: ", __LINE__, struct_name); 

	SerializedProperty* property = __serialized_struct_get_property(object_definition, property_name); 
	if(property->address == -1)
	{
		printf("[ERROR] You are trying to get value from an invalid property \"%s\", exception thrown at serialized_struct_get_property_value()\n", property->name); 
		return NULL; 
	}
	void* shifted_pointer = (void*)(property->address + (intptr_t)object_ptr);
	return shifted_pointer; 
}

void* serialized_property_get_value(SerializedProperty* property)
{
	if(property->address == -1)
	{
		printf("[ERROR] You are trying to get value from an invalid property \"%s\", exception thrown at serialized_property_get_value()\n", property->name);
		return NULL; 
	}
	return (void*)(property->address);
}

void serialized_struct_set_property_value(const char* struct_name, const char* property_name, void* value, void* object_ptr)
{
	SerializedStruct* object_definition = serialized_struct_get(struct_name); 
	if(object_definition == NULL)
		throw_error("No serialized struct definition is found named: ", __LINE__, struct_name); 

	SerializedProperty* property = __serialized_struct_get_property(object_definition, property_name); 
	memcpy((uint8_t*)(property->address + (intptr_t)object_ptr), (uint8_t*)value, _sizeof(property)); 
}

void serialized_property_set_value(SerializedProperty* property, void* value)
{
	if(property->address == -1)
	{
		printf("[ERROR] You are trying to set a value to an invalid property \"%s\", exception thrown at serialized_property_set_value()\n", property->name);
		return; 
	}
	memcpy((uint8_t*)(property->address), (uint8_t*)value, _sizeof(property)); 
}

SerializedProperty serialized_struct_get_property(const char* struct_name,const char* property_name, void* object_ptr)
{
	BUFFER* previous_buffer = BUFget_binded_buffer();
	SerializedProperty serialized_property;
	SerializedStruct* serialized_struct = serialized_struct_get(struct_name); 
	if(serialized_struct == NULL)
	{
		printf("[ERROR] No struct has been serialized of name \"%s\", exception thrown at 'serialized_struct_get_property()'\n", struct_name); 
		serialized_property.address = -1;
		serialized_property.type = TYPE_NONE; 
		serialized_property.storage = STSP_NONE;
		strcpy(serialized_property.name,property_name);
		serialized_property.is_const = false; 
		serialized_property.is_pointer = false;
		BUFbind(previous_buffer);
		return serialized_property;
	}
	BUFbind(serialized_struct->properties); 
 	 
	for(int i =0 ; i < BUFget_element_count(); i++)
	{
		if(strcmp(((SerializedProperty*)BUFgetptr_at(i))->name, property_name) == 0)
		{
			BUFget_at(i, &serialized_property);
			serialized_property.address +=  (intptr_t)object_ptr; 
			BUFbind(previous_buffer);
			return serialized_property;
		}
	//	else printf("Property name: %s\n", ((SerializedProperty*)BUFgetptr_at(i))->name);
	}
	throw_error("no propery definition found named as", __LINE__, property_name); 
}

SerializedStruct* serialized_struct_get(const char* name)
{
	if(serialized_structs == NULL)
		serialized_structs = BUFcreate(NULL, sizeof(SerializedStruct), 1, 0);

	BUFFER* previous_buffer = BUFget_binded_buffer();
	BUFbind(serialized_structs); 

	for(int i = 0; i < BUFget_element_count(); i++)
	{
		SerializedStruct* serialized_struct = (SerializedStruct*)BUFgetptr_at(i);
		if(!strcmp(serialized_struct->name, name))
			return serialized_struct;
	}
	BUFbind(previous_buffer);
	return NULL; 
}

static SerializedProperty* __serialized_struct_get_property(SerializedStruct* serialized_struct, const char* property_name)
{
	BUFFER* previous_buffer = BUFget_binded_buffer();
	BUFbind(serialized_struct->properties); 
	for(int i = 0; i < BUFget_element_count(); i++)
		if(strcmp(((SerializedProperty*)BUFgetptr_at(i))->name, property_name) == 0)
			return (SerializedProperty*)BUFgetptr_at(i); 
	BUFbind(previous_buffer);
	return NULL; 
}

static void __initialize_local_property_addresses(SerializedStruct* serialized_struct)
{
	BUFFER* previous_buffer = BUFget_binded_buffer();
	BUFbind(serialized_struct->properties); 
	int byte_count = 0; 
	int word_length = 8; //bytes
	int word_count = 1;
	for(int i = 0; i < BUFget_element_count(); i++)
	{
		SerializedProperty* property = (SerializedProperty*)BUFgetptr_at(i); 
		if((byte_count + _sizeof(property)) > (word_count * word_length))
		{
			byte_count = word_count * word_length;
			++word_count;
		}
		property->address = byte_count;
		byte_count += _sizeof(property);
	}
	serialized_struct->size = word_count * word_length; 
	BUFbind(previous_buffer);
} 

static SerializedStruct  __serialize_properties(const char* struct_name, char* _buffer)
{
	SerializedStruct serialized_struct; 
	uint8_t bytes[BUF_BUFFER_OBJECT_SIZE]; 
	BUFFER* properties = NULL; 
	BUFFER* previous_buffer = BUFget_binded_buffer();
	char string_buffer[strlen(_buffer) + 1]; 
	strcpy(string_buffer, _buffer);

	if(strlen(string_buffer) > 2)
		properties = BUFcreate(NULL, sizeof(SerializedProperty), 1, 0); 
	else 
		throw_error("string_buffer is not a valid string to parse any kind of property", __LINE__, "exception at __serialize_properties"); 

	char* token = strtok(string_buffer, " \t\n");
	_storage_specifiers storage; 
	_type_specifiers type; 
	bool storage_checked = false; 
	bool type_checked = false; 
	SerializedProperty property; 
	property.is_const = false; 
	property.is_pointer = false;
	strcpy(property.name, "\0"); 
	BUFbind(properties); 
	while(token != NULL)
	{ 
		if(strstr(token, "}") != NULL)
		{
			if(BUFget_element_count() == 0)
			{
				BUFfree(); 
				serialized_struct.is_valid = false;
				serialized_struct.properties = NULL; 
				strcpy(serialized_struct.name,"\0"); 
				#ifdef DEBUG
				printf("[Warning]: serialized struct \"%s\" hasn't any property, it's empty\n", struct_name); 
				#endif
			}
			else 
			{
				serialized_struct.is_valid = true; 
				strcpy(serialized_struct.name, struct_name); 
				serialized_struct.properties = properties; 
				#ifdef DEBUG 
				printf("[LOG]: struct \"%s\" is serialized successfully\n", struct_name); 
				#endif
			}
			break;
		}
		if((!property.is_const) && (strcmp(token, "const") == 0))
			property.is_const = true; 
		else
		if((!property.is_pointer) && ((strcmp(token,"*") == 0)))
			property.is_pointer = true; 
		else
		if(!storage_checked && !type_checked && (isstorage(token, &storage) == true))
		{ 
			storage_checked = true; 
			property.storage = storage; 
		}
		else
		if(!type_checked)
		{
			char* _token = token;
			char _buffer[strlen(token)];
			if(strchr(token, '*') != NULL)
			{
				strncpy(_buffer, token, strlen(token) - 1);
				_buffer[strlen(token) - 1] = 0;
				_token = _buffer;
				property.is_pointer = true; 
			} 
			if(istype(_token, &type))
			{
				if(type == TYPE_INCOMPLETE)
				{

				}
				else 
				{ 
					type_checked = true; 
					property.type = type;
				}
			}
		}
		else
		if(type_checked && (strcmp(token, "*") != 0))
		{
			char* identifier_name = token;
			if(*identifier_name == '*')
			{
				property.is_pointer = true;
			 	INCREMENT_CHAR_PTR(identifier_name, 1); 
			}
			int counter = 0;  
			while(*identifier_name != ';')
			{
				property.name[counter] = *identifier_name; 
				INCREMENT_CHAR_PTR(identifier_name, 1);  
				++counter; 
				if(counter > 19)
					throw_error("Property name length is larger than 20 characters", __LINE__, ""); 
			}
			property.name[counter] = 0;
			if(*identifier_name == ';')
			{
				if(!storage_checked)
					property.storage = STSP_NONE;
				BUFpush(&property); 
				type_checked = false; 
				storage_checked = false; 
				property.is_const = false; 
				property.is_pointer = false; 
				strcpy(property.name, "\0"); 
			}
		}
		token = strtok(NULL, " \t\n"); 
	} 
	BUFbind(previous_buffer);
	return serialized_struct; 
}