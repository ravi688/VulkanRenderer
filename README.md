[![MSYS2](https://github.com/ravi688/VulkanRenderer/actions/workflows/msys2.yml/badge.svg)](https://github.com/ravi688/VulkanRenderer/actions/workflows/msys2.yml)
[![Ubuntu](https://github.com/ravi688/VulkanRenderer/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ravi688/VulkanRenderer/actions/workflows/ubuntu.yml)

![Spectrum-VulkanRenderer-Small](https://github.com/ravi688/VulkanRenderer/assets/67525292/5ec8fd4e-97e7-48aa-ab8d-cc808917e6ad)


# Spectrum Graphics Engine

**SGE** (formerly Vulkan Renderer) is a cross platform 2D and 3D graphics engine primarly designed for high quality text rendering, and 2D/3D geometry rendering.
For now it is based on Vulkan API, and in future, more API backends will be implemented, such as DirectX 12. Currently it works on Debian based Linux Distros and Windows 64-bit.

<a href="https://www.buymeacoffee.com/raviprakashsingh" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>

## Features

### Multipass Rendering

1. Render Pass Pool to avoid pass duplication
2. Sorting Render Passes using Topological Sort to minimize multiple runs of the same render pass.

### Render Queues

1. Background Render Queue

2. Geometry Render Queue

3. Overlay Render Queue

4. General Purpose Render Queues

### Render Textures  (Hybrid render textures)
A camera can render its depth and color output to a user supplied textures - both at the same time.

1. **Depth Render Texture** <br>
   One can redirect the depth render output of a camera to a depth render texture

2. **Color Render Texture** <br>
   One can redirect the color render output of a camrea to a color render texture

3. **Depth Cube Render Texture (Shadow map for point lights)** <br>
   One can redirect the depth render output of 360 view (6 faces of a cube) to a cube depth render texture - which can further be used as a shadow map for point lights.

4. **Color Cube Render Texture (Environment map for reflection effects)** <br>
   One can redirect the color render output of 360 view (6 faces of a cube) to a cube color render texture - which can further be used as a environment reflection map.

### Textures and Cubemaps

1. Normal Map

2. Albedo Map

3. Skybox &  Reflection cubemap

### Camera System

1. Multiple Cameras

2. Render Target switching

3. Orthographic and Perspective Projection
4. Split rendering

### Lights

1. Directional lights

2. Point lights

3. Spot lights

### Meshes

1. Support for ASCII STL, BINARY STL

2. Support for ASCII OBJ

3. Mesh Trangulation and Quadrangulation

4. Tangent Vector Generation

### Text Mesh Rendering

1. Glyph pool to avoid glyph mesh duplication

2. High performance internal data structure to frequently update the text data

3. Text Mesh batching

### Bitmap Text Rendering
1. Glyph Bitmap Atlas to avoid glyph bitmap duplication
2. Text Layout Callback to customize the placement of glyphs.

### Materials

1. String to fast integer handles for constant lookup

### Shaders

1. Multiple Render passes and Subpasses (fully control over the input and output attachments)

2. Custom attachments configuration

3. Fixed Function pipeline configuration for each pass

## Tested on

1. Windows 11 64 bit
2. Windows 10 64 bit
3. Linux Mint 64 bit + Nvidia Geforce GTX 1650
4. Nvidia GeForce GTX 1650 4 GB (VRAM) Discrete Mobile GPU + 8 GB of Main Memory (RAM)
5. AMD Radeon(TM) Graphics 512 MB (VRAM) Integratred Mobile GPU + 8 GB of Main Memory (RAM)
6. AMD Vega 8 Graphics 512 MB (VRAM) Integrated Mobile GPU  + 12 GB of Main Memory (RAM)

## Getting Started

### Requirements for building on Windows

1. **[Msys2](https://www.msys2.org/)**
2. **Vulkan SDK**

3. **MINGW 11.2.0** (tested on this, but might also work with previous versions), you can check if it is already installed in your machine by running <br>
   
   ```
   $ gcc --version
   ```
   
   OR
   
   ```
   $ g++ --version
   ```
   
   If this isn't already installed, run the following in MSYS2 MinGW shell
   
   ```
   $ pacman -S gcc
   ```

4. **GNU Make 4.3**, you can check if it is already installed, though it already comes with mingw64 binutils package, by running 
   
   ```
   $ make --version
   ```
   
   If this isn't already installed, run the following in MSYS2 MinGW shell
   
   ```
   $ pacman -S make
   ```

5. **Git version 2.35.1**, git must be installed in your machine, you can check if it is already installed by running <br>
   
   ```
   $ git --version
   ```
   
   If this isn't already installed, run the following in MSYS2 MinGW shell
   
   ```
   $ pacman -S git
   ```

6. **glslc**, glslc can be installed as follows, run the following in MSYS2 MinGW shell <br>
   
   ```
   $ pacman -S mingw-w64-x86_64-shaderc
   ```

### Requirements for building on Linux (Debian)

1. **Vulkan Tools**

   ```
   $ sudo apt-get install vulkan-tools
   ```

2. **Vulkan SDK**

   * Download vulkan sdk for linux (debian) from https://www.lunarg.com/vulkan-sdk/
   * Extract the archive by `tar -xvf <archive name>`
   * Change directoy into the extracted folder, usually name as version number `1.x.x.x`
   * Export SDK environment variables, `source ./setup-env.sh`
   * Or you may also just install vulkan-sdk via apt, `sudo apt-get install vulkan-sdk`

3. **GCC 11.2.0** (tested on this, but might also work with previous versions), you can check if it is already installed in your machine by running <br>
   
   ```
   $ gcc --version
   ```
   
   OR
   
   ```
   $ g++ --version
   ```
   
   If this isn't already installed, run the following in the terminal
   
   ```
   $ sudo apt-get install gcc
   ```
   You might also need to install `build-essentials` in case you encounter any standar library header inclusion errors:
   ```
   $ sudo apt-get update
   $ sudo apt-get install build-essentials
   ```

4. **GNU Make 4.3**, you can check if it is already installed, though it already comes with mingw64 binutils package, by running 
   
   ```
   $ make --version
   ```
   
   If this isn't already installed, run the following in the terminal
   
   ```
   $ sudo apt-get install make
   ```

5. **Git version 2.35.1**, git must be installed in your machine, you can check if it is already installed by running <br>
   
   ```
   $ git --version
   ```
   
   If this isn't already installed, run the following in the terminal
   
   ```
   $ sudo apt-get install git
   ```
6. You might also need to install the followings:
   ```
   $ sudo apt-get install libz-dev
   $ sudo apt-get install libpng-dev
   $ sudo apt-get install libbrotli-dev
   ```

### Runtime requirements

1. Windows 64 bit
2. GPU supporting vulkan api (integrated or discrete)
3. Main memory (RAM) - No data as of now
4. Disk space - No data as of now
5. Better to have Vulkan LunarG SDK installed for additional debugging and vulkan configuration (validation layers), but it is not a requirement because the static library and headers are already included in the repository and would be updated as new updates will come in future.

### Building steps

1. Clone the repository by running the following command <br>
   
   ```
   $ git clone https://github.com/ravi688/VulkanRenderer.git
   ```

2. Change the working directory to `VulkanRenderer` and setup all the dependency git submodules by running the following command
   
   ```
   $ cd VulkanRenderer
   $ make -s setup
   $ make -s init
   ```

3. Start building by running the following command
   
   ```
   $ make -s build
   ```
   
   OR
   
   ```
   $ make -s build-debug
   ```
   
   For release mode
   
   ```
   $ make -s build-release
   ```

### Building executable manually (Optional)

1. Change the working directory to `VulkanRenderer` and build the `main` executable by running the following command
   
   ```
   $ make -s debug
   ```

2. Now run the `main` executable by running the following command
   
   ```
   $ ./main
   ```

### Test Run (Optional)
There are several tests which you can try running by just passing arguments:
```
$ cd <build directory>
$ ./main CUBE
```
The above set of commands would launch a window in which a white cube will be spinning.
If you want to see all the possible test cases, you may launch the execution without any arguments and it would just print the list of possible test cases:
```
$ ./main
supported tests:
        DEPTH_RENDER_TEXTURE
        DEPTH_RENDER_TEXTURE_LOAD
        ENVIRONMENT_REFLECTIONS
        ENVIRONMENT_REFLECTIONS_LOAD
        DEPTH_CUBE_RENDER_TEXTURE
        DEPTH_CUBE_RENDER_TEXTURE_LOAD
        POINT_LIGHT_SHADOWS
        POINT_LIGHT_SHADOWS_LOAD
        SPOT_LIGHT
        SPOT_LIGHT_LOAD
        CUBE
        TEXTURE_SAMPLING
        TEXTURE_SAMPLING_ALPHA_CHANNEL
        TEXT_MESH
        BITMAP_TEXT
        TID_14_CASE_1
        TID_14_CASE_2
        TID_14_CASE_3
        TID_28_CASE_1
        TID_28_CASE_2
        TID_28_CASE_3
        TID_28_CASE_4
        TID_42_CASE_1
        TID_43_CASE_1
        TID_43_CASE_2
        TID_43_CASE_3
        TID_43_CASE_4
        TID_48_CASE_1
        TID_48_CASE_2
        TID_48_CASE_3
        TID_48_CASE_4
        TID_48_CASE_5
```

### Cleaning everything  (Optional)

1. Change the working directory to `VulkanRenderer` and run the following command
   
   ```
   $ make -s clean
   ```

## Documentation
The full documentation will be available very soon, however, for now you can have a look at the [Wiki](https://github.com/ravi688/VulkanRenderer/wiki).

## Shader Description Language Example
```GLSL
#sb version 2023
#sl version 2023

[Name("BitmapTextShader")]
Shader
{
    Properties
    {
        struct Color
        {
            float r;
            float g;
            float b;
        }

        [Stage(vertex, fragment)]
        [Set(material_set, material_properties)]
        uniform Parameters
        {
            vec2 tex_size;
            Color color;
            int space_type;
            int surface_type;
        } parameters;

        [Stage(fragment)]
        [Set(material_set, texture0)]
        uniform sampler2D bga;

        struct GTC
        {
            vec2 tltc;
            vec2 trtc;
            vec2 brtc;
            vec2 bltc;
        }

        [Stage(vertex, fragment)]
        [Set(material_set, texture1)]
        uniform GTCBuffer
        {
            GTC texcoords[];
        } GTCBuffer;

        [Stage(vertex)]
        [Set(material_set, texture2)]
        uniform TSTBuffer
        {
            mat4 transforms[];
        } TSTBuffer;
    }

    Layout
    {
        [Rate(per_vertex)]
        [MeshLayout(sge_optimal)]
        [Attribute(position)]
        vec3 position;

        [Rate(per_instance)]
        [Attribute(binding=5,location=5)]
        vec4 ofst_indx;

        [Rate(per_instance)]
        [Attribute(binding=5,location=6)]
        vec4 rotn_stid;

        [Rate(per_instance)]
        [Attribute(binding=5,location=7)]
        vec4 rotn_stid;
    }
    
    RenderPass
    {
        SubPass
        {
            [NoParse]
            GraphicsPipeline
            {
                colorBlend
                {
                    attachment { }
                }
            }
            [NoParse]
            GLSL
            {
                #stage vertex
        
                #version 450
                
                #include <v3d.h>
                
                layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = SCREEN_BINDING) uniform DisplayInfo
                {
                    uvec2 resolution;
                    uvec2 dpi;
                    uvec2 window_size;
                    mat4 matrix;
                } displayInfo;
                
                layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
                layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;
                
                struct Color
                {
                    float r;
                    float g;
                    float b;
                };
                
                layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
                {
                    uvec2 tex_size;
                    Color color;
                    int space_type;
                    int surface_type;
                } parameters;
                
                struct GlyphTexCoord
                {
                    vec2 tltc;
                    vec2 trtc;
                    vec2 brtc;
                    vec2 bltc;
                };
                
                layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = TEXTURE_BINDING1) uniform GTCBuffer
                {
                    GlyphTexCoord[512] gtc_buffer;
                };
                
                layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = TEXTURE_BINDING2) uniform TSTBuffer
                {
                    mat4[128] tst_buffer;
                };
                
                layout(location = POSITION_LOCATION) in vec3 position;
                
                layout(location = 5, component = 0) in vec3 ofst;
                layout(location = 5, component = 3) in float indx_f;
                layout(location = 6, component = 0) in vec3 rotn; // not required
                layout(location = 6, component = 3) in float stid_f;
                layout(location = 7, component = 0) in vec3 scal; // not required
                
                layout(location = 0) out vec2 out_texcoord;
                layout(location = 1) out vec3 out_color;
                
                void main()
                {
                    uint indx = floatBitsToUint(indx_f);
                    uint stid = floatBitsToUint(stid_f);
                
                    GlyphTexCoord texcoord = gtc_buffer[indx];
                
                    vec2 tex_size = parameters.tex_size;
                    vec2 win_size = vec2(displayInfo.window_size);
                    vec2 glyph_size = vec2(texcoord.trtc.x - texcoord.tltc.x, texcoord.bltc.y - texcoord.tltc.y) * tex_size;
                
                    vec4 pos = vec4((position.x * glyph_size.x + ofst.x) * 0.5, (position.y * glyph_size.y + ofst.y) * 0.5, 0, 1.0);
                
                    vec4 world = objectInfo.transform * tst_buffer[stid] * pos.zyxw;
                
                    vec4 clipPos;
                
                    switch(parameters.space_type)
                    {
                        case TEXT_RENDER_SPACE_TYPE_2D:
                        {
                            switch(parameters.surface_type)
                            {
                                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                                    clipPos = cameraInfo.screen * world;
                                    break;
                                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                                    clipPos = displayInfo.matrix * world;
                                    break;
                            }
                            break;
                        }
                        case TEXT_RENDER_SPACE_TYPE_3D:
                        {
                            switch(parameters.surface_type)
                            {
                                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                                    clipPos = cameraInfo.projection * cameraInfo.view * world;
                                    break;
                                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                                    clipPos = displayInfo.matrix * world;
                                    break;
                            }
                            break;
                        }
                    }
                
                    gl_Position = vec4(clipPos.x, -clipPos.y, clipPos.z, clipPos.w);
                
                    if(gl_VertexIndex == 0)
                        out_texcoord = texcoord.tltc;
                    else if(gl_VertexIndex == 1)
                        out_texcoord = texcoord.trtc;
                    else if(gl_VertexIndex == 2)
                        out_texcoord = texcoord.brtc;
                    else if(gl_VertexIndex == 3)
                        out_texcoord = texcoord.bltc;
                
                    out_color = vec3(parameters.color.r, parameters.color.g, parameters.color.b);
                }

                #stage fragment
                
                #version 450
                
                #include <v3d.h>
                                
                layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D bga;
                
                layout(location = 0) in vec2 in_texcoord;
                layout(location = 1) in vec3 in_color;
                
                layout(location = 0) out vec4 color;
                
                void main()
                {
                    vec2 texcoord = vec2(in_texcoord.x, in_texcoord.y);
                    color = vec4(in_color.x, in_color.y, in_color.z, 1.0) * texture(bga, texcoord).r;
                }
            }
        }
    }

}
```

## Screenshots and video clips

#### Point Lights

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/V7vfsykNBPY/0.jpg)](https://www.youtube.com/watch?v=V7vfsykNBPY)

#### Spot Lights

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/I390_yvlLE8/0.jpg)](https://www.youtube.com/watch?v=I390_yvlLE8)

#### Render Textures

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Ti5y9OSK6Ow/0.jpg)](https://www.youtube.com/watch?v=Ti5y9OSK6Ow)

#### Skybox Rendering

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/f3wqkk3p7MA/0.jpg)](https://www.youtube.com/watch?v=f3wqkk3p7MA)

#### Mesh Rendering

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Jh2ViGbWrrE/0.jpg)](https://www.youtube.com/watch?v=Jh2ViGbWrrE)

#### Specular Shading

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/nqGjmHLH4jk/0.jpg)](https://www.youtube.com/watch?v=nqGjmHLH4jk)

#### Diffuse Shading

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/NQ_kysdMxHg/0.jpg)](https://www.youtube.com/watch?v=NQ_kysdMxHg)

#### Color Blending and Text Rendering

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/2jXi0bTWkC4/0.jpg)](https://www.youtube.com/watch?v=2jXi0bTWkC4)

#### Flat Shading

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/6K_JPCuZfFM/0.jpg)](https://www.youtube.com/watch?v=6K_JPCuZfFM)

## About Me (Open for Hire)

#### [LinkedIn](https://www.linkedin.com/in/ravi-prakash-singh-095a271a8/)
#### [Github](https://github.com/ravi688)
#### [Medium](https://medium.com/@rp0412204)
#### [Youtube](https://www.youtube.com/channel/UCWe_os3p4z3DBnQ4B5DUTfw/videos)
