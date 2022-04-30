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
per-vertex [3, 3] vec3 tangent;

vertex [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
	mat4 model_matrix;
} push;

fragment [0, 0] uniform sampler2D albedo;
fragment [0, 1] uniform sampler2D normal_map;
fragment [0, 2] uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

fragment [0, 3] uniform Misc
{
	vec3 eye_dir;
} misc;

fragment [0, 4] uniform Properties
{
	float specularity;
	vec3 specular_color;
} properties;

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
        layout(location = 3) in vec3 tangent;
        
        layout(location = 0) out vec2 _texcoord;
        layout(location = 1) out vec3 _tangent;
        layout(location = 2) out vec3 _normal;
        layout(location = 3) out vec3 _bitangent;
        
        void main()
        {
        	mat4 normal_matrix = transpose(inverse(model_matrix));
        	_tangent = normalize((normal_matrix * vec4(tangent, 0)).xyz);
         	_normal = normalize((normal_matrix * vec4(normal, 0)).xyz);
            _bitangent = cross(_normal, _tangent);
        	_texcoord = texcoord;
        	gl_Position = mvp_matrix * vec4(position, 1);
        }
        
        
        #stage fragment
        
        #version 450
        
        layout(set = 0, binding = 0) uniform sampler2D albedo;
        layout(set = 0, binding = 1) uniform sampler2D normal_map;
        layout(set = 0, binding = 2) uniform Light
        {
        	vec3 dir;
        	float intensity;
        	vec3 color;
        } light;
        
        layout(set = 0, binding = 3) uniform Misc
        {
        	vec3 eye_dir;
        } misc;
        
        layout(set = 0, binding = 4) uniform Properties
        {
        	float specularity;
        	vec3 specular_color;
        } properties;
        
        layout(location = 0) in vec2 _texcoord;
        layout(location = 1) in vec3 _tangent;
        layout(location = 2) in vec3 _normal;
        layout(location = 3) in vec3 _bitangent;
        
        layout(location = 0) out vec4 color;
        
        void main()
        {
        	vec3 n = normalize(texture(normal_map, _texcoord).rgb * 2 - 1);
        	vec3 normal = _bitangent * n.g + _tangent * n.r + _normal * n.b;
        	vec3 light_dir = -light.dir;
        	float incoming_light = max(0, dot(normal, light_dir));
        	incoming_light = incoming_light * 1.05 + (1 - incoming_light) * 0.1;
        
        	vec3 reflection_vector = -light_dir + 2 * (dot(normal, light_dir) * normal);
        	float specular_dot = pow(max(0, dot(reflection_vector, normal)), properties.specularity);
        	specular_dot = specular_dot + (1 - specular_dot) * 0.1;
        
        	vec3 diffuse = texture(albedo, _texcoord).rgb;
        	
        	color = vec4(diffuse * incoming_light * light.color * light.intensity * properties.specular_color * specular_dot, 1);
        }
    }
}
