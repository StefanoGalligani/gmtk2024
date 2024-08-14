#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 shadowMatrix;

uniform sampler2D displacementTexture;
uniform int displacementTextureSet;
uniform float displacementValue;

in vec2 textureCTess[];

out vec3 viewPosition;
out vec2 textureC;
out vec4 lightViewPosition;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix(v0,v1,gl_TessCoord.x);
	vec4 b = mix(v3,v2,gl_TessCoord.x);
	return mix(a,b,gl_TessCoord.y);
}

vec3 interpolate(vec3 v0, vec3 v1, vec3 v2, vec3 v3)
{
	vec3 a = mix(v0,v1,gl_TessCoord.x);
	vec3 b = mix(v3,v2,gl_TessCoord.x);
	return mix(a,b,gl_TessCoord.y);
}

vec2 interpolate(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
{
	vec2 a = mix(v0,v1,gl_TessCoord.x);
	vec2 b = mix(v3,v2,gl_TessCoord.x);
	return mix(a,b,gl_TessCoord.y);
}

void main()
{
	vec4 pos = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	textureC = interpolate(textureCTess[0], textureCTess[1], textureCTess[2], textureCTess[3]);

	if (displacementTextureSet == 1) pos.y += length(texture(displacementTexture, textureC)) * displacementValue;

	gl_Position = mvp * pos;
	viewPosition = (mv * pos).xyz;
	lightViewPosition = shadowMatrix * pos;

}