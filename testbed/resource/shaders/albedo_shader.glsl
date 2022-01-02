#section SETTINGS DEFAULT

wireframe true
line_thickness 5
blend true
winding clockwise


//TODO: Make vertex attributes programmable in this file as followings:
per-vertex [0] vec3 position;
per-vertex [1] vec3 normal;
per-vertex [2] vec3 color;
per-vertex [3] vec2 cootex_rd;

per-instance[0] vec3 offset;


#section LAYOUT

// vertex[0, 0] uniform Matrices
// {
//     mat4 model_matrix;
//     mat4 view_matrix;
//     mat4 projection_matrix;
//     mat4 clip_matrix;
// } matrices;

fragment[0, 0] uniform sampler2D texture;
// fragment[0, 0] uniform sampler2D windows_texture;
// fragment[0, 1] uniform sampler2D apple_texture;
// fragment vertex[0, 1] uniform SceneData
// {
//     //TODO: Add default value support to shader_compiler
//     float time;             //float time = 0.0f
//     vec3 light_dir;         //vec3 light_dir = (1.0f, 0.0f, 0.0f)
//     float light_intensity;  //float light_intensity = 1.0f
// } scene_data;



#section SHADER


#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
};

// layout(set = 0, binding = 0) uniform Matrices
// {
//     mat4 projection_matrix;
//     mat4 view_matrix;
//     mat4 model_matrix;
// } matrices;

// layout(set = 0, binding = 1) uniform SceneData
// {
//     float time;
//     vec3 light_dir;
//     float light_intensity;
// } scene_data;

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
// layout(set = 0, binding = 0) uniform sampler2D texSampler2;
// layout(set = 0, binding = 1) uniform sampler2D texSampler3;

// layout(set = 0, binding = 1) uniform SceneData
// {
//     float time;
//     vec3 light_dir;
//     float light_intensity;
// } scene_data;

float dot_product(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

void main() {
    vec3 lightDir = normalize(vec3(1, -1, 0.1));
    color = fragColor * (max(0, dot(lightDir, -normal)) + 0.1f);
    color = color * texture(texSampler, texCoord).xyz;
}


