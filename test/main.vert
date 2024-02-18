#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 surfacePos;
out vec3 inNormalFrag;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 worldToView;

void main(void)
{
	gl_Position = projMatrix * worldToView * mdlMatrix *vec4(inPosition, 1.0);

	inNormalFrag = inNormal;

	//For camera cross
	surfacePos = vec3(mdlMatrix  * vec4(inPosition,1.0f));

	texCoord = inTexCoord;
}
