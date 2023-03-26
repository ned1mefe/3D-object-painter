#version 430

/*
	**Basic Vertex Shader**

	File Name	: vShader.vert
	Author		: Bora Yalciner
	Description	:

	// Basic Write Shader Nothing Fancy

*/

// Includes

// Definitions
#define IN_POS layout(location = 0)
#define IN_COLOR layout(location = 1)

#define OUT_COLOR layout(location = 0)
#define OUT_INTERP layout(location = 1)

#define U_TRANSFORM layout(location = 0)

struct ModelTransform
{
	mat4 mvp;
};

// Input
in IN_POS vec3 vPos;
in IN_COLOR vec3 vColor;

// Output
out gl_PerVertex {vec4 gl_Position;};	// Mandatory
out OUT_COLOR vec3 fColor;
out OUT_INTERP float vInterp;

// Uniforms
U_TRANSFORM uniform mat4 mvp;

void main(void)
{
	// Mask internally
	vec3 d = abs(vColor - vec3(0.5f, 0.5f, 0.5f));
	if(d.x < 0.1f && d.y < 0.1f && d.z < 0.1f)
		vInterp = 0.0f;
	else
		vInterp = 1.0f;

	// To fragment shader
	fColor = vColor;
	// To rasterizer,
	vec4 pos = mvp * vec4(vPos.xyz, 1.0f);
	gl_Position = pos;
}