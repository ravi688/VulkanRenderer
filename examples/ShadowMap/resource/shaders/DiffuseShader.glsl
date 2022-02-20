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

vertex [push_constant] [0] uniform Push
{
	mat4 mvpMatrix;
	mat4 modelMatrix;
} push;

fragment [0, 0] uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

fragment [0, 1] uniform Properties
{
	vec4 color;
} properties;

#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvpMatrix;
	mat4 modelMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec2 _texcoord;

void main()
{
	_normal = normalize((transpose(inverse(modelMatrix)) * vec4(normal, 0)).xyz);
	_texcoord = texcoord;
	gl_Position = mvpMatrix * vec4(position, 1);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

layout(set = 0, binding = 1) uniform Properties
{
	vec4 color;
} properties;

layout(location = 0) in vec3 _normal;
layout(location = 1) in vec2 _texcoord;

layout(location = 0) out vec3 color;

void main()
{
	color = properties.color.rgb * light.color * light.intensity * (0.5f * dot(_normal, -light.dir) + 0.5f);
}
