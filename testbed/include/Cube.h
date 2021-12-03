
#pragma once

#include <ecs/component.h>

#define Cube_TYPE_ID CREATE_COMPONENT_ID(11)

typedef struct 
{
	DERIVE_FROM_COMPONENT;
	float width;
	float height;
	float depth;
} Cube;

component_declare_callbacks(Cube);