#version 330 core

struct SpotLight {
	vec3 pos;
	vec3 color;
	float intensity;
};
uniform SpotLight spotLight;
uniform float ambientLight;

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

out vec4 color;

vec3 CalcLightComponent(vec3 albedo, vec3 specular, vec3 camDir) {
	vec3 lightDir = normalize(spotLight.pos - viewPosition);
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));
	
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), material.Ns);
	
	//blinn-phong
	return diffuseComponent * albedo + specular * specularComponent;
}

void main() {
	vec3 albedo = (material.dTextureSet == 1) ? texture(material.dTexture, textureC).xyz * material.Kd : material.Kd;
	vec3 specular = (material.sTextureSet == 1) ? texture(material.sTexture, textureC).xyz * material.Ks : material.Ks;
	//vec3 ambient = (material.aTextureSet == 1) ? texture(material.aTexture, textureC).xyz * material.Ka : material.Ka;
	
	vec3 camDir = -normalize(viewPosition);
	vec3 shading = CalcLightComponent(albedo, specular, camDir);

	color = vec4(shading + albedo * ambientLight, material.d);
}