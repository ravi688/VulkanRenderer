#section SETTINGS

input-assembly
{
    topology = triangle,
    primitive-restart-enable = true
}

rasterization
{
    depth-clamp-enable = false,
    rasterizer-discard-enable = true,
    polygon-mode = fill,
    line-width = 1.0f,
    cull-mode = back,
    front-face = clockwise,
    depth-bias-enable = true,
    depth-bias-constant-factor = 0.0f,
    depth-bias-clamp = 0.0f,
    depth-bias-slope-factor = 0.0f
}

viewport
{
    scissor
    {
        offset
        {
            x = 0,
            y = 0
        }
        extent = full_screen // or { 800, 800 }
    }
    viewport
    {
        x = 0,
        y = 0,
        width = screen_width,
        height = screen_width,
        min-depth = 0.0f,
        max-depth = 1.0f
    }
}

multisample
{
    sample-shading-enable = false,
    rasterization-samples = 1,
    min-sample-shading = 1.0f,
    alpha-to-converage-enable = false,
    alpha-to-one-enable = false
}

color-blend
{
    logic-op-enable = false,
    logic-op = copy,

    color-attachment
    {
        color-write-mask = r|g|b|a, // or rgba in case of color
        blend-enable = false,
        src-color-blend-factor = src-alpha,
        dst-color-blend-factor = one-minus-src-alpha,
        color-blend-op = add,
        src-alpha-blend-factor = one,
        dst-alpha-blend-factor = zero,
        alpha-blend-op = add
    }

    blend-constants
    {}
}

stencil
{
    front
    {
        fail-op = keep // or zero, replace etc.
        pass-op = keep
        depth-fail-op = keep
        compare-op = less
        compare-mask = 0xFF00FF00
        write-mask = 0x000F0FFF
        reference = 14
    }

    back
    {
        // same as above
    }
}

depth
{
    test-enable = true,
    write-enable = true,
    compare-op = less,
    bounds-test-enable = false,
    min-bounds = 0.0f,
    max-bounds = 1.0f,
}


#section LAYOUT

per-vertex [0, 0] vec3 position;
per-vertex [1, 1] vec3 normal;
per-vertex [2, 2] vec2 texcoord;


vertex fragment [1, 4] uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

fragment[2, 6] uniform sampler2D albedo_texture[2];
fragment vertex[12, 34] uniform SceneData
{
    float time;
    vec3 light_dir;
    float light_intensity;
} scene_data[3];


#section SHADER
#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
};

layout(set = 0, binding = 1) uniform UBO
{
    float time;
} ubo;

layout(set = 0, binding = 0) uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;

void main()
{
    vec4 v = mvp_matrix * vec4(inPosition, 1.0);
    gl_Position = v;
    fragColor = inColor;
    normal = inNormal;
    texCoord = inTexCoord;
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(set = 0, binding = 1) uniform UBO
{
    float time;
    vec3 light_dir;
    float light_intensity;
} ubo1;

float dot_product(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

void main() {
    vec3 lightDir = normalize(vec3(1, -1, 0.1));
    color = fragColor * (max(0, dot(lightDir, -normal)) + 0.1f);
    color = color * texture(texSampler, texCoord).xyz;
}

