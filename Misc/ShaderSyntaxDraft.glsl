#section SETTINGS

[PipelineLayout("ShadowMap")]
[Name("ShadowMap")]
GraphicsPipeline
{
    DepthStencil
    {
        depthTestEnable = true,
        depthWriteEnable = true,
        depthCompareOp = less
    }
}

[PipelineLayout("Color")]
[Name("Color")]
GraphicsPipeline
{
    ColorBlend
    {
        attachment
        {
            colorWriteMask = RGBA,
            blendEnable = false
        }
    }

    DepthStencil
    {
        depthTestEnable = true,
        depthWriteEnable = true,
        depthCompareOp = less
    }
}


#section LAYOUT

[Name("ShadowMap")]
PipelineLayout
{
    per-vertex [0, 0] vec3 position;

    vertex [push_constant] [0] uniform Push
    {
        mat4 mvp_matrix;        // model view projection matrix of the light
    } push;
}

[Name("Color")]
PipelineLayout
{
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
}

#section SHADER

RenderPass
{
    [GraphicsPipeline("ShadowMapPipeline")]
    [Output(id = 0)]         // store the color attachment after rendering
    [Output(id = 1)]         // store the color attachment after rendering
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
        layout(location = 1) out vec3 color2;
        
        void main()
        {
            color = vec3(1, 1, 1) * light.color * light.intensity * max(0, dot(-_normal, light.dir));
            color2 = color * 2;
        }
    }

    [GraphicsPipeline("ColorPipeline")]
    [Input(id = 0, index = 0, binding = 0)]
    [Input(id = 1, index = 1, binding = 1)]
    [Output(id = 2)]
    SubPass
    {
        #stage vertex

        #version 450
        
        layout(push_constant) uniform Push
        {
            mat4 mvp_matrix;
        };

        layout(input_attachment_index = 0, binding = 0) uniform subpassInput previousResult1;
        layout(input_attachment_index = 1, binding = 1) uniform subpassInput previousResult2;        

        layout(location = 0) in vec3 position;
        
        void main()
        {
            gl_Position = mvp_matrix * vec4(position, 1);
        }

        #stage fragment

        #version 450

        layout(location = 0) out vec3 color;

        void main()
        {
            color = subpassLoad(previousResult1).rgb * subpassLoad(previousResult2).rgb;
        }
    }

    [GraphicsPipeline("ColorPipeline")]
    [Input(id = 2, index = 0, binding = 0)]
    [Input(id = 0, index = 1, binding = 1)]
    SubPass
    {
        #stage vertex

        void main()
        {

        }

        #stage fragment

        #version 450

        layout(input_attachment_index = 0, binding = 0) uniform subpassInput previousResult1;
        layout(input_attachment_index = 1, binding = 1) uniform subpassInput previousResult2;

        layout(location = 0) out vec3 color;

        void main()
        {
            color = vec3(1, 1, 1);
        }
    }
}


RenderPass
{
    [GraphicsPipeline("ShadowMapPipeline")]
    [Input(depth, set = 0, binding = 0)]
    [Input(id = 0, set = 0, binding = 1)]
    [Input(id = 1, set = 0, binding = 2)]
    [Input(id = 2, set = 0, binding = 3)]
    [Output(id = 0)]
    SubPass
    {
        #stage vertex

        void main()
        {

        }

        #stage fragment

        layout(set = 0, binding = 0) uniform sampler2D depthBuffer;
        layout(set = 0, binding = 1) uniform sampler2D colorInput1;
        layout(set = 0, binding = 2) uniform sampler2D colorInput2;
        layout(set = 0, binding = 3) uniform sampler2D colorInput3;

        layout(location = 0) out vec3 color;

        void main()
        {
            color = vec3(1, 1, 1);
        }
    }

    [GraphicsPipeline("Color")]
    [Input(id = 0, index = 0, binding = 0)]
    SubPass
    {
        #stage vertex

        void main()
        {

        }

        #stage fragment

        layout(input_attachment_index = 0, binding = 0) uniform subpassInput previousResult;

        void main()
        {
            color = vec3(1, 1, 1);
        }
    }
}
