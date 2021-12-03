#pragma once

#include <ecs/component.h>

#define Ammo_TYPE_ID CREATE_COMPONENT_ID(3)

typedef struct __Ammo Ammo;

typedef struct __Ammo
{
	DERIVE_FROM_COMPONENT;
	float penetrating_power;
} Ammo;


component_declare_callbacks(Ammo);

