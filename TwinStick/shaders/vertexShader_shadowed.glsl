#version 330 core

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 mvt;
uniform mat4 shadowMatrix;
layout(location=0) in vec3 position;
layout(location=1) in vec2 textureCoord;
layout(location=2) in vec3 normal;

out vec3 viewPosition;
out vec2 textureC;
out vec3 fragNormal;
out vec4 spotLightViewPosition;

void main() {
	gl_Position = mvp * vec4(position, 1);
	viewPosition = (mv * vec4(position, 1)).xyz;
	fragNormal = mvt * normal;
	textureC = textureCoord;
	spotLightViewPosition = shadowMatrix * vec4(position,1);
}