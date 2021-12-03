#pragma once

#include <ecs/component.h>

#define mesh_filter_t_TYPE_ID ((uint64_t)(Component_TYPE_ID | (((uint64_t)6) << 32)))

typedef struct
{
	DERIVE_FROM_COMPONENT;
} mesh_filter_t;



component_declare_callbacks(mesh_filter_t);
