# VulkanRenderer
3D renderer built on Vulkan API
NOTE: Currently only nvidia dedicated gpus are supported.

## Setting up dependencies/submodules
1. change the working directory to VulkanRenderer
2. run "make setup"

## Building shaders
1. change the working directory to VulkanRenderer/shader_compiler
2. run "make debug"
3. include VulkanRenderer/shader_compiler/shader_compiler.exe in your PATH environment variable (Windows)
4. change the working directory to VulkanRenderer/testbed
5. run "make shader"

## Building executable
1. change the working directory to VulkanRenderer/testbed
2. run "make debug"
3. run "main.exe"


## Features

### Material/Shader defintiion

#### Sections
Shader definition has been devided into 3 sections: `SETTINGS` `LAYOUT` `SHADER`.
```GLSL
#section SETTINGS       // optional
// settings for fixed function stages in the graphics pipeline

#section LAYOUT         // optinal
// all the push constants and descriptor sets definition should go inside this section

#section SHADER         // mandatory
// GLSL shader
```

#### Push Constants
   Each block represents a push constant range having an offset and the size would automatically be calculated
   Value in the square brackets after `[push_constant]` represents the offset of the push constant range in the support buffer size by the GPU.
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
   First value in the square brackets represents the descriptor set number and the second one represents the binding number.
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
```
