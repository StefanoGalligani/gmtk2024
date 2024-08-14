#version 330 core

uniform mat3 toWorld;

layout(location=0) in vec3 position;

out vec3 dir;

void main() {
	dir = toWorld * position;
	gl_Position = vec4(position, 1);
}