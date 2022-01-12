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
