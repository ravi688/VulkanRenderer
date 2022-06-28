
#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
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
	7. Shadow mapping improvement [ done ]
	8. Lights [ in - progress ]
	9. Multiple light system
	10. Deferred rendering & forward rendering
	11. Camera and Light as a Render Object in the scene
	12. Depth texture sharing across multiple render passes
	13. Each shader will have prefered render pass description, if a render pass in the render pass pool
		suffice to meet the requirements of the shader then that pass should be used instead of creating
		another one.
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	
	// initialize renderer
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_DISCRETE, 900, 900, "Renderer", false, true);

	AUTO camera_system = renderer_get_camera_system(renderer);

	// TODO: every scene will have their own camera system ( Camera sets )
	// create a camera
	AUTO camera = camera_system_getH(camera_system,
							camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(camera, COLOR_WHITE, 1.0f);
	camera_set_active(camera, true);

	AUTO camera3 = camera_system_getH(camera_system,
							camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_active(camera3, false);
	camera_set_clear(camera3, COLOR_GREEN, 1.0f);
	camera_set_position(camera3, vec3(float)(0, 0.6, -3.0f));
	camera_set_rotation(camera3, vec3(float)(10 DEG, -90 DEG, 0));

	AUTO light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);
	light_set_rotation(light, vec3(float)(10 DEG, -120 DEG, 0));
	light_set_position(light, vec3(float)(1, 0.6f, -3.0f));
	// light_set_position(light, vec3(float)(0, 1, -1));

	// create a render scene
	AUTO scene = render_scene_create_from_preset(renderer, RENDER_SCENE_PRESET_TYPE_DEFAULT);

	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	AUTO skyboxTexture = texture_load(renderer, TEXTURE_TYPE_CUBE, 
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	AUTO skyboxGeometry = mesh3d_cube(5.0f);
	mesh3d_flip_triangles(skyboxGeometry);
	AUTO skyboxMesh = mesh_create(renderer, skyboxGeometry);
	AUTO skyboxShaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_SKYBOX);
	AUTO skyboxMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, skyboxShaderH, "SkyboxMaterial"));
	material_set_texture(skyboxMaterial, "albedo", skyboxTexture);

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

	render_object_t* skyboxObj = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_BACKGROUND));
	render_object_set_material(skyboxObj, skyboxMaterial);
	render_object_attach(skyboxObj, skyboxMesh);

	render_object_t* obj2 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj2, greenMaterial);
	render_object_attach(obj2, planeMesh);
	render_object_set_transform(obj2, mat4_translation(float)(0, -0.5f, 0));

	render_object_t* obj4 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj4, blueMaterial);
	render_object_attach(obj4, mesh);
	render_object_set_transform(obj4, mat4_mul(float)(2, mat4_scale(float)(0.5f, 0.5f, 0.5f), mat4_translation(float)(-0.8f, 0.1f, 1)));

	render_object_t* obj1 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(obj1, blueMaterial);
	render_object_attach(obj1, mesh);
	render_object_set_transform(obj1, mat4_translation(float)(0, 0, 0));

	AUTO uiShaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI);
	AUTO uiShaderH2 = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2);
	AUTO uiMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, uiShaderH, "UIMaterial"));
	AUTO uiMaterial2 = material_library_getH(mlib, material_library_create_materialH(mlib, uiShaderH2, "UIMaterial2"));

	render_scene_add_queue(scene, RENDER_QUEUE_TYPE_QUEUE0);
	AUTO quadMesh = mesh_create(renderer, mesh3d_plane(400));
	render_object_t* obj5 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(obj5, uiMaterial);
	render_object_attach(obj5, quadMesh);
	render_object_set_transform(obj5, mat4_mul(float)(2, mat4_translation(float)(0, 0, -200), mat4_rotation(float)(0, 0, 90 DEG)));
	material_set_vec4(uiMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));

	render_object_t* obj6 = render_scene_getH(scene, render_scene_create_object(scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(obj6, uiMaterial2);
	render_object_attach(obj6, quadMesh);
	render_object_set_transform(obj6, mat4_mul(float)(2, mat4_translation(float)(0, 0, 200), mat4_rotation(float)(0, 0, 90 DEG)));
	material_set_vec4(uiMaterial2, "parameters.color", vec4(float)(1, 1, 1, 1));


	vulkan_texture_create_info_t create_info = 
	{
		.width = renderer_get_window(renderer)->width,
		.height = renderer_get_window(renderer)->height,
		.depth = 1,
		.channel_count = 4,
		.type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
		.initial_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.usage = VULKAN_TEXTURE_USAGE_RENDER_TARGET,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.technique = VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH
	};
	vulkan_texture_t* colorRenderTexture = vulkan_texture_create(renderer->vulkan_handle, &create_info);

	create_info.type = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
	vulkan_texture_t* depthRenderTexture = vulkan_texture_create(renderer->vulkan_handle, &create_info);

	material_set_texture(uiMaterial, "albedo", depthRenderTexture);
	material_set_texture(uiMaterial2, "albedo", colorRenderTexture);
	
	camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, colorRenderTexture);
	camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, depthRenderTexture);

	bool swap = true;
	float angle = 0;
	float speed = 0;
	int camera_index = 0;
	time_handle_t tHandle = time_get_handle();
	camera_t* previous_camera = camera;
	while(renderer_is_running(renderer))
	{
		speed = 20;
		if(kbhit())
		{
			getch();
			// camera_set_active(previous_camera, false);
			switch(camera_index % 2)
			{
				case 1:
					// camera_set_active(camera2, true);
					// previous_camera = camera2;
					camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, colorRenderTexture);
					camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, depthRenderTexture);
					camera_set_active(camera, true);
					break;
				// case 1:
				// 	camera_set_active(camera3, true);
				// 	previous_camera = camera3;
				// 	break;
				case 0:
					// camera_set_active(camera, true);
					// previous_camera = camera;
					camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_SCREEN);
					camera_set_render_target(camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_SCREEN);
					camera_set_active(camera, false);
					break;
			}
			camera_index++;
			swap = !swap;
		}
		// else speed = 50.0f;
		float deltaTime = time_get_delta_time(&tHandle);
		angle += deltaTime * speed;
		render_object_set_transform(obj1, mat4_rotation(float)(0 DEG, angle DEG, 0 DEG));
		// light_set_position(light, vec3(float)(0, 0.5f, sin(angle * 2 DEG)));
		// camera_set_rotation(camera3, vec3(float)(10 DEG, -90 DEG * sin(angle DEG), 0));

		renderer_begin_frame(renderer);

		// activate the offscreen camera and exclude the queue QUEUE0
		camera_set_active(camera3, true);
		render_scene_render(scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY)
								 | BIT64(RENDER_QUEUE_TYPE_BACKGROUND), RENDER_SCENE_CLEAR);
		
		// deactivate the offscreen camera and render only for QUEUE0
		camera_set_active(camera3, false);
		render_scene_render(scene, BIT64(RENDER_QUEUE_TYPE_QUEUE0), RENDER_SCENE_DONT_CARE);

		renderer_end_frame(renderer);

		renderer_update(renderer);
	}

	texture_destroy(colorRenderTexture);
	texture_release_resources(colorRenderTexture);

	texture_destroy(depthRenderTexture);
	texture_release_resources(depthRenderTexture);

	texture_destroy(texture);
	texture_release_resources(texture);

	texture_destroy(skyboxTexture);
	texture_release_resources(skyboxTexture);

	mesh_destroy(quadMesh);
	mesh_release_resources(quadMesh);
	mesh_destroy(planeMesh);
	mesh_release_resources(planeMesh);
	mesh_destroy(mesh);
	mesh_release_resources(mesh);

	mesh_destroy(skyboxMesh);
	mesh_release_resources(skyboxMesh);

	render_scene_destroy(scene);
	render_scene_release_resources(scene);

	light_destroy(light);
	light_release_resources(light);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
