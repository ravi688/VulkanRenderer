#ifndef __UTILITIES_H__
#define __UTILITIES_H__


#define c_reinterpret_cast(value, target) (*((target*)(&(value))))
#define c_static_cast(value, target) ((target)(value))


#endif/*__UTILITIES_H__*/