#version 330 core

layout(location=0) in vec3 position;

uniform mat4 model;
uniform mat4 mvp;

out vec3 fragPos;

void main() {
	fragPos = (model * vec4(position, 1)).xyz;
	gl_Position = mvp * vec4(position, 1);
}