#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;
in vec3 inNormalFrag;

in vec3 surfacePos;

const vec3 lightPos = vec3(10,200,0);
const float normalizeFactor = 0.5f;

const vec3 lightColor = vec3(0.7,0.7,0.55);
uniform mat4 mdlMatrix;

void main(void)
{
	vec3 lightDir = normalize(lightPos - surfacePos);
	float lampStrength = dot(normalize(inNormalFrag), lightDir);

	lampStrength = max(0.01, lampStrength) *0.5f;

	vec3 lightStrength = lampStrength * lightColor;

	outColor = texture(tex, texCoord/3) + vec4(lightStrength, 0);
}
