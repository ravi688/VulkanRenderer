
#include <ecs/component.h>


static const u64 TYPE_ID(FPSText) = CREATE_COMPONENT_ID(4);

#include <Defines.h>

#include <Camera.h>
#include <RenderSystem.h>
#include <Math.h>

#include <sge/material.h>
#include <sge/shader.h>
#include <sge/text_mesh.h>
#include <sge/time.h>

typedef struct FPSText
{
	DERIVE_FROM_COMPONENT;
	mat4f transform;
	shader_t* shader;
	material_t* material;
	Camera* camera;
	font_t* font;
	glyph_mesh_pool_t* glyphPool;
	text_mesh_t* textMesh;
	text_mesh_string_handle_t stringHandle;
	float fontSize;
	vec3f position;
	u32 fpsCount;
	float fpsUpdateInterval;
	time_handle_t fpsTimeHandle;
} FPSText;


void OnInstantiate(FPSText);
void OnUpdate(FPSText);
void OnDestroy(FPSText);
void OnStart(FPSText);
void OnRender(FPSText);


static ALWAYS_INLINE void FPSText_set(FPSText* text, const char* str)
{
	text_mesh_string_setH(text->textMesh, text->stringHandle, str);
}

void FPSText_setPosition(FPSText* text, vec3f position);

static ALWAYS_INLINE void FPSText_setColor(FPSText* text, vec4f color)
{
	material_set_vec4(text->material, "properties.color", color);
}

void FPSText_setFontSize(FPSText* text, float size);
