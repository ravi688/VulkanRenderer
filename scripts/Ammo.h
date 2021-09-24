#ifndef __AMMO_H__
#define __AMMO_H__

#include <engine/engine.h>

#define Ammo_TYPE_ID CREATE_COMPONENT_ID(3)

typedef struct __Ammo Ammo;

typedef struct __Ammo
{
	component_t component;
	float penetrating_power;
} Ammo;


component_declare_callbacks(Ammo);

#endif/*__AMMO_H__*/