
#version 450

layout(set = 0, binding = 0) uniform CameraInfo
{
	mat4 transform;
	mat4 projection;
	mat4 view;
} cameraInfo;


layout(set = 4, binding = 0) uniform ObjectInfo
{
	mat4 transform;
	mat4 normal;
} objectInfo;

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
}
