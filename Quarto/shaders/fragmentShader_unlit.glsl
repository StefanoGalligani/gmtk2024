#version 330 core

uniform vec3 Kd;
uniform float d;

uniform sampler2D dTexture;
uniform int dTextureSet;

in vec2 textureC;

out vec4 color;

void main() {
	vec3 albedo = (dTextureSet == 1) ? texture(dTexture, textureC).xyz : Kd;
	color = vec4(albedo + 0.2, d);
}