#version 330 core

uniform vec3 lightPos;
uniform float ambientLight;
uniform sampler2DShadow shadowTexture;

struct Material {
	vec3 Kd;
	vec3 Ks;
	vec3 Ka;
	float Ns;
	float d;
	sampler2D dTexture;
	int dTextureSet;
	sampler2D sTexture;
	int sTextureSet;
	sampler2D aTexture;
	int aTextureSet;
};  
uniform Material material;

in vec3 viewPosition;
in vec2 textureC;
in vec3 fragNormal;
in vec4 spotLightViewPosition;

out vec4 color;

vec3 CalcSpotLightComponent(vec3 albedo, vec3 specular, vec3 camDir) {
	vec3 lightDir = normalize(lightPos - viewPosition);
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));

	vec3 p = spotLightViewPosition.xyz;
	p.z *= 0.99999;
	p /= spotLightViewPosition.w;
	if (p.x > 1 || p.x < 0 || p.y > 1 || p.y < 0 || p.z > 1.0 || p.z < 0.0) { diffuseComponent = 0; }
	else { diffuseComponent *= texture(shadowTexture, p); }
	
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), material.Ns);
	
	//modified blinn-phong
	return diffuseComponent * (albedo + specular * specularComponent);
}

void main() {
	vec3 albedo = (material.dTextureSet == 1) ? texture(material.dTexture, textureC).xyz * material.Kd : material.Kd;
	vec3 specular = (material.sTextureSet == 1) ? texture(material.sTexture, textureC).xyz * material.Ks : material.Ks;
	//vec3 ambient = (material.aTextureSet == 1) ? texture(material.aTexture, textureC).xyz * material.Ka : material.Ka;
	
	vec3 camDir = -normalize(viewPosition);
	vec3 shading = CalcSpotLightComponent(albedo, specular, camDir);

	color = vec4(shading + albedo * ambientLight, material.d);
}