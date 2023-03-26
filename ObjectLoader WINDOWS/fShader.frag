#version 430

/*
	**Basic Fragment Shader**

	File Name	: fShader.vert
	Author		: Bora Yalciner
	Description	:

	// Shows the color directly

*/

// Includes

// Definitions
#define IN_COLOR layout(location = 0)
#define IN_INTERP layout(location = 1)

// Input
in IN_COLOR vec3 fColor;
in IN_INTERP float fInterp;

out vec4 framebuffer;

void main(void)
{
	vec4 gColor;
	if(fInterp < 0.99f)
	{
		gColor.rgb = vec3(0.5f, 0.5f, 0.5f);
	}
	else
	{
		gColor.rgb = fColor;
	}
	gColor.a = 1.0f;

	framebuffer = gColor;
}