
#pragma once

#include <ecs/component.h>

#define TestBehaviour_TYPE_ID CREATE_COMPONENT_ID(10)


typedef struct TestBehaviour
{
	DERIVE_FROM_COMPONENT;
} TestBehaviour;


component_declare_callbacks(TestBehaviour);
