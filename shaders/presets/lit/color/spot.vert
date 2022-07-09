
#version 450

#include <v3d.h>

SPOT_LIGHT light;
layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;
layout(location = NORMAL_LOCATION) in vec3 normal;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec3 _position;

void main()
{
	vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
	_normal = normalize((objectInfo.normal * vec4(normal, 1)).xyz);
	_position = (objectInfo.transform * vec4(position, 1)).xyz;
}
