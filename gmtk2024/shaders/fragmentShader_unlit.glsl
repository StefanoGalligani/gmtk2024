#version 330 core

struct Material {
	vec3 Kd;
	float d;
	sampler2D dTexture;
	int dTextureSet;
};  
uniform Material material;

in vec2 textureC;

out vec4 color;

void main() {
	vec3 albedo = (material.dTextureSet == 1) ? texture(material.dTexture, textureC).xyz * material.Kd : material.Kd;
	color = vec4(albedo, material.d);
}