#pragma once

#include <ecs/component.h>

static const u64 TYPE_ID(Plane) = CREATE_COMPONENT_ID(3);

#include <Math.h>
#include <Camera.h>

#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/material.h>
#include <renderer/shader.h>

typedef struct Plane
{
	DERIVE_FROM_COMPONENT;
	mat4f transform;			// model matrix
	mesh_t* mesh;				// handle to the mesh
	texture_t* textures[2];		// handle to normal and albedo map
	material_t* material;		// handle to the material
	shader_t* shader;			// handle to the shader
	Camera* camera;				// pointer to the camera
} Plane;


void OnInstantiate(Plane);
void OnDestroy(Plane);
void OnStart(Plane);
void OnRender(Plane);
