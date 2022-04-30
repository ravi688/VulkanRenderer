#section SETTINGS


GraphicsPipeline
{
    InputAssembly
    {
        topology = trianglelist,
        primitiveRestartEnable = false
    }

    Rasterization
    {
        depthClampEnable = false,
        rasterizerDiscardEnable = false,
        polygonMode = fill,
        lineWidth = 1,
        cullMode = back,
        frontFace = clockwise,
        depthBiasEnable = false,
    }

    Viewport
    {
        viewport
        {
            x = 0,
            y = 0,
            width = 800,
            height = 800,
            minDepth = 0,
            maxDepth = 1
        }

        scissor
        {
            offset
            {
                x = 0,
                y = 0
            }

            extent
            {
                width = 800,
                height = 800
            }
        }
    }

    Multisample
    {
        sampleShadingEnable = false,
        rasterizationSamples = 1
    }

    ColorBlend
    {
        logicOpEnable = false,
        logicOp = copy,

        attachment
        {
            colorWriteMask = RGBA,
            blendEnable = false,
            srcColorBlendFactor = srcalpha,
            dstColorBlendFactor = oneminussrcalpha,
            colorBlendOp = add,
            srcAlphaBlendFactor = one,
            dstAlphaBlendFactor = zero,
            alphaBlendOp = add
        }
    }

    DepthStencil
    {
        depthTestEnable = true,
        depthWriteEnable = true,
        depthCompareOp = less,
        depthBoundsTestEnable = false,
        stencilTestEnable = false
    }
}


#section LAYOUT

per-vertex [0, 0] vec3 position;
per-vertex [1, 1] vec3 normal;
per-vertex [2, 2] vec2 texcoord;

vertex fragment [push_constant] [0] uniform Push
{
    mat4 mvp_matrix;
    mat4 model_matrix;
} push;

fragment [0, 0] uniform sampler2D albedo;
fragment [0, 1] uniform Light
{
    vec3 dir;
    float intensity;
    vec3 color;
} light;

#section SHADER

RenderPass
{
    SubPass
    {
        #stage vertex
        
        #version 450
        
        layout(push_constant) uniform Push
        {
            mat4 mvp_matrix;
            mat4 model_matrix;
        };
        
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 normal;
        layout(location = 2) in vec2 texcoord;
        
        layout(location = 0) out vec3 _normal;
        layout(location = 1) out vec2 _texcoord;
        
        void main()
        {
            _normal = normalize((transpose(inverse(model_matrix)) * vec4(normal, 0)).xyz);
            _texcoord = texcoord;
            gl_Position = mvp_matrix * vec4(position, 1);
        }
        
        
        #stage fragment
        
        #version 450
        
        layout(set = 0, binding = 0) uniform sampler2D albedo;
        layout(set = 0, binding = 1) uniform Light
        {
            vec3 dir;
            float intensity;
            vec3 color;
        } light;
        
        layout(location = 0) in vec3 _normal;
        layout(location = 1) in vec2 _texcoord;
        
        layout(location = 0) out vec3 color;
        
        void main()
        {
            color = vec3(1, 1, 1) * light.color * light.intensity * max(0, dot(-_normal, light.dir));
        }
    }
}
