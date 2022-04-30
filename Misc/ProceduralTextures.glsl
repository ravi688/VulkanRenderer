
/*
	A more sophisticated example of a procedural texture
	computes the Mandelbrot function given by the texture coordinates
	of each vertex, using an iterative process. 

NOTE: The Mandelbrot set is the set of values of c in the complex plane
	for which the orbit of the critical point z = 0 under iteration of the
	quadratic map

		Zn+1 = Zn^2 + c

	remains bounded. Thus, a complex number c is a member of the Mandelbrot set if,
	when starting with Z0 = 0 and applying the iteration repeatedly, the absolute
	value of Zn remains bounded for all n > 0.

	For example, for c = 1, the sequence is 0, 1, 2, 5, 26, ..., which tends to
	infintiy, so 1 is not an element of the Mandelbrot set. On the other hande,
	for c = -1, the sequence is 0, -1, 0, -1, 0, ..., which is bounded, so -1 does belong
	to the set.

NOTE: Cardioid
	A cardioid is a plane curve traced by a point on the perimeter of a 
	circle that is rolling around a fixed circle of the same radius.
	It can also be defineid as an epicycloid having a single cusp.
 */

// Initial input is complex number (real, imag)
// Set x = real, and y = imag
// Iterate until we reach a max # of iterations or x*x + y*y >= some limit
{
	float newx = x*x - y*y + r;
	float newy = 2*x*y + c;
	x = newx; y = newy;
}

if x*x + y*y < some_limit		// the process has converged
	// color the fragment blue
else
	// color the fragment based on the number of iterations

/* GLSL code  */

uniform int uMaxIters;
uniform float uTS;				// texture coordinate scaling
uniform float uCS;				// color scaling
uniform float uS0;				// starting the texture value in S
uniform float uT0;				// starting the texture value in T
uniform float uLimit;			// how large before the stop iterations
uniform vec3 uConvergeColor;
uniform vec3 uDivergeColor1;
uniform vec3 uDivergeColor2;

in vec2 vST;
in float vLightIntensity;
out vec4 fFragColor;

void main()
{
	float real = vST.s * uTS + uS0;
	float imag = vST.t * uTS + uT0;
	float real0 = real;
	float imag0 = imag;
	float newr;
	int numIters;
	vec4 color = vec4(0, 0, 0, 1);

	for(numIters = 0; numIters < uMaxIters; numIters++)
	{
		float newreal = real0 + real * real - imag * imag;
		float newimag = imag0 + 2 * real * imag;
		newr = newreal * newreal + newimag * newimag;
		if(newr >= uLimit)
			break;
		real = newreal;
		imag = newimag;
	}

	// choose the color
	if(newr < uLimit)
		color = uConvergeColor;
	else
		color = mix(uDivergeColor1, uDivergeColor2, fract(numIters/uCS));
		color.rgb *= vLightIntensity;
		fFragColor = color;
}

/*
	An advantage of procedural texturing in fragment shaders is that the texture
	can be computed at the pixel level, no matter what size that pixel actually represents
	in the model. The potentially large number of iterations for the Mandelbrot
	sequence gives us the opportunity to illustrate the power of double precision in shaders.
	
 */