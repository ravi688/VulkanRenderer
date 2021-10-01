

#ifndef __HPML_TEMPLATE_SYSTEM_H__
#define __HPML_TEMPLATE_SYSTEM_H__

#define template(typename, T) typename##_##T


#define template1(typename, T) template(typename, T)
#define template2(typename, T1, T2) typename##_##T1##T2


typedef char* pchar_t;
typedef int* pint_t;
typedef float* pfloat_t;
typedef double* pdouble_t;

typedef const char* pcchar_t;
typedef const int* pcint_t; 
typedef const float* pcfloat_t;
typedef const double* pcdouble_t;

#endif/*__HPML_TEMPLATE_SYSTEM_H__*/