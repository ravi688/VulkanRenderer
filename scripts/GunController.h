#ifndef __GUN_CONTROLLER_H__
#define __GUN_CONTROLLER_H__

#include <engine/engine.h>

#define GunController_TYPE_ID CREATE_COMPONENT_ID(4)

typedef struct GunController
{
	DERIVE_FROM_COMPONENT;
} GunController;


component_declare_callbacks(GunController);

#endif/*__GUN_CONTROLLER_H__*/