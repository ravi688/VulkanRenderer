
#if !defined(HPML_DEBUG_MODE) && !defined(HPML_RELEASE_MODE)
#	warning "No build mode is defined, neither DEBUG nor RELEASE! using default build mode = DEBUG_MODE"
#	define HPML_DEBUG_MODE
#	define HEADER_CONFIGURATION_SYSTEM_DEBUG_MODE
#	define DEBUG_MODE
#endif

#if !defined(HEADER) && !defined(IMPLEMENTATION)
#	warning "No header inclusion mode is defined, neither HEADER or IMPLEMENTATION! using default header inclusion mode = HEADER"
#	define HEADER
#	define HPML_HEADER
#	define HEADER_CONFIGURATION_SYSTEM_HEADER
#endif