# VulkanRenderer
3D renderer built on Vulkan API
NOTE: Currently only nvidia dedicated gpus are supported.

## Setting up dependencies/submodules
1. change the working directory to VulkanRenderer
2. run "make setup"

## Building shaders
1. change the working directory to VulkanRenderer/shader_compiler
2. run "make debug"
3. change the working directory to VulkanRenderer/testbed
4. run "make shader"

## Building executable
1. change the working directory to VulkanRenderer/testbed
2. run "make debug"
3. run "main.exe"
