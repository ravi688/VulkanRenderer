#ifndef __EXCEPTION_DECLARATIONS_H__
#define __EXCEPTION_DECLARATIONS_H__

#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

declare_exception(METHOD_NOT_IMPLEMENTED);
declare_exception(ACCESS_TO_RESTRICTED_CODE);
declare_exception(DIVIDE_BY_ZERO);
declare_exception(INVERSION_OF_SINGULAR_MATRIX);
declare_exception(VULKAN_INSTANCE_CREATION_FAILED);
declare_exception(NEGATIVE_VALUE);

END_CPP_COMPATIBLE

#endif/*__EXCEPTION_DECLARATIONS_H__*/