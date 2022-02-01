# VulkanRenderer
3D renderer built on Vulkan API <br>
~~Note: Currently only nvidia dedicated gpus are supported.~~

## Setting up dependencies/submodules
1. change the working directory to `VulkanRenderer`
2. run `make setup`

## Building shaders
1. change the working directory to `VulkanRenderer/shader_compiler`
2. run `make debug`
3. include `VulkanRenderer/shader_compiler/shader_compiler.exe` in your PATH environment variable (Windows)
4. change the working directory to `VulkanRenderer/testbed`
5. run `make shader`

## Building executable
1. change the working directory to `VulkanRenderer/testbed`
2. run `make debug`
3. run `main.exe`

## Where to go for examples?
You can check `VulkanRenderer/testbed' directory

## Features

### Shader Binary
`Shader defintion` (`example_shader.glsl`) source files are first compiled into a `Shader Binary` files (`example_shader.sb`) by `shader_compiler.exe` executable.
After the compilation, the shader binary files consumed as follows:
```GLSL 

shader_t* example_shader = shader_load(renderer, "resource/shaders/example_shader.sb");

// use the shader

shader_destroy(renderer, example_shader); // destroys the vulkan objects
shader_release_resources(example_shader); // releases some extra heap allocated memory
```

### Shader defintiion

#### Sections
Shader definition has been devided into 3 sections: `SETTINGS` `LAYOUT` `SHADER`. <br>
`SHADER` section is devided into stages: `vertex` `tessellation` `geometry` `fragment`. <br>
Currently `tessellation` and `geometry` are not supported.
```GLSL
#section SETTINGS       // optional, to be added. As of now it is always ignored.

// settings for fixed function stages in the graphics pipeline

#section LAYOUT         // optional

// all the push constants and descriptor sets definition should go inside this section

#section SHADER         // mandatory

#stage vertex 			// mendatory for now

// GLSL vertex shader

#stage fragment 		// mendatory for now

// GLSL fragment shader
```

#### Push Constants
   Each block represents a push constant range having an offset and the size would automatically be calculated.
   Numerical value in the square brackets after `[push_constant]` represents the offset of the push constant range in the supported buffer size by the GPU.
```GLSL
vertex fragment [push_constant] [0] uniform Push
{
    mat4 mvp_matrix;
    mat4 model_matrix;
} push;
```
```GLSL
vertex [push_constant] [0] uniform Push
{
    mat4 mvp_matrix;
} push;

fragment [push_constant] [64] uniform Push
{
    mat4 model_matrix;
} push;
```

#### Descriptor Sets
   First value in the square brackets represents the descriptor set number and the second value represents the binding number.
```GLSL
fragment [0, 0] uniform Light
{
    vec3 color;
    vec3 dir;
    float intensity;
} light;

vertex [0, 1] uniform Matrices
{
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 normal_matrix;
} matrices;

fragment vertex [0, 2] uniform Sphere
{
    float radius;
    vec3 color;
} sphere;

fragment [0, 3] uniform sampler2D albedo;
fragment [0, 4] uniform sampler2D normal_map;

```

#### 3D Mesh loading
Supported file formats are `ASCII STL` `Binary STL` `ASCII OBJ` <br>
STL files could be ASCII or Binary, it is automatically detected when we load the file.
As of now only polygonal 3d models are supported, no curves and surfaces as it could be in the OBJ files.
```GLSL
// load stl file, it automatically detects ASCII and Binary stl files.
mesh3d_t* sphere = mesh3d_load("resources/Sphere.stl"); 

// load obj file, it supports triangulated and quadrangulated meshes both.
mesh3d_t* monkey = mesh3d_load("resources/Monkey.obj");

// create renderable meshes (`mesh_t`)
mesh_t* sphere_mesh = mesh_create(renderer, sphere);
mesh_t* monkey_mesh = mesh_create(renderer, monkey);

// render the meshes by called mesh3d_draw_* kind of functions

mesh3d_destroy(sphere);       // destroy and release memory
mesh3d_destroy(monkey);       // destroy and release memory
// no mesh3d_release_resources kind of function because mesh3d_t object isn't related to vulkan at all. 
// mesh3d is just a data structure to store 3d meshes.

mesh_destroy(sphere_mesh, renderer); // destroy vulkan objects
mesh_release_resources(sphere_mesh); // release heap memory

mesh_destroy(monkey_mesh, renderer); // destroy vulkan objects
mesh_release_resources(moneky_mesh); // release heap memory
```

#### 2D Textures
Supported file formats are: `Windows BMP`

```GLSL
// load the texture data and create required vulkan objects
texture_t* texture = texture_load(renderer, "resource/textures/linuxlogo.bmp");

// use the texture somewhere

// destroy vulkan objects
texture_destroy(texture, renderer);
// release heap allocated memory
texture_release_resources(texture);
```

#### Materials

All the descriptors defined in the shader definition file could be set by material_set_* functions.
All the push constants defined in the shader definition file could be set by material_set_push_* functions.

```GLSL
// prepare the mesh data structure
mesh3d_t* box_mesh3d = mesh3d_load("resource/Box.obj");
mesh3d_make_centroid_origin(box_mesh3d); // set the model's origin equal to its centroid, optional
mesh3d_calculate_tangents(box_mesh3d);   // calculate tangents for normal mapping

// create renderable meshe object
mesh_t* box = mesh_create(renderer, box_mesh3d);

// load the shader binary file
shader_t* box_shader = shader_load(renderer, "resource/shaders/bump_shader.sb");

