
/* Vertex Shader  */

in vec4 aVertex;
in vec4 aColor;

uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjectionMatrix;

out float vLightIntensity;
out vec3 vMyColor;

const vec3 LIGHTPOS = vec3(0, 10, 0);

void main()
{
	vec4 thisPos = aVertex;
	vMyColor = aColor.rgb;

	// create a new height for this vertex:
	float thisX = thisPos.x;
	float thisY = thisPos.y;
	// this surface is z = 0.3 * sin(x^2 + y^2)
	thisPos.z = 0.3 * sin(thisX * thisX + thisY * thisY);


	// now compute the normal and the light intensity
	vec3 xtangent = vec3(1, 0, 0); 		// x-axis
	xtangent.z = 2 * 0.3 * thisX * cos(thisX * thisX + thisY * thisY);

	vec3 ytangent = vec3(0, 1, 0); 		// y-axis
	ytangent.z = 2 * 0.3 * thisY * cos(thisX * thisX + thisY * thisY);

	vec3 thisNormal = normalize(cross(xtangent, ytangent));

	vec3 ECpos = vec3(uModelViewMatrix * thisPos);
	vLightIntensity = dot(normalize(LIGHTPOS - ECpos), thisNormal);

	vLightIntensity = 0.3 + abs(vLightIntensity); 	// 0.3 ambient
	vLightIntensity = clamp(vLightIntensity, 0, 1);

	gl_Position = uModelViewProjectionMatrix * thisPos;
}

