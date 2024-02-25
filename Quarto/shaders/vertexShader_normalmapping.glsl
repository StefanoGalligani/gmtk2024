#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 textureCoord;
layout(location=2) in vec3 normal; //servirà se e quando le normali andranno aggiustate in base all'orientamento

out vec2 vTextureC;

void main() {
	gl_Position = vec4(position, 1);
	vTextureC = textureCoord;
}