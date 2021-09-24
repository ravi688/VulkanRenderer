#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include <engine/engine.h>

#define Animator_TYPE_ID CREATE_COMPONENT_ID(2)

typedef struct __Animator Animator;

typedef struct __Animator
{
	component_t component;
} Animator;


component_declare_callbacks(Animator);

#endif/*__ANIMATOR_H__*/