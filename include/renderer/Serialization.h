#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_
#include <renderer/defs.h>
#include <buffer.h>

#define serialized_struct_set_property_int8(struct_name, property_name, value, object_ptr)\
{\
	int8_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_int16(struct_name, property_name, value, object_ptr)\
{\
	int16_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_int32(struct_name, property_name, value, object_ptr)\
{\
	int32_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_int64(struct_name, property_name, value, object_ptr)\
{\
	int64_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_uint8(struct_name, property_name, value, object_ptr)\
{\
	uint8_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_uint16(struct_name, property_name, value, object_ptr)\
{\
	uint16_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_uint32(struct_name, property_name, value, object_ptr)\
{\
	uint32_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_uint64(struct_name, property_name, value, object_ptr)\
{\
	uint64_t _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_float(struct_name, property_name, value, object_ptr)\
{\
	float _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_double(struct_name, property_name, value, object_ptr)\
{\
	double _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value, object_ptr);\
}
#define serialized_struct_set_property_long_double(struct_name, property_name, value, object_ptr)\
{\
	long double _value = value;\
	serialized_struct_set_property_value(struct_name, property_name,&_value);\
}

#define serialized_struct_get_property_int8(struct_name, property_name,  object_ptr) (*(int8_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_int16(struct_name, property_name,  object_ptr) (*(int16_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_int32(struct_name, property_name,  object_ptr) (*(int32_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_int64(struct_name, property_name,  object_ptr) (*(int64_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_uint8(struct_name, property_name,  object_ptr) (*(uint8_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_uint16(struct_name, property_name,  object_ptr) (*(uint16_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_uint32(struct_name, property_name,  object_ptr) (*(uint32_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_uint64(struct_name, property_name,  object_ptr) (*(uint64_t*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_float(struct_name, property_name,  object_ptr) (*(float*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_double(struct_name, property_name,  object_ptr) (*(double*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))
#define serialized_struct_get_property_long_double(struct_name, property_name,  object_ptr) (*(long double*)serialized_struct_get_property_value(struct_name, property_name, object_ptr))

#define serialized_property_set_int8(property, value)\
{\
	int8_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_int16(property, value)\
{\
	int16_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_int32(property, value)\
{\
	int32_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_int64(property, _value)\
{\
	int64_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_uint8(property, value)\
{\
	uint8_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_uint16(property, value)\
{\
	uint16_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_uint32(property, value)\
{\
	uint32_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_uint64(property, value)\
{\
	uint64_t _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_float(property, value)\
{\
	float _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_double(property, value)\
{\
	double _value = value;\
	serialized_property_set_value(property, &_value);\
}
#define serialized_property_set_long_double(property, value)\
{\
	long double _value = value;\
	serialized_property_set_value(property, &_value);\
}

#define serialized_property_get_int8(property) (*(int8_t*)serialized_property_get_value(property))
#define serialized_property_get_int16(property) (*(int16_t*)serialized_property_get_value(property))
#define serialized_property_get_int32(property) (*(int32_t*)serialized_property_get_value(property))
#define serialized_property_get_int64(property) (*(int64_t*)serialized_property_get_value(property))
#define serialized_property_get_uint8(property) (*(uint8_t*)serialized_property_get_value(property))
#define serialized_property_get_uint16(property) (*(uint16_t*)serialized_property_get_value(property))
#define serialized_property_get_uint32(property) (*(uint32_t*)serialized_property_get_value(property))
#define serialized_property_get_uint64(property) (*(uint64_t*)serialized_property_get_value(property))
#define serialized_property_get_float(property) (*(float*)serialized_property_get_value(property))
#define serialized_property_get_double(property) (*(double*)serialized_property_get_vaule(property))
#define serialized_property_get_long_double(property) (*(long double*)serialized_property_get_value(property))

//loads the source file from secondary storage to the main memory for further parsing and compiling
void load_serialization_source_file(const char* name);
//sets the internal source buffer 
//NOTE: if this buffer is allocated in the heap then it will not be automatically freed, you must have to free it explicitly
void set_serialization_source_buffer(const char* buffer); 
//destoyes the allocated buffers for the serialized objects and properties
void destroy_serialization_data(); 
//prints the details of a serialized property
void serialized_property_print(SerializedProperty* property);
//prints the details of a serialized struct along with its all property details
void serialized_struct_print(const char* name); 
//serializes a struct and stores the retrieved (serialized information) information into a buffer of SerializedStructs
void struct_serialize(const char* struct_name);
//instantiates a new object of type typeof('serialized_struct_name')
//NOTE: that struct must be serialized first
void* instantiate_object(const char* struct_name); 
//returns a pointer to the memory bock which corresponds to the property
void* serialized_property_get_value(SerializedProperty* property);
//sets a value of a memory block which corresponds to the property
void serialized_property_set_value(SerializedProperty* property, void* value);
//sets a value to a property in the serialized struct named as 'struct_name'
void serialized_struct_set_property_value(const char* struct_name, const char* property_name, void* value, void* object_ptr); 
//returns a pointer to the memory block of property of the struct named as 'struct_name'
void* serialized_struct_get_property_value(const char* struct_name, const char* property_name, void* object_ptr); 
//returns the property with name 'property_name' and you have to pass the base address of an object (struct)
SerializedProperty serialized_struct_get_property(const char* serialized_struct_name, const char* property_name, void* object_ptr);

SerializedStruct* serialized_struct_get(const char* name);


#endif