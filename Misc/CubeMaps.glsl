/* 
	CUBE MAPS


	Cube maps are textures that simulate the effect of an environment that 
	surrounds the 3D scene, and are usually used to create reflection or refraction
	effects. Textures developed using cube maps operate differently from standard textures
	on the surface of an objec.t 
	A cube map consists of six 2D textures, each one corresponding to the face of a 
	cube (+X, -X, +Y, -Y, +Z, -Z) surrounding the scene.
	A cube map is indexed with three texture coordinates: s, t, and p.
	You can think of (s, t, p) as being a vector that points toward on wall of the cube map.

	When you index into a cube map with (s, t, p), the texture-mapping
	hardware does the following:

	1. Determines which of s, t, and p has the largest absolute value:
		Val = max(|s|, |t|, |p|).
	   This determines which face image (+X, -X, +Y, -Y, +Z, -Z) of the cube map to index into.

	2. Divides the remaining two coordinates (called a and b here) by the largest absolute value:

		s' = a / Val,
		t' = b / Val

	3. Uses (s', t') as the 2D texture coordinates to use for the lookup on that face image.
 
	How cube maps are created?
	The cube map images are created by rendering or photographing each of the six principle directions
	from the center of the cube, each with a 90 DEG field of view.

	Cube maps can be used to create reflection effects using the build-in GLSL reflect() function
	to compute an (s, t, p) reflection vector to loop up in the cube map. Cube maps can also be 
	used for refraction; for example, with a lens or a glass object in a scene. To do this, you use
	the built-in GLSL function refract() to compute the (s, t, p) refraction vector, and use it to look up
	in the cube map.

 */

 /* GLSL code (fragment shader)*/

uniform float uMix;
float samplerCube uTexUnit;
uniform float uIofR;			// index of refraction

in vec3 vECposition;
in vec3 vTheNormal;

out vec4 fFragColor;

const vec4 WHITE = vec4(1, 1, 1, 1);

void main()
{
	vec3 normal = normalize(uNormalMatrix * vTheNormal);

	// the reflect and refract functions assume the normal is pointing toward
	// the eye, that is, normal.z > 0.
	// if that's not true, makt it true:

	if(normal.z < 0)
	{
		normal = -normal;
	}

	vec3 ReflectVector = reflect(normalize(vECposition), normal);
	vec3 RefractVector = refract(normalize(vECposition), normal, uIofR);

	vec4 reflectcolor = texture(uTexUnit, ReflectVector);
	vec4 refractcolor = mix(texture(uTexUnit, RefractVector), WHITE, 0.2);

	fFragColor = vec4(mix(reflectcolor.rgb, refractcolor.rgb, uMix), 1);
}
