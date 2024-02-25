#version 330 core

uniform mat4 mvp;
layout(location=0) in vec3 position;
layout(location=1) in vec2 textureCoord;

out vec2 textureC;

void main() {
	gl_Position = mvp * vec4(position, 1);
	textureC = textureCoord;
}