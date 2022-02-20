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

per-vertex 		[0, 0] vec3 position;
per-instance 	[1, 1] vec3 offset;
per-instance 	[1, 2] vec3 scale;
per-instance 	[1, 3] vec3 rotation;

vertex [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
} push;

fragment [0, 0] uniform UBO
{
	float time;
} ubo;


#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvp;
} push;


//Inputs
//per vertex
layout(location = 0) in vec3 norm_position;

//per instance
layout(location = 1) in vec3 offset;
layout(location = 2) in vec3 scale;
layout(location = 3) in vec3 rotation;

void main()
{
	vec3 pos = offset;
	pos += norm_position;
	pos = pos * scale;
	gl_Position = push.mvp * vec4(pos, 1);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform UBO
{
	float time;
};

layout(location = 0) out vec3 color;

vec3 lerp(vec3 v1, vec3 v2, float t) { return v1 * (1 - t) + v2 * t; }

void main()
{
	color = lerp(vec3(0.2f, 0.9f, 1.0f), vec3(0.5f, 0.1f, 1.0f), (1 - sin(time * 5)) * 0.5f);
}


