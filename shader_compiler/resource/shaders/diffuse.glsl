#section SETTINGS

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

