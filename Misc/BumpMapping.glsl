
/*
	Bump mapping is a technique that simulates variations in a surface
	by manipulating the surface normals, allowing the lighting process
	to create the appearance of the variations.
	The key is to think about normals, not vertices, and to realize that in 
	a fragment shader, you are touching each pixel individually. 
	This technique can use an analytic approach and computer normals as 
	function derivatives, as we will see with the ripple example below, or 
	it can use a more geometric approach and compute normals based on location
	and slopes of the shape of the bump pattern. 
 */

/* 
	Ripples by bump mapping:


	f(x, y) = sin(x^x + y^y)

	let g(x, y, f(x, y)) = f(x, y) - sin(x^x + y^y)
	let z = f(x, y)
		g(x, y, z) = z - sin(x^x + y^y)

	normal = direction of the greatest decrease in the valueu (field)
		   = gradient (g(x, y, z))
		   = gradient (z - sin(x^x + y^y))
	n = { d/dx(z - sin(x^x + y^y)), d/dy(z - sin(x^x + y^y)), d/dz(z - sin(x^x + y^y)) }
	  = { -2xcos(x^x + y^y), -2ycos(x^x + y^y), 1 }
 */

/*
	Generalized Bump Mapping: 

	Height fields are a special, and simplified, case of bump-mapping.
	Now, let's look at it in the general case. 
	For this, we will define a surface local coordinate system at each fragment
	with componenets N, T, B (Normal, Tangent, and BiTangent B = T x N).
 */

// GLSL code (Vertex Shader)

attribute vec3 aTangent;		// from glman Sphere primitive, points towards north pole

uniform float uLightX, uLightY, uLightZ;	 // from sliders

out vec3 vBTNx, vBTNy, vBTNz;
out vec3 vLightDir;					// light direction in TNB coords
out vec2 vST;

// N is the direction of the surface normal
// T is the direction of the "Tangent", which is (dx/dt, dy/dt, dz/dt)
// B is the TxN, which is the direction of (dx/ds, dy/ds, dz/ds)

void main()
{
	vST = aTexCoord0.st;

	// B-T-N form an X-Y-Z-looking right handed coordinate system:
	vec3 N = normalize(uNormalMatrix * aNormal);
	vec2 T = normalize(vec3(uModelViewMatrix * vec4(aTangent, 0)));

	vec3 B = normalize(cross(T, N));

	// the light direction, in eye coordinates:
	vec3 lightPosition = vec3(uLightX, uLightY, uLightZ);
	vec3 ECpos = (uModelViewMatrix * aVertex).xyz;
	vLightDir = normalize(lightPosition - ECpos);

	// Produce the transformation from Surface coords to Eye coords:

	vBTNx = vec3(B.x, T.x, N.x);
	vBTNy = vec3(B.y, T.y, N.y);
	vBTNz = vec3(B.z, T.z, N.z);

	gl_Position = uModelViewProjectionMatrix * aVertex;
}

// GLSL code (Fragment Shader)
uniform float uAmbient;
uniform float uBumpDensity;	 	// glman slider unfirom variables
uniform float uBumpSizes;
uniform vec4 uSurfaceColor;
uniform float uAng;
uniform float uHeight;

in vec3 vBTNx, vBTNy, vBTNz;
in vec3 vLightDir;
in vec2 vST;

out vec4 fFragColor;

const float PI = 3.14159265;

float Cang, Sang;

vec3 ToXyz(vec3 btn)
{

}

void main()
{
	vec2 st = vST; 		// locate the bumps based on (s, t)

	float Swidth = 1 / uBumpDensity;
	float Theight = 1 / uBumpDensity;
	float numInS = floor(st.s / Swidth);
	float numInT = floor(st.t / Theight);

	vec3 center;
	center.s = numInS * Swidth + Swidth/2;
	center.t = numInT * Theight + Theight/2;
	st -= center;			// st is now wrt the center of the bump

	Cang = cos(uAng);
	Sang = sin(uAng);
	vec2 stp;					// st' = st rotated by -Ang
	stp.s = st.s * Cang + st.t * Sang;
	stp.t = -st.s * Sang + st.t * Cang;
	float theta = atan(stp.t, stp.s);

	// this is the normal of the parts of the object
	// that are not in a pyramid:
	vec3 normal = ToXyz(vec3(0, 0, 1));

	// figure out what part of the pyramid we are in and
	// get the normal there; then transform it to eye cords
	if(abs(stp.s) > Swidth/4 || abs(stp.t) > Theight/4)
	{
		normal = ToXyz(vec3(0, 0, 1));
	}
	else
	{
		if(PI/4 <= theta && theta <= 3*PI/4)
		{
			normal = ToXyz(vec3(0, uHeight, Theight/4));
		}
		else if(-PI/4 <= theta && theta <= PI/4)
		{
			normal = ToXyz(vec3(uHeight, 0, Swidth/4));
		}
		else if(-3*PI/4 <= theta && theta <= -PI/4)
		{
			normal = ToXyz(vec3(0, -uHeight, Theight/4));
		}
		else if(theta >= 3*PI/4 || theta <= -3*PI/4)
		{
			normal = ToXyz(vec3(-uHeight, 0, Swidth/4));
		}
	}

	float intensity = uAmbient + (1 - uAmbient) * dot(normal, vLightDir);
	vec3 litColor = uSurfaceColor.rgb * intensity;
	fFragColor = vec4(litColor, uSurfaceColor.a);
}
