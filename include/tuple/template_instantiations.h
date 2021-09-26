
#pragma once


#include <tuple/template_definitions.h>

instantiate_tuple_t(int, pchar_t); 

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