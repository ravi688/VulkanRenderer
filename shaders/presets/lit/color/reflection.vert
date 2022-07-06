
#version 450

#include <v3d.h>

layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;


layout(location = POSITION_LOCATION) in vec3 position;
layout(location = NORMAL_LOCATION) in vec3 normal;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec3 _position;

void main()
{
	_position = normalize((objectInfo.normal * vec4(position, 0)).xyz);
	vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
	_normal = normalize((objectInfo.normal * vec4(normal, 0)).xyz);
}
