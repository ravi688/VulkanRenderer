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

fragment[0, 0] uniform sampler2D texture;
fragment[0, 1] uniform sampler2D texture2;
fragment[0, 2] uniform SceneData
{
    vec3 green_color;
    float time;
    uint value;
} scene_data;

fragment[0, 3] uniform Light
{
    vec3 dir;
    float intensity;
} light;



#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
};

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

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 0) uniform sampler2D texSampler;
layout(set = 0, binding = 1) uniform sampler2D texSampler2;
layout(set = 0, binding = 2) uniform SceneData
{
    vec3 green_color;
    float time;
    uint value;
};
layout(set = 0, binding = 3) uniform Light
{
    vec3 dir;
    float intensity;
} light;


void main() {
    color = vec4(fragColor * (max(0, dot(light.dir, -normal)) + 0.1f), 1);
    float t = (1 - sin(time)) * 0.5;
    vec3 texture_color = (texture(texSampler, texCoord).xyz * t + (1 - t) * texture(texSampler2, texCoord).xyz);
    color = vec4(color.rgb * green_color * light.intensity * texture_color, 1);
}
