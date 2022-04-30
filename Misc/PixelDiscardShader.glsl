
/* Vertex Shader */
uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjectionMatrix;
uniform mat3 uNormalMatrix;

in vec4 aVertex;
in vec4 aTexCoord0;
in vec4 aColor;
in vec3 aNormal;

out vec4 vColor;
out float vLightIntensity;
out vec2 vST;

const vec3 LIGHTPOS = vec3(0, 0, 10);


void main()
{
	vec3 transNorm = normalize(vec3(uNormalMatrix * normal));

	vec3 ECposition = vec3(uModelViewMatrix * aVertex);
	vLightIntensity = dot(normalize(LIGHTPOS - ECposition), transNorm);
	vLightIntensity = clamp(0.3 + abs(vLightIntensity), 0, 1);

	vST = aTexCoord0.st;
	vColor = aColor;
	gl_Position = uModelViewProjectionMatrix * aVertex;
}

/* Fragment Shader */

uniform float uDensity;
uniform float uFrequency;

in vec4 vColor;
in float vLightIntensity;
in vec2 vST;

out vec4 fFragColor;

void main()
{
	float sf = vST.s * uFrequency;
	float tf = vST.t * uFrequency;

	if(fract(sf) >= uDensity && fract(tf) >= uDensity)
		discard;

	fFragColor = vec4(vLightIntensity * vColor.rgb, 1);
}

/* Performance Optimization */

vec2 stf = vST * uFrequency;

if(all(fract(stf) >= vec2(uDensity, uDensity)))
	discard;
