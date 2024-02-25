#version 330 core

uniform vec3 Kd;

uniform samplerCube skybox;
uniform int skyboxSet;

in vec3 dir;

out vec4 color;

void main() {
	vec3 albedo = (skyboxSet == 1) ? texture(skybox, dir).xyz : Kd;
	color = vec4(albedo, 1);
}