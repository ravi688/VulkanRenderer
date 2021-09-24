#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <stdint.h>
#include <stdbool.h>

#define COMPONENT(component) ((component_t*)(component))
#define IS_COMPONENT(component) (((component)->id & Component_TYPE_ID) == Component_TYPE_ID)
#define Component_TYPE_ID ((uint64_t)(2))

#define CREATE_COMPONENT_ID(id) ((uint64_t)(Component_TYPE_ID | (((uint64_t)(id)) << 32)))

#define DERIVE_FROM_COMPONENT component_t component

#define BindOnAwake(type) COMPONENT(this)->m_OnAwake = 	 COMPONENT_CALLBACK(type##_OnAwake)
#define BindOnStart(type) COMPONENT(this)->m_OnStart = 	 COMPONENT_CALLBACK(type##_OnStart)
#define BindOnUpdate(type) COMPONENT(this)->m_OnUpdate = COMPONENT_CALLBACK(type##_OnUpdate)
#define BindOnLateUpdate(type) COMPONENT(this)->m_OnLateUpdate = COMPONENT_CALLBACK(type##_OnLateUpdate)
#define BindOnPreRender(type) COMPONENT(this)->m_OnPreRender =  COMPONENT_CALLBACK(type##_OnPreRender)
#define BindOnPostRender(type) COMPONENT(this)->m_OnPostRender = COMPONENT_CALLBACK(type##_OnPostRender)
#define BindOnDestroy(type) COMPONENT(this)->m_OnDestroy = COMPONENT_CALLBACK(type##_OnDestroy)

#define component_new(type) ((type*)__component_new(NULL, sizeof(type), type##_TYPE_ID))
#define component_get_object(component) __component_get_object(COMPONENT(component))
#define component_destroy(component) __component_destroy(COMPONENT(component))

#define component_init_callbacks(component, type) \
	COMPONENT(this)->m_OnDestroy = COMPONENT_CALLBACK(type##_OnDestroy); \
	COMPONENT(this)->m_OnStart = COMPONENT_CALLBACK(type##_OnStart); \
	COMPONENT(this)->m_OnUpdate  = COMPONENT_CALLBACK(type##_OnUpdate)
	// COMPONENT(this)->m_OnLateUpdate = COMPONENT_CALLBACK(type##_OnLateUpdate); \
	// COMPONENT(this)->m_OnAwake = COMPONENT_CALLBACK(type##_OnAwake); \
	// COMPONENT(this)->m_OnPostRender = COMPONENT_CALLBACK(type##_OnPostRender); \
	// COMPONENT(this)->m_OnPreRender = COMPONENT_CALLBACK(type##_OnPreRender)

#define component_declare_callbacks(type)\
	void type##_OnInstantiate(type* this);\
	void type##_OnDestroy(type* this);\
	void type##_OnStart(type* this);\
	void type##_OnUpdate(type* this);\
	void type##_OnLateUpdate(type* this);\
	void type##_OnAwake(type* this);\
	void type##_OnPostRender(type* this);\
	void type##_OnPreRender(type* this)

#define component_set_OnDestroy(component, on_destroy) __component_set_OnDestroy(COMPONENT(component), on_destroy)
#define component_set_OnStart(component, on_start) __component_set_OnStart(COMPONENT(component), on_start)
#define component_set_OnUpdate(component, on_update) __component_set_OnUpdate(COMPONENT(component), on_update)

// #define COMPONENT_CALLBACK(callback) (*((component_callback_t*)(&callback)))

#ifndef COMPONENT_CALLBACK
#define COMPONENT_CALLBACK(callback) ((component_callback_t)(callback))
#endif

#define	COMPONENT_ENABLED ((uint8_t)(((uint8_t)1) << 7))
#define COMPONENT_START_CALLED ((uint8_t)(((uint8_t)1) << 6))
#define COMPONENT_AWAKE_CALLED ((uint8_t)(((uint8_t)1) << 5))

#define component_is_enabled(component) ((COMPONENT(component)->boolean_info & COMPONENT_ENABLED) == COMPONENT_ENABLED)
#define component_is_start_called(component) ((COMPONENT(component)->boolean_info & COMPONENT_START_CALLED) == COMPONENT_START_CALLED)
#define component_is_awake_called(component) ((COMPONENT(component)->boolean_info & COMPONENT_AWAKE_CALLED) == COMPONENT_AWAKE_CALLED)
#define component_set_enabled(component, boolean) __component_set_enabled(COMPONENT(component), boolean)
#define component_set_awake_called(component, boolean) __component_set_awake_called(COMPONENT(component), boolean)
#define component_set_start_called(component, boolean) __component_set_start_called(COMPONENT(component), boolean)

#define OnInstantiate(component_type, function_definition) void component_type##_OnInstantiate(component_type* this)\
{\
 	function_definition \
}

#define OnDestroy(component_type, function_definition) void component_type##_OnDestroy(component_type* this)\
{\
	function_definition\
}

#define OnStart(component_type, function_definition) void  component_type##_OnStart(component_type* this) { function_definition }
#define OnUpdate(component_type, function_definition) void  component_type##_OnUpdate(component_type* this) { function_definition }
#define OnLateUpdate(component_type, function_definition)  void component_type##_OnLateUpdate(component_type* this) { function_definition }
#define OnAwake(component_type, function_definition) void  component_type##_OnAwake(component_type* this) { function_definition }
#define OnPostRender(component_type, function_definition) void component_type##_OnPostRender(component_type* this) { function_definition }
#define OnPreRender(component_type, function_definition) void component_type##_OnPreRender(component_type* this) { function_definition }

//component_t.id specification
//
//The size of "id" is of 64 bits
//
// [32 bits] 																[32 bits]
// The first 32 bits represent  											The second 32 bits represent the built in types in the engine
// IDs of different implementations (derivations)
// of type component_t

typedef struct object_t object_t;
typedef struct component_t component_t;
typedef void (*component_callback_t)(component_t*);

typedef struct component_t
{
	uint64_t id;
	object_t* object;
	uint8_t boolean_info;
	component_callback_t m_OnDestroy;
	component_callback_t m_OnStart;
	component_callback_t m_OnUpdate;
	component_callback_t m_OnAwake;
	component_callback_t m_OnLateUpdate;
	component_callback_t m_OnPostRender;
	component_callback_t m_OnPreRender;
} component_t; 	//holds the component data


component_t* __component_new(object_t* object, uint32_t size_of_component, uint64_t type_id);;
object_t* __component_get_object(component_t* component);
void __component_destroy(component_t* component); /*Note: this will not invoke OnDestroy callback function*/

void __component_set_enabled(component_t* component, bool value);
void __component_set_awake_called(component_t* component, bool value);
void __component_set_start_called(component_t* component, bool value);

void __component_set_OnDestroy(component_t* component, void (*on_destroy)(component_t*));
void __component_set_OnStart(component_t* component, void (*on_start)(component_t*));
void __component_set_OnUpdate(component_t* component, void (*on_update)(component_t*));

#define ensure_component_is_not_NULL(return_value)\
if(component == NULL)\
{\
	log_err("component_t is NULL");\
	return return_value;\
}
#define ensure_passed_value_is_component(return_value)\
if((component->id & Component_TYPE_ID) != Component_TYPE_ID)\
{\
	log_err("Passed value of type id %d is not of component type", component->id);\
 	return return_value;\
}

#endif/*__COMPONENT_H__*/