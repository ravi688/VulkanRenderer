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


fragment[0, 0] uniform sampler2D albedo;
fragment[0, 1] uniform Light
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
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec2 _texcoord;

void main()
{
    gl_Position = mvp_matrix * vec4(inPosition, 1);
    _normal = normalize(transpose(inverse(model_matrix)) * vec4(inNormal, 0)).xyz;
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform Light
{
    vec3 dir;
    float intensity;
} light;


layout(location = 0) in vec3 _normal;
layout(location = 1) in vec2 _texcoord;

layout(location = 0) out vec3 color;

void main() {

    float dot_product = 0.5f * dot(-light.dir, _normal) + 0.5f;
    color = texture(albedo, _texcoord).rgb * dot_product;

}
