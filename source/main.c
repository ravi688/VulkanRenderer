
#define RENDERER_INCLUDE_TIMING
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

/*
	1. Default clear screen render pass after creating a camera [done]
	2. Test Shader & Material, Default Shaders [done]
	3. Rendering [done]
	4. Render Queue [done]
	5. Muliple render passes (shadow mapping) [ done ]
	6. Multiple sub passes (greyscale) [ done ]
	7. Camera and Light as a Render Object in the scene
	8. Depth texture sharing across multiple render passes
	9. Each shader will have prefered render pass description, if a render pass in the render pass pool
		suffice to meet the requirements of the shader then that pass should be used instead of creating
		another one.
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	
	// initialize renderer
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_DISCRETE, 800, 800, "Renderer", false, true);

	// create a camera
	AUTO camera = camera_create(renderer, CAMERA_PROJECTION_TYPE_PERSPECTIVE, 0.04f, 100, 65 DEG);
	camera_set_clear(camera, COLOR_BLACK, 1.0f);
	
	AUTO light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	// create a render scene
	AUTO scene = render_scene_create_from_preset(renderer, RENDER_SCENE_PRESET_TYPE_DEFAULT);

	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	AUTO shaderH2 = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR);
	AUTO mat2 = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH2, "Material2"));

	AUTO texture = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Smile.bmp");
	AUTO shaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST);
	AUTO shader = shader_library_getH(slib, shaderH);
	AUTO blueMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH, "BlueColorMaterial"));
	AUTO greenMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH, "GreenColorMaterial"));
	AUTO mesh = mesh_create(renderer, mesh3d_cube(1.0f));
	AUTO planeMesh = mesh_create(renderer, mesh3d_plane(2.0f));

	material_set_texture(blueMaterial, "albedo", texture);
	material_set_texture(greenMaterial, "albedo", texture);
	material_set_vec4(blueMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));
	material_set_vec4(greenMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));
	material_set_vec4(mat2, "parameters.color", vec4(float)(1, 1, 1, 1));


	render_object_t* obj2 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj2, greenMaterial);
	render_object_attach(obj2, planeMesh);
	render_object_set_transform(obj2, mat4_translation(float)(0, -0.5f, 0));

	render_object_t* obj1 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj1, blueMaterial);
	render_object_attach(obj1, mesh);
	render_object_set_transform(obj1, mat4_translation(float)(0.5f, 0, 0));

	render_object_t* obj3 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj3, mat2);
	render_object_attach(obj3, mesh);
	render_object_set_transform(obj3, mat4_mul(float)(2, mat4_scale(float)(0.5f, 0.5f, 0.5f), mat4_translation(float)(-2, 0, 0)));

	bool swap = true;
	float angle = 0;
	time_handle_t tHandle = time_get_handle();
	while(renderer_is_running(renderer))
	{
		if(kbhit())
		{
			getch();
			render_object_set_material(obj1, swap ? greenMaterial: blueMaterial);
			render_object_set_material(obj2, swap ? blueMaterial : greenMaterial);
			swap = !swap;
		}
		float deltaTime = time_get_delta_time(&tHandle);
		angle += deltaTime * 90;
		render_object_set_transform(obj1, mat4_mul(float)(2, mat4_translation(float)(0.5f, 0, 0), mat4_rotation(float)(0, angle DEG, 0)));

		// begin command buffer recording
		renderer_begin_frame(renderer);

		// clear the screen
		camera_render(camera, NULL);

		// render the scene
		render_scene_render(scene);

		// end command buffer recording
		renderer_end_frame(renderer);

		// submit the work to the GPU and present the rendered image to the window
		renderer_update(renderer);
	}

	texture_destroy(texture);
	texture_release_resources(texture);

	mesh_destroy(planeMesh);
	mesh_release_resources(planeMesh);
	mesh_destroy(mesh);
	mesh_release_resources(mesh);

	render_scene_destroy(scene);
	render_scene_release_resources(scene);

	light_destroy(light);
	light_release_resources(light);
	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
