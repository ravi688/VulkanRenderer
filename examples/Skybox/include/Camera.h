#pragma once

#include <ecs/component.h>

static const u64 TYPE_ID(Camera) = CREATE_COMPONENT_ID(2);

typedef struct Camera
{
	DERIVE_FROM_COMPONENT;
} Camera;


void OnInstantiate(Camera);
void OnDestroy(Camera);
// void OnAwake(Camera);
// void OnStart(Camera);
// void OnUpdate(Camera);

