#pragma once

#include <ecs/component.h>
static const u64 TYPE_ID(Camera) = CREATE_COMPONENT_ID(2);

#include <Math.h>

typedef struct Camera
{
	DERIVE_FROM_COMPONENT;
	mat4f transform;			// model matrix
	mat4f projection;			// view space to clip space projection matrix
	mat4f view;					// view matrix
	mat4f screenProjection; 	// screen space projection
	vec4f clearColor;
	union
	{
		float fov;
		float height;
	};
	float aspectRatio;
	float farClipPlane;
	float nearClipPlane;
	bool isOrthographic;
} Camera;


void OnInstantiate(Camera);
void OnDestroy(Camera);
void OnRender(Camera);
void OnPostRender(Camera);
// void OnAwake(Camera);
// void OnStart(Camera);
// void OnUpdate(Camera);

// setters
#define Camera_setHeight(camera, height) Camera_setFOV(camera, fov)
void Camera_setFOV(Camera* camera, float fov);
void Camera_setAspectRatio(Camera* camera, float aspectRatio);
void Camera_setClipPlanes(Camera* camera, float nearClipPlane, float farClipPlane);
void Camera_setOrthographic(Camera* camera, bool isOrthographic);

