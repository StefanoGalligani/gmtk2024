#version 410 core

layout(vertices = 4) out;
uniform float level;

in vec2 vTextureC[];

out vec2 textureCTess[];

void main(void)
{
	gl_TessLevelOuter[0] = level;
	gl_TessLevelOuter[1] = level;
	gl_TessLevelOuter[2] = level;
	gl_TessLevelOuter[3] = level;

	gl_TessLevelInner[0] = level;
	gl_TessLevelInner[1] = level;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	textureCTess[gl_InvocationID] = vTextureC[gl_InvocationID];
}
