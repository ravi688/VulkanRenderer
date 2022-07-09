
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform SpotLight light;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 0) out vec3 _position;

void main()
{
	_position = (objectInfo.transform * vec4(position, 1.0)).xyz;
	vec4 clipPos = light.projection * light.view * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
}

