
#pragma once


#define c_reinterpret_cast(value, target) (*((target*)(&(value))))
#define c_static_cast(value, target) ((target)(value))