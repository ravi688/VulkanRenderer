
#pragma

#include <renderer/defines.h>
#include <renderer/color.h>
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_handles.h> 	// vulkan_render_pass_handle_t
#include <renderer/internal/vulkan/vulkan_types.h> 		// vulkan_render_target_render_technique_t
#include <renderer/internal/vulkan/vulkan_descriptor_set.h> 	// vulkan_descriptor_set_t

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef enum vulkan_camera_projection_type_t
{
	VULKAN_CAMERA_PROJECTION_TYPE_UNDEFINED,
	VULKAN_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
	VULKAN_CAMERA_PROJECTION_TYPE_PERSPECTIVE,
	VULKAN_CAMERA_PROJECTION_TYPE_STEROGRAPHIC
} vulkan_camera_projection_type_t;

typedef enum vulkan_camera_render_target_type_t
{
	VULKAN_CAMERA_RENDER_TARGET_TYPE_UNDEFINED = 0,
	VULKAN_CAMERA_RENDER_TARGET_TYPE_SWAPCHAIN,
	VULKAN_CAMERA_RENDER_TARGET_TYPE_TEXTURE
} vulkan_camera_render_target_type_t;

typedef struct vulkan_camera_create_info_t
{
	vulkan_camera_projection_type_t projection_type;
	vulkan_render_pass_handle_t default_render_pass;
} vulkan_camera_create_info_t;

typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_render_scene_t vulkan_render_scene_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_framebuffer_t vulkan_framebuffer_t;

typedef struct vulkan_camera_t
{
	vulkan_renderer_t* renderer;
	vulkan_buffer_t buffer;
	vulkan_descriptor_set_t set;
	vulkan_render_pass_t* default_render_pass;

	vulkan_texture_t* render_target; 	// NULL, if the target is the screen

	BUFFER framebuffers;

	struct_descriptor_t struct_definition;
	struct_field_handle_t transform_handle;
	struct_field_handle_t projection_handle;
	struct_field_handle_t view_handle;
	struct_field_handle_t screen_handle;

	bool is_active;
	vulkan_camera_projection_type_t projection_type;
	float far_clip_plane;
	float near_clip_plane;
	union
	{
		float height;
		float field_of_view;
	};
	vec3_t(float) position;
	vec3_t(float) rotation;
	mat4_t(float) transform;
	mat4_t(float) view;
	mat4_t(float) projection;
	mat4_t(float) screen;
} vulkan_camera_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_camera_t* vulkan_camera_new();
RENDERER_API vulkan_camera_t* vulkan_camera_create(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info);
RENDERER_API void vulkan_camera_create_no_alloc(vulkan_renderer_t* renderer, vulkan_camera_create_info_t* create_info, vulkan_camera_t OUT camera);
RENDERER_API void vulkan_camera_destroy(vulkan_camera_t* camera);
RENDERER_API void vulkan_camera_release_resources(vulkan_camera_t* camera);

/* logic functions */
#define VULKAN_CAMERA_CLEAR_FLAG_DONT_CARE 1
#define VULKAN_CAMERA_CLEAR_FLAG_CLEAR 0
RENDERER_API void vulkan_camera_begin(vulkan_camera_t* camera, u32 clear_flags);
RENDERER_API void vulkan_camera_end(vulkan_camera_t* camera);
RENDERER_API void vulkan_camera_set_clear(vulkan_camera_t* camera, color_t color, float depth);
RENDERER_API void vulkan_camera_set_render_target(vulkan_camera_t* camera, vulkan_camera_render_target_type_t target_type, vulkan_texture_t* texture);
RENDERER_API void vulkan_camera_render(vulkan_camera_t* camera, vulkan_render_scene_t* scene, u64 queue_mask);
RENDERER_API void vulkan_camera_render_to_texture(vulkan_camera_t* camera, vulkan_render_scene_t* scene, vulkan_texture_t* texture);

RENDERER_API vulkan_framebuffer_t* vulkan_camera_get_framebuffer_list(vulkan_camera_t* camera, vulkan_framebuffer_list_handle_t handle);
RENDERER_API vulkan_framebuffer_list_handle_t vulkan_camera_register_render_pass(vulkan_camera_t* camera, vulkan_render_pass_t* pass);


/* getters */
static RENDERER_API FORCE_INLINE bool vulkan_camera_is_offscreen(vulkan_camera_t* camera)
{
	return camera->render_target != NULL;
}
static RENDERER_API FORCE_INLINE bool vulkan_camera_is_active(vulkan_camera_t* camera)
{
	return camera->is_active;
}
RENDERER_API mat4_t(float) vulkan_camera_get_view(vulkan_camera_t* camera);
RENDERER_API mat4_t(float) vulkan_camera_get_transform(vulkan_camera_t* camera);
RENDERER_API mat4_t(float) vulkan_camera_get_projection(vulkan_camera_t* camera);
RENDERER_API vec3_t(float) vulkan_camera_get_position(vulkan_camera_t* camera);
RENDERER_API vec3_t(float) vulkan_camera_get_rotation(vulkan_camera_t* camera);
RENDERER_API vec2_t(float) vulkan_camera_get_clip_planes(vulkan_camera_t* camera);
static RENDERER_API FORCE_INLINE float vulkan_camera_get_near_clip_plane(vulkan_camera_t* camera) { return vulkan_camera_get_clip_planes(camera).x; }
static RENDERER_API FORCE_INLINE float vulkan_camera_get_far_clip_plane(vulkan_camera_t* camera) { return vulkan_camera_get_clip_planes(camera).y; }
RENDERER_API float vulkan_camera_get_field_of_view(vulkan_camera_t* camera);
static RENDERER_API FORCE_INLINE float vulkan_camera_get_height(vulkan_camera_t* camera) { return vulkan_camera_get_field_of_view(camera); }

/* setters */
static RENDERER_API FORCE_INLINE void vulkan_camera_set_active(vulkan_camera_t* camera, bool is_active)
{
	camera->is_active = is_active;
}
RENDERER_API void vulkan_camera_set_transform(vulkan_camera_t* camera, mat4_t(float) transform);
RENDERER_API void vulkan_camera_set_position(vulkan_camera_t* camera, vec3_t(float) position);
RENDERER_API void vulkan_camera_set_rotation(vulkan_camera_t* camera, vec3_t(float) rotation);
RENDERER_API void vulkan_camera_set_clip_planes(vulkan_camera_t* camera, float near_clip_plane, float far_clip_plane);
RENDERER_API void vulkan_camera_set_field_of_view(vulkan_camera_t* camera, float fov);
static RENDERER_API FORCE_INLINE void vulkan_camera_set_height(vulkan_camera_t* camera, float height) { vulkan_camera_set_field_of_view(camera, height); }

END_CPP_COMPATIBLE
