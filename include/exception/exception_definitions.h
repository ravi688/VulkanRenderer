#ifndef __EXCEPTION_DEFINITIONS_H__
#define __EXCEPTION_DEFINITIONS_H__

#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

define_exception(METHOD_NOT_IMPLEMENTED);
define_exception(ACCESS_TO_RESTRICTED_CODE);
define_exception(DIVIDE_BY_ZERO);
define_exception(INVERSION_OF_SINGULAR_MATRIX);
define_exception(VULKAN_INSTANCE_CREATION_FAILED);
define_exception(NEGATIVE_VALUE);

END_CPP_COMPATIBLE

#endif/*__EXCEPTION_DEFINITIONS_H__*/