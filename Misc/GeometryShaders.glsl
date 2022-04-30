
/*

	This shaders' operations change or expand the original 
	geometry sent to the shader by developing new vertices and vertex
	groups.

	As an example, each triangle in a model could be replaced by a triangle
	shrunk about its centroid. 
 */

/* Geometry Shader  */
layout(triangles) in;
layout(triangle_strip, max_vertices = 32) out;

uniform float uShrink;
uniform mat4 uModelViewProjectMatrix;

in vec3 vNormal[3];

out float gLightIntensity;

const vec3 LIGHTPOS = vec3(0, 10, 0);

vec3 V[3];
vec3 CG;

void ProduceVertex(int vi)
{
	gLightIntensity = dot(normalize(LIGHTPOS - V[vi]), vNormal[Vi]);
	gLightIntensity = abs(gLightIntensity);

	gl_Position = uModelViewProjectMatrix * vec4(CG + uShrink * (V[vi] - CG), 1);
	EmitVertex();
}

void main()
{
	V[0] = gl_Position[0].xyz;
	V[1] = gl_Position[1].xyz;
	V[2] = gl_Position[2].xyz;
	CG = (V[0] + V[1] + V[2]) / 3;
	ProduceVertex(0);
	ProduceVertex(1);
	ProduceVertex(2);
}
