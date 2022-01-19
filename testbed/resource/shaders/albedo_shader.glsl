#section SETTINGS DEFAULT

wireframe true
winding clockwise
line_thickness 5
//TODO: make color blending configurable here
blend true
//TODO: make depth buffer configurable here
depth true


//TODO: Make vertex attributes programmable in this file as followings:
per-vertex [0] vec3 position;
per-vertex [1] vec3 normal;
per-vertex [2] vec3 color;
per-vertex [3] vec2 cootex_rd;

per-instance[0] vec3 offset;


#section LAYOUT


vertex [push_constant] [0] uniform Push
{
    mat4 mvp_matrix;
    mat4 model_matrix;
} push;

// fragment [push_constant] [64] uniform Push
// {
//     mat4 model_matrix;
// } push;


fragment[0, 0] uniform sampler2D texture;
fragment[0, 1] uniform sampler2D texture2;
fragment[0, 4] uniform sampler2D normal_map;
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
    mat4 model_matrix;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;
layout(location = 2) out mat3 tbn_matrix;

mat3 build_tbn_matrix(mat4 normal_matrix, vec3 tangent, vec3 normal)
{
    tangent = (normal_matrix * vec4(tangent, 0)).xyz;
    normal = (normal_matrix * vec4(normal, 0)).xyz;
    vec3 bi_tangent = normalize(cross(tangent, normal));
    return mat3(tangent, bi_tangent, normal);
}

void main()
{
    vec4 v = mvp_matrix * vec4(inPosition, 1.0);
    gl_Position = v;
    fragColor = inColor;
    texCoord = inTexCoord;
    tbn_matrix = build_tbn_matrix(transpose(inverse(model_matrix)), vec3(0, 1, 0), inNormal);
}


#stage fragment

#version 450

// layout(push_constant) uniform Push
// {
//     mat4 _;
//     mat4 model_matrix;
// };

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in mat3 tbn_matrix;

layout(set = 0, binding = 0) uniform sampler2D texSampler;
layout(set = 0, binding = 1) uniform sampler2D texSampler2;
layout(set = 0, binding = 4) uniform sampler2D normal_map;
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

vec3 unpack_normal(sampler2D normal_map, vec2 uv)
{
    return normalize(tbn_matrix * normalize(texture(normal_map, uv).rgb * 2 - 1));
}

void main() {
    vec3 _normal = unpack_normal(normal_map, texCoord);
    color = vec4(fragColor * (dot(-normalize(light.dir), _normal) + 1) * 0.5, 1);
    float t = (1 - sin(time)) * 0.5;
    vec3 texture_color = (texture(texSampler, texCoord).xyz * t + (1 - t) * texture(texSampler2, texCoord).xyz);
    color = vec4(color.rgb * green_color * light.intensity, 1);
}
