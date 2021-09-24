

#ifndef __HPML_TEMPLATE_SYSTEM_H__
#define __HPML_TEMPLATE_SYSTEM_H__

#define template(typename, T) typename##_##T


#define template1(typename, T) template(typename, T)
#define template2(typename, T1, T2) typename##_##T1##T2

#endif/*__HPML_TEMPLATE_SYSTEM_H__*/