

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;

float dot_product(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

void main() {
    vec3 lightDir = normalize(vec3(-0.1, -1, 0));
    color = fragColor * (max(0, dot(lightDir, -normal)) + 0.1f);
}
