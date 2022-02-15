#include <RenderSystem.h>
#include <Camera.h>
#include <calltrace/calltrace.h>


static void rebuildProjectionMatrix(Camera* this, void* funcptr)
{
	mat4f (*_funcptr)(float, float, float, float) = funcptr;
	this->projection = _funcptr(this->nearClipPlane, this->farClipPlane, this->fov, this->aspectRatio);
	mat4f clip_matrix = mat4f_identity(); clip_matrix.m11 = -1;
	mat4_move(float)(&this->projection, mat4f_mul(clip_matrix, this->projection));
}

static void rebuildScreenProjectionMatrix(Camera* this)
{
	this->screenProjection = mat4f_ortho_project(-0.04f, 100, WINDOW->height, (float)WINDOW->width / WINDOW->height);
	mat4f clip_matrix = mat4f_identity(); clip_matrix.m11 = -1;
	mat4_move(float)(&this->screenProjection, mat4f_mul(clip_matrix, this->screenProjection));
}

static void onWindowResize(render_window_t* window, void* user_data)
{
	Camera* this = user_data;
	rebuildScreenProjectionMatrix(this);
	Camera_setAspectRatio(this, (float)window->width / window->height);
}

void OnInstantiate(Camera)
{
	BindOnDestroy(Camera);
	BindOnRender(Camera);
	BindOnPostRender(Camera);
	this->fov = 60 * DEG2RAD;
	this->farClipPlane = 100.0f;
	this->nearClipPlane = 0.04f;
	this->aspectRatio = (float)WINDOW->width / WINDOW->height;
	this->transform = mat4f_transform((vec3f){ -1.8f, 0.6f, 0 }, (vec3f){ 0, 0, -22 * DEG2RAD });
	this->view = mat4f_inverse(this->transform);
	this->isOrthographic = false;
	this->clearColor = (vec4f) { 0.01f, 0.05f, 0.1f, 1.0f };
	rebuildScreenProjectionMatrix(this);
	rebuildProjectionMatrix(this, this->isOrthographic ? mat4f_ortho_project : mat4f_persp_project);
	render_window_subscribe_on_resize(WINDOW, onWindowResize, this);
}

void OnRender(Camera)
{
	renderer_begin_frame(RENDERER, this->clearColor.x, this->clearColor.y, this->clearColor.z, this->clearColor.w);
}

void OnPostRender(Camera)
{
	renderer_end_frame(RENDERER);
	renderer_update(RENDERER);
}


void OnDestroy(Camera)
{

}


void Camera_setFOV(Camera* this, float fov)
{
	this->fov = fov;
	rebuildProjectionMatrix(this, this->isOrthographic ? mat4f_ortho_project : mat4f_persp_project);
}

void Camera_setAspectRatio(Camera* this, float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	rebuildProjectionMatrix(this, this->isOrthographic ? mat4f_ortho_project : mat4f_persp_project);
}

void Camera_setClipPlanes(Camera* this, float nearClipPlane, float farClipPlane)
{
	this->nearClipPlane = nearClipPlane;
	this->farClipPlane = farClipPlane;
	rebuildProjectionMatrix(this, this->isOrthographic ? mat4f_ortho_project : mat4f_persp_project);
}

void Camera_setOrthographic(Camera* this, bool isOrthographic)
{
	this->isOrthographic = isOrthographic;
	rebuildProjectionMatrix(this, this->isOrthographic ? mat4f_ortho_project : mat4f_persp_project);
}
