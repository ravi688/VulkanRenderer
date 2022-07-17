
#pragma once


#include <renderer/defines.h>


#define TYPE_ID(type) type##_TYPE_ID
#define TYPE_ID_CREATE(id) CAST_TO(u64, id)

typedef u64 type_t;
