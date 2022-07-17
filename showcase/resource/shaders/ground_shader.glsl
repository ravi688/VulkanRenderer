[Name("GroundShader")]
Shader
{
    // Descriptor Sets
    Properties
    {
        // Internal & Built-in sets are as follows:
        // GLOBAL_SET
        // RENDER_SET
        // SUB_RENDER_SET
        // OBJECT_SET

        fragment [MATERIAL_SET, TEXTURE_BINDING0] uniform sampler2D normalMap;
        fragment [MATERIAL_SET, TEXTURE_BINDING1] uniform sampler2D normalMap2;
        fragment [MATERIAL_SET, TEXTURE_BINDING2] uniform sampler2D specularMap;
        fragment [MATERIAL_SET, TEXTURE_BINDING3] uniform sampler2D heightMap;

        fragment vertex [MATERIAL_SET, MATERIAL_PROPERTIES_BINDING] uniform Parameters
        {
            float specularity;
            float roughness;
            float reflectance;
        } parameters;
    }

    // Vertex Attribute Descriptions
    VertexBufferLayout
    {
        per-vertex [0, POSITION_LOCATION] vec3 position;
        per-vertex [1, NORMAL_LOCATION] vec3 normal;
        per-vertex [2, TEXCOORD_LOCATION] vec2 texcoord;
        per-vertex [3, COLOR_LOCATION] vec4 color;
        per-vertex [4, TANGENT_LOCATION] vec3 tangent;
    }
        
    RenderPass
    {
        SubPass
        {
            GraphicsPipeline
            {
                // no color attachments
                colorBlend { } 
                depthStencil
                {
                    // enabling test or write attaches a depth buffer to this render pass
                    depthTestEnable = true
                    depthWriteEnable = true
                }
            }

            // GLSL code
            glsl
            {
                #stage vertex

                #version 450    
                #include <v3d.h>
    
                layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo lightInfo;
                layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;
    
                layout(location = POSITION_LOCATION) in vec3 position;
    
                void main()
                {
                    gl_Position = lightInfo.projectionMatrix * lightInfo.viewMatrix * objectInfo.modelMatrix * vec4(position, 1);
                }
            }
        }
    }

    [Input(depth, set = RENDER_SET, binding = TEXTURE_BINDING0)]
    RenderPass
    {
        SubPass
        {
            GraphicsPipeline
            {
                // one color attachment
                colorBlend
                {
                    attachment { }
                }
                // one depth attachment
                depthStencil
                {
                    depthTestEnable = true
                    depthWriteEnable = true
                }
            }

            glsl
            {
                #stage vertex
                
                #version 450
                #include <v3d.h>

                layout(set = GLOBAL_SET, binding = CAMERA_BINDING) uniform CameraInfo cameraInfo;
                layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

                layout(location = POSITION_LOCATION) in vec3 position;
                layout(location = NORMAL_LOCATION) in vec3 normal;
                layout(location = COLOR_LOCATION) in vec4 color;
                layout(location = TEXCOORD_LOCATION) in vec2 texcoord;
                layout(location = TANGENT_LOCATION) in vec3 tangent;

                layout(location = 0) out vec2 _texcoord;
                layout(location = 1) out vec3 _normal;
                layout(location = 2) out vec3 _tangent;
                layout(location = 3) out vec4 _color;
                layout(location = 4) out vec4 _shadowPos;
                
                void main()
                {
                    _shadowPos = lightInfo.projectionMatrix * lightInfo.viewMatrix * objectInfo.modelMatrix * vec4(position, 1);
                    _normal = (objectInfo.normalMatrix * vec4(normal, 0)).xyz;
                    _tangent = (objectInfo.normalMatrix * vec4(tangent, 0)).xyz;
                    _color = color;
                    _texcoord = texcoord;
                    gl_Position = cameraInfo.projectionMatrix * cameraInfo.viewMatrix * objectInfo.modelMatrix * vec4(position, 1);
                }

                #stage fragment

                #version 450
                #include <v3d.h>

                layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo lightInfo;

                layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;


                layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D normalMap;
                layout(set = MATERIAL_SET, binding = TEXTURE_BINDING1) uniform sampler2D normalMap2;
                layout(set = MATERIAL_SET, binding = TEXTURE_BINDING2) uniform sampler2D specularMap;
                layout(set = MATERIAL_SET, binding = TEXTURE_BINDING3) uniform sampler2D heightMap;

                layout(location = 0) in vec2 _texcoord;
                layout(location = 1) in vec3 _normal;
                layout(location = 2) in vec3 _tangent;
                layout(location = 3) in vec4 _color;
                layout(location = 4) in vec4 _shadowPos;

                layout(location = 0) out vec4 color;

                void main()
                {
                    float lightSpaceDepth = _shadowPos.z / _shadowPos.w;
                    vec2 shadowCoord = vec2(_shadowPos.x / _shadowPos.w, _shadowPos.y / _shadowPos.w);
                    shadowCoord.x = (shadowCoord.x + 1.0) / 2.0;            // map 0 -> 1
                    shadowCoord.y = (shadowCoord.y + 1.0) / 2.0;            // map 0 -> 1
                    float dist = texture(shadowMap, shadowCoord).r;
                    if((lightSpaceDepth - dist) > 0.001)
                        color = vec4(0.1, 0.1, 0.1, 1.0);
                    else
                    {
                        float t = max(0, dot(-lightInfo.dir, _normal));
                        vec3 light1 = t  * lightInfo.color * lightInfo.intensity;
                        vec4 lighting = vec4(light1, 1);
                        color = _color * lighting;
                    }
                }
            }
        }

        [Input(color = 0, set = SUB_RENDER_SET, binding = INPUT_ATTACHMENT_BINDING0)]
        SubPass
        {
            GraphicsPipeline
            {
                colorBlend
                {
                    attachment { }
                }
                depthStencil
                {
                    depthTestEnable = true
                    depthWriteEnable = true
                }
            }

            glsl
            {
                #stage vertex

                #version 450
                #include <v3d.h>

                layout(set = GLOBAL_SET, binding = CAMERA_BINDING) uniform CameraInfo cameraInfo;
                layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

                layout(location = POSITION_LOCATION) in vec3 position;

                void main()
                {
                    gl_Position = cameraInfo.projectionMatrix * cameraInfo.viewMatrix * objectInfo.modelMatrix * vec4(position, 1);
                }

                #stage fragment

                #version 450
                #include <v3d.h>

                layout(set = SUB_RENDER_SET, binding = INPUT_ATTACHMENT_BINDING0) uniform subpassInput previousColor;

                layout(location = 0) out vec4 color;

                void main()
                {
                    color = subpassLoad(previousColor).rgba;
                }
            }
        }
    }
}
