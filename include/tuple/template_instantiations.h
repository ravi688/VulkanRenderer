
#pragma once


#include <tuple/template_definitions.h>
#include <stdint.h>
#include <renderer/defines.h>

instantiate_tuple_t(int, pchar_t); 
instantiate_tuple_t(uint32_t, pchar_t);
instantiate_tuple_t(uint64_t, pchar_t);

BEGIN_CPP_COMPATIBLE

instantiate_declaration_tuple_get1(int, pchar_t);
instantiate_declaration_tuple_get2(int, pchar_t);
instantiate_declaration_tuple_set1(int, pchar_t);
instantiate_declaration_tuple_set2(int, pchar_t);


#ifdef IMPLEMENTATION

instantiate_implementation_tuple_get1(int, pchar_t); 
instantiate_implementation_tuple_get2(int, pchar_t);
instantiate_implementation_tuple_set1(int, pchar_t); 
instantiate_implementation_tuple_set2(int, pchar_t);

#endif

END_CPP_COMPATIBLE

