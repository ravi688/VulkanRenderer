#pragma once

#include <ecs/component.h>

#define GunController_TYPE_ID CREATE_COMPONENT_ID(4)

typedef struct GunController
{
	DERIVE_FROM_COMPONENT;
} GunController;


component_declare_callbacks(GunController);

