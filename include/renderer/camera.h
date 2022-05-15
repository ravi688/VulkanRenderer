
#pragma

#include <renderer/defines.h>

#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef enum projection_type_t
{
	PROJECTION_TYPE_ORTHOGRAPHIC,
	PROJECTION_TYPE_PERSPECTIVE,
	PROJECTION_TYPE_STEROGRAPHIC
} projection_type_t;

typedef struct camera_t camera_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API camera_t* camera_new();
RENDERER_API camera_t* camera_create(renderer_t* renderer, projection_type_t projection_type, float height_or_angle);
RENDERER_API void camera_destroy(camera_t* camera);
RENDERER_API void camera_release_resources(camera_t* camera);

/* getters */
RENDERER_API vec3_t(float) camera_get_position(camera_t* camera);
RENDERER_API vec3_t(float) camera_get_rotation(camera_t* camera);
RENDERER_API float camera_get_aspect_ratio(camera_t* camera);
RENDERER_API float camera_get_field_of_view(camera_t* camera);
static RENDERER_API FORCE_INLINE float camera_get_height(camera_t* camera) { return camera_get_field_of_view(camera); }

/* setters */
RENDERER_API void camera_set_position(camera_t* camera, vec3_t(float) position);
RENDERER_API void camera_set_rotation(camera_t* camera, vec3_t(float) rotation);
RENDERER_API void camera_set_aspect_ratio(camera_t* camera, float aspect_ratio);
RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov);
static RENDERER_API FORCE_INLINE void camera_set_height(camera_t* camera) { camera_set_field_of_view(camera); }

END_CPP_COMPATIBLE
