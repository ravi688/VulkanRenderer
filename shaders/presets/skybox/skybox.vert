
#version 450

#include <v3d.h>

layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 0) out vec3 _uvw;

void main()
{
    _uvw = position;
        	
    // In our renderer, x and z axis are swapped with each other
    _uvw = vec3(_uvw.z, -_uvw.y, _uvw.x);

    mat4 view = cameraInfo.view;
    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;
    view[3][3] = 1;

    vec4 clipPos = cameraInfo.projection * view * objectInfo.transform * vec4(position, 1);
    gl_Position = vec4(clipPos.x, -clipPos.y, clipPos.w, clipPos.w);
}
