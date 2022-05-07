#section SETTINGS

[Name("ShadowMap")]
[PipelineLayout("ShadowMap")]
GraphicsPipeline
{
    // no color attachments
    // depth test and write is enabled by default
}

[Name("Color")]
[PipelineLayout("Color")]
GraphicsPipeline
{
    ColorBlend
    {
        // only one color attachment
        attachment
        {

        }
    }
    // depth test and write is enabled by default
}

#section LAYOUT

[Name("ShadowMap")]
PipelineLayout
{
    vertex [GLOBAL_SET, LIGHT_BINDING] uniform LightInfo
    {
        mat4 projectionMatrix;
        mat4 viewMatrix;
        vec3 dir;
        vec3 color;
        float intensity;
    } lightInfo;
    
    vertex [OBJECT_SET, TRANSFORM_BINDING] uniform ObjectInfo
    {
        mat4 modelMatrix;
        mat4 normalMatrix;
    } objectInfo;

    per-vertex [0, POSITION_LOCATION] vec3 position;
}

[Name("Color")]
PipelineLayout
{
    vertex [GLOBAL_SET, CAMERA_BINDING] uniform CameraInfo
    {
        mat4 projectionMatrix;
        mat4 viewMatrix;
    } cameraInfo;

    fragment [RENDER_SET, RENDER_INPUT_BINDING0] uniform sampler2D depthMap;

    fragment [GLOBAL_SET, LIGHT_BINDING] uniform LightInfo
    {
        mat4 projectionMatrix;
        mat4 viewMatrix;
        vec3 dir;
        vec3 color;
        float intensity;
    } lightInfo;

    vertex [OBJECT_SET, TRANSFORM_BINDING] uniform ObjectInfo
    {
        mat4 modelMatrix;
        mat4 normalMatrix;
    } objectInfo;

    per-vertex [0, POSITION_LOCATION] vec3 position;
    per-vertex [1, NORMAL_LOCATION] vec3 normal;
    per-vertex [2, TEXCOORD_LOCATION] vec2 texcoord;
}

#section SHADER

// Shadow map render pass
RenderPass
{
    [GraphicsPipeline("ShadowMap")]
    [Output(depth)]
    SubPass
    {
        #stage vertex

        #include <v3d.h>

        #version 450

        layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo
        {
            mat4 projectionMatrix;
            mat4 viewMatrix;
            vec3 dir;
            vec3 color;
            float intensity;
        } lightInfo;

        // local descriptor set (bound for each object/mesh)
        layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo
        {
            mat4 modelMatrix;
            mat4 normalMatrix;
        } objectInfo;


        // input attribute : POSITION
        layout(location = POSITION_LOCATION) in vec3 position;

        void main()
        {
            gl_Position = lightInfo.projectionMatrix * lightInfo.viewMatrix * objectInfo.modelMatrix * vec4(position, 1);
        }
    }
}

RenderPass
{
    [GraphicsPipeline("Color")]
    [Input(depth, set = RENDER_SET, binding = RENDER_INPUT_BINDING0)]
    SubPass
    {
        #stage vertex
        
        #include <v3d.h>

        #version 450

        // global descriptor set (bound once)
        layout(set = GLOBAL_SET, binding = CAMERA_BINDING) uniform CameraInfo
        {
            mat4 projectionMatrix;
            mat4 viewMatrix;
        } cameraInfo;

        // object descriptor set (bound for each object/mesh)
        layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo
        {
            mat4 modelMatrix;
            mat4 normalMatrix;
        } objectInfo;

        layout(location = POSITION_LOCATION) in vec3 position;
        layout(location = NORMAL_LOCATION) in vec3 normal;
        layout(location = TEXCOORD_LOCATION) in vec2 texcoord;
        
        layout(location = 0) out vec3 _normal;
        layout(location = 1) out vec2 _texcoord;
        
        void main()
        {
            _normal = (objectInfo.normalMatrix * vec4(normal, 0)).xyz;
            _texcoord = texcoord;
            gl_Position = cameraInfo.projectionMatrix * cameraInfo.viewMatrix * vec4(position, 1);
        }
        
        #stage fragment
        
        #include <v3d.h>

        #version 450

        layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo
        {
            mat4 projectionMatrix;
            mat4 viewMatrix;
            vec3 dir;
            vec3 color;
            float intensity;
        } lightInfo;

        // render descriptor set (bound for each render pass)
        layout(set = RENDER_SET, binding = RENDER_INPUT_BINDING0) uniform sampler2D depthMap;
        
        layout(location = 0) in vec3 _normal;
        layout(location = 1) in vec2 _texcoord;
        
        layout(location = 0) out vec3 color;
        
        void main()
        {
            color = vec3(1, 1, 1) * lightInfo.color * lightInfo.intensity * max(0, dot(-_normal, lightInfo.dir));
        }
    }
}
