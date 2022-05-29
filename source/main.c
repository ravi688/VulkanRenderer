
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

/*
	1. Default clear screen render pass after creating a camera [done]
	2. 
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	
	// initialize renderer
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_AUTO, 800, 800, "Vulkan Renderer", false);
	// create a render queue and add it to the camera
	AUTO queue = render_queue_create(renderer, "GeometryQueue");

	// create a camera
	AUTO camera = camera_create(renderer, CAMERA_PROJECTION_TYPE_PERSPECTIVE, 0.04f, 100, 65);
	// camera_add_render_queue(camera, queue);
	
	// // for shadow rendering there should be a light
	// AUTO light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);
	// light_set_shadow(light, true);
	// light_add_render_queue(light, queue);

	// // load a shader & create a material
	// AUTO shaderH = shader_library_create_shader_from_preset(renderer_get_shader_library(renderer), SHADER_LIBRARY_SHADER_PRESET_UNLIT);
	// AUTO mlib = renderer_get_material_library(renderer);
	// AUTO material = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH));
	// material_set_vec4(material, "color", vec4(float)(1, 0, 1, 1));

	// // load a mesh and create a render object/node and add it to the GeometryQueue
	// AUTO mesh = mesh_load(renderer, "Monkey.obj")
	// AUTO object = mesh_get_render_object(mesh);
	// render_object_set_material(object, material);
	// render_object_set_position(object, vec3(float)(0, 0, 0));
	// render_queue_add(queue, object);


	while(renderer_is_running(renderer))
	{
		// begin command buffer recording
		renderer_begin_frame(renderer);

		// shadow render pass
		// light_render(light);

		// color render pass
		camera_render(camera, NULL);

		// end command buffer recording
		renderer_end_frame(renderer);

		// submit the work to the GPU and present the rendered image to the window
		renderer_update(renderer);
	}

	// light_destroy(light);
	// light_release_resources(light);

	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