u64 per_vertex_attributes[4];
per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); // vertex buffer for vertex positions
per_vertex_attributes[1] = MATERIAL_ALIGN(MATERIAL_VEC3, 1); // vertex buffer for vertex normal positions
per_vertex_attributes[2] = MATERIAL_ALIGN(MATERIAL_VEC2, 2); // vertex buffer for vertex texture coordinates
per_vertex_attributes[3] = MATERIAL_ALIGN(MATERIAL_VEC3, 3); // vertex buffore for vertex tangents

// prepare the material create info
material_create_info_t box_material_info = 
{
	.per_vertex_attribute_binding_count = 4,     // we want four separate vertex buffers, not interleaved
	.per_vertex_attribute_bindings = &per_vertex_attributes[0],
	.shader = box_shader, 
   .is_transparent = false    // optional
};

// create the material
material_t* box_material = material_create(renderer, &box_material_info);

// load the required textures
texture_t* box_textures[] = 
{ 
	texture_load(renderer, "resource/textures/white.bmp"),
	texture_load(renderer, "resource/textures/normal_map.bmp")
};

// set the albedo texture to its corresponding slot in the shader
material_set_texture2d(box_material, "albedo", box_textures[0]);

// set the normal map texture to its corresponding slot in the shader
material_set_texture2d(box_material, "normal_map", box_textures[1]);

// set the transformation matrices, note: uniform buffers could also be used instead of push constants
material_set_push_mat4(box_material, "push.mvp_matrix", mat4_transpose(float)(mvp));
material_set_push_mat4(box_material, "push.model_matrix", mat4_transpose(float)(model_matrix));

// bind the material by calling material_bind(box_material, renderer)
// render the mesh by calling mesh_draw_* functions

// destroy and release mesh_t, mesh3d_t, shader, and textures

// destroy vulkan objects
material_destroy(box_material, renderer);

// release heap allocated memory
material_release_resources(box_material);
```

#### Text Rendering

Currently on text meshes are support, meaning all the glyphs are being tessellated and rendered as complex polygons.

Texts are being accomplished by `font_t` `text_mesh_t` `glyph_mesh_pool_t` and `text_mesh_string_handle_t` objects. <br>

**Font**: 
```GLSL

// load the font data and allocate the required memory
font_t* font = font_load_and_create("resource/fonts/Pushster-Regular.ttf");

// use the font somewhere

// destroys and releases the memory allocated by the backed third party library for parsing the true-type fonts
font_destroy(font);    
// release the heap memory allocated by the font_t object
font_release_resources(font);
```

**Glyph Mesh Pool**:
```GLSL

// load the font data and allocate the required memory
font_t* font = font_load_and_create("resource/fonts/Pushster-Regular.ttf");

// create glyph mesh pool to create renderable meshes (`mesh_t`)
glyph_mesh_pool_t* pool = glyph_mesh_pool_create(renderer, font);

// creates a new `mesh_t` because it is the first query for the glyph 'A'
mesh_t* glyph_A = glyph_mesh_pool_get_mesh(pool, 'A'); 
// creates a new `mesh_t` because it is the first query for the glyph 'A'
mesh_t* glyph_B = glyph_mesh_pool_get_mesh(pool, 'B');
// returns the already created `mesh_t` object for the glyph 'A', so value of glyph_A2 must be equals to glph_A
mesh_t* glph_A2 = glyph_mesh_pool_get_mesh(pool, 'A');

// render the meshes by calling mesh_draw_* kind of functions

// destroys and releases the memory allocated by the backed third party library for parsing the true-type fonts
font_destroy(font);    
// release the heap memory allocated by the font_t object
font_release_resources(font);

// destoy vulkan objects
glyph_mesh_pool_destroy(pool); 
// release heap allocated memory
glyph_mesh_pool_release_resources(pool); 

```

**Text Mesh and String Handles**:
```GLSL
// load the font data and allocate the required memory
font_t* font = font_load_and_create("resource/fonts/Pushster-Regular.ttf");

// create glyph mesh pool to create renderable meshes (`mesh_t`)
glyph_mesh_pool_t* pool = glyph_mesh_pool_create(renderer, font);

// create text mesh object
text_mesh_t* game_ui = text_mesh_create(renderer, pool);

// create a string, note: these string instances would be pooled automatically
text_mesh_string_handle_t score_text = text_mesh_string_create(game_ui);
// set the position
text_mesh_string_set_positionH(game_ui, score_text, vec3(float)(0, 7, 5));
// set the scale
text_mesh_string_set_scaleH(game_ui, score_text, vec3(float)(0.8f, 0.8f, 0.8f));
// set the string value, note: it allocates heap memory internally if the current size of the internal buffer is less than the required.
text_mesh_string_setH(game_ui, score_text, "Score: 456");

// render the texts by calling text_mesh_draw function

// destroys and releases the memory allocated by the backed third party library for parsing the true-type fonts
font_destroy(font);    
// release the heap memory allocated by the font_t object
font_release_resources(font);

// destoy vulkan objects
glyph_mesh_pool_destroy(pool);
// release heap allocated memory
glyph_mesh_pool_release_resources(pool);

// no need to destroy string handles because they are pooled inside the text_mesh_t object.

// destroy vulkan objects
text_mesh_destroy(game_ui);
// release heap allocated memory
text_mesh_release_resources(game_ui);

```

## More?
#### [Youtube](https://www.youtube.com/channel/UCWe_os3p4z3DBnQ4B5DUTfw/videos)

## About Me
#### [LinkedIn](https://www.linkedin.com/in/ravi-prakash-095a271a8/)
