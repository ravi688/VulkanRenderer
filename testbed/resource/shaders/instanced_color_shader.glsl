
#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 offset;
layout(location = 4) in vec4 rotation;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;

void main()
{
    mat2 rotation_matrix;
    rotation_matrix[0] = vec2(rotation.x, rotation.y);
    rotation_matrix[1] = vec2(rotation.z, rotation.w);
    vec2 v = vec2(position.z, position.y);
    v = rotation_matrix * v;
    v.x += offset.z;
    v.y += offset.y;
    gl_Position =  push.mvp_matrix * vec4(0, v.y, v.x, 1);
    fragColor = color;
    texCoord = uv;
}


#stage fragment

#version 450
layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;

void main() {
    color = fragColor * texture(texSampler, texCoord).xyz;
}

