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
            blendEnable = true,
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

vertex [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
} push;

#section SHADER

RenderPass
{
    SubPass
    {
        #stage vertex
        
        #version 450
        
        layout(push_constant) uniform Push
        {
        	mat4 mvp;
        } push;
        
        layout(location = 0) in vec3 position;
        
        
        void main()
        {
        	gl_Position = push.mvp * vec4(position, 1.0);
        }
        
        
        #stage fragment
        
        #version 450
        
        layout(location = 0) out vec4 color;
        
        void main()
        {
        	color = vec4(0.1f, 0.6f, 0.4f, 0.2f);
        }
    }
}
