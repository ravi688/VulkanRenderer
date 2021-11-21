#pragma once

#include <ecs/component.h>

#define Animator_TYPE_ID CREATE_COMPONENT_ID(2)

typedef struct __Animator Animator;

typedef struct __Animator
{
	DERIVE_FROM_COMPONENT;
} Animator;


component_declare_callbacks(Animator);


