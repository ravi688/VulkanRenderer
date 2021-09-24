
#pragma once

#include <engine/engine.h>

#define TestBehaviour_TYPE_ID CREATE_COMPONENT_ID(10)


typedef struct TestBehaviour
{
	DERIVE_FROM_COMPONENT;
} TestBehaviour;


component_declare_callbacks(TestBehaviour);