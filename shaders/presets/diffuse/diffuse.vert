
#version 450

#include <v3d.h>

layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight light;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;
layout(location = NORMAL_LOCATION) in vec3 normal;
layout(location = TEXCOORD_LOCATION) in vec2 texcoord;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec2 _texcoord;

layout(location = 2) out vec4 _lightClipPos;

void main()
{
	vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
	_normal = normalize((objectInfo.normal * vec4(normal, 1)).xyz);
	_texcoord = texcoord;

	_lightClipPos = light.projection * light.view * objectInfo.transform * vec4(position, 1);
}
