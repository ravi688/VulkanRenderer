
#section LAYOUT

vertex [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
} push;

fragment [0, 0] uniform samplerCube skybox;


#section SHADER


#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvp_matrix;
};

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 _uvw;

void main()
{
	_uvw = position;
	
	// In our renderer, x and z axis are swapped with each other
	_uvw = vec3(_uvw.z, -_uvw.y, _uvw.x);

	vec4 clip_pos = mvp_matrix * vec4(position, 1);
	gl_Position = vec4(clip_pos.x, clip_pos.y, clip_pos.w, clip_pos.w);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform samplerCube skybox;

layout(location = 0) in vec3 _uvw;

layout(location = 0) out vec3 color;

void main()
{
	color = texture(skybox, _uvw).rgb;
}
