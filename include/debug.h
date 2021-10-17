#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#ifdef LOG_DEBUG
#define LOG_ERR 
#define LOG_WRN
#define LOG_FETAL_ERR 
#define LOG_MSG
#endif


#include <stdio.h>
#include <stdbool.h>

#ifdef LOG_ERR
#define log_err(...)do {\
 printf("[Error]: ");\
 printf(__VA_ARGS__);\
 printf("\t, AT %s, LINE: %d, FILE: %s\n",__PRETTY_FUNCTION__, __LINE__, __FILE__);\
} while(false)
#else
#define log_err(...)
#endif

#ifdef LOG_WRN
#define log_wrn(...)do {\
 printf("[Warning]: ");\
 printf(__VA_ARGS__);\
 printf("\t, AT %s, LINE: %d, FILE: %s\n",__PRETTY_FUNCTION__, __LINE__, __FILE__);\
} while(false)
#else
#define log_wrn(...)
#endif

#ifdef LOG_FETAL_ERR
#define log_fatal_err(...)do {\
 printf("[Fetal Error]: ");\
 printf(__VA_ARGS__);\
 printf("\t, AT %s, LINE: %d, FILE: %s\n",__PRETTY_FUNCTION__, __LINE__, __FILE__);\
 exit(1);\
} while(false)
#else
#define log_fatal_err(...)
#endif

#define log_fetal_err(...) log_fatal_err(__VA_ARGS__)

#ifdef LOG_MSG
#define log_msg(...)do {\
 printf("[Log]: ");\
 printf(__VA_ARGS__);\
 printf("\t, AT %s, LINE: %d, FILE: %s\n",__PRETTY_FUNCTION__, __LINE__, __FILE__);\
} while(false)
#else
#define log_msg(...)
#endif

#endif/*__DEBUG_H__*/