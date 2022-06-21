
#version 450

#include <v3d.h>

CAMERA cameraInfo;
OBJECT objectInfo;

POSITION position;
TEXCOORD texcoord;

layout(location = 0) out vec2 _texcoord;

void main()
{
	vec4 clipPos = cameraInfo.screen * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
	_texcoord = texcoord;
}

