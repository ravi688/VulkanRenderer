#include <engine/component.h>
#include <engine/object.h>
#include <stdlib.h>
#include <stdio.h>
#include <debug.h>

void __component_set_enabled(component_t* component, bool value)
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	if(value)//turn on the bits
		component->boolean_info |= COMPONENT_ENABLED; 
	else //cancel the bits
		component->boolean_info &= (~COMPONENT_ENABLED);
}
void __component_set_awake_called(component_t* component, bool value)
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	if(value)//turn on the bits
		component->boolean_info	|= COMPONENT_AWAKE_CALLED;
	else//cancel the bits
		component->boolean_info &= (~COMPONENT_AWAKE_CALLED);
}

void __component_set_start_called(component_t* component, bool value)
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	if(value)//turn on the bits
		 component->boolean_info |= COMPONENT_START_CALLED;
	else //cancel the bits
		component->boolean_info &= (~COMPONENT_START_CALLED);
}

component_t* __component_new(object_t* object, uint32_t size_of_component, uint64_t type_id)
{
	if((type_id & Component_TYPE_ID) != Component_TYPE_ID)
	{
		log_err("Passed type_id \"%d\" is not of component type", type_id);
	 	return NULL;
	}
	if(size_of_component < sizeof(component_t))
	{
		log_err("Passed size_of_component is less than sizeof(component_t)");
		return NULL;
	}

	void* allocated_memory = malloc(size_of_component);
	component_t* component = (component_t*)allocated_memory;
	component->id = type_id;
	component->object = object;
	component->boolean_info = 0;

	component_set_enabled(component, true);
	component_set_start_called(component, false);
	component_set_awake_called(component, false);

	component->m_OnDestroy = NULL;
	component->m_OnStart= NULL;
	component->m_OnUpdate= NULL;
	component->m_OnAwake= NULL;
	component->m_OnLateUpdate= NULL;
	component->m_OnPostRender= NULL;
	component->m_OnPreRender= NULL;
	log_msg("component_t of type_id %d is Instantiated", type_id);
	return component;
}

void __component_set_OnDestroy(component_t* component, void (*on_destroy)(component_t*))
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();	
	component->m_OnDestroy = on_destroy;
}
void __component_set_OnStart(component_t* component, void (*on_start)(component_t*))
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	component->m_OnStart = on_start;
}
void __component_set_OnUpdate(component_t* component, void (*on_update)(component_t*))
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	component->m_OnUpdate = on_update;
}

object_t* __component_get_object(component_t* component)
{
	ensure_component_is_not_NULL(NULL);
	ensure_passed_value_is_component(NULL);
	return component->object;
}

void __component_destroy(component_t* component)
{
	ensure_component_is_not_NULL();
	ensure_passed_value_is_component();
	free(component);
}