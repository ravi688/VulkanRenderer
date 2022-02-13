#pragma once

#include <ecs/component.h>

static const u64 TYPE_ID(Rock) = CREATE_COMPONENT_ID(1);

typedef struct Rock
{
	DERIVE_FROM_COMPONENT;
} Rock;


void OnInstantiate(Rock);
void OnDestroy(Rock);
// void OnAwake(Rock);
// void OnStart(Rock);
// void OnUpdate(Rock);

