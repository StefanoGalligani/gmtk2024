#version 330 core

#define MAX_LIGHTS 10

struct SpotLight {
	vec3 pos;
	vec3 color;
	float intensity;
};
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int nSpotLights;

struct DirLight {
	vec3 dir;
	vec3 color;
	float intensity;
};
uniform DirLight dirLights[MAX_LIGHTS];
uniform int nDirLights;

struct AmbientLight {
	vec3 color;
	float intensity;
};
uniform AmbientLight ambientLights[MAX_LIGHTS];
uniform int nAmbientLights;

struct Material {
	vec3 Kd;
	vec3 Ks;
	float Ns;
	float d;
	sampler2D dTexture;
	int dTextureSet;
	sampler2D sTexture;
	int sTextureSet;
};  
uniform Material material;

in vec3 viewPosition;
in vec2 textureC;
in vec3 fragNormal;

out vec4 color;

vec3 CalcSpotLightComponent(int i, vec3 albedo, vec3 specular, vec3 camDir) {
	vec3 lightDir = normalize(spotLights[i].pos - viewPosition);
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));
	
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), material.Ns);
	
	// blinn-phong
	vec3 shading = spotLights[i].color * (diffuseComponent * albedo) + specular * specularComponent;
	return spotLights[i].intensity * shading;
}

vec3 CalcDirLightComponent(int i, vec3 albedo, vec3 specular, vec3 camDir) {
	vec3 lightDir = normalize(-dirLights[i].dir);
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));
	
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), material.Ns);
	
	// blinn-phong
	vec3 shading = dirLights[i].color * (diffuseComponent * albedo) + specular * specularComponent;
	return dirLights[i].intensity * shading;
}

vec3 CalcAmbientLightComponent(int i, vec3 albedo) {
	return albedo * ambientLights[i].color * ambientLights[i].intensity;
}

void main() {
	vec3 albedo = (material.dTextureSet == 1) ? texture(material.dTexture, textureC).xyz * material.Kd : material.Kd;
	vec3 specular = (material.sTextureSet == 1) ? texture(material.sTexture, textureC).xyz * material.Ks : material.Ks;
	
	vec3 camDir = -normalize(viewPosition);
	vec3 shading = vec3(0.);
	for(int i=0; i<nSpotLights; i++) {
		shading += CalcSpotLightComponent(i, albedo, specular, camDir);
	}
	for(int i=0; i<nDirLights; i++) {
		shading += CalcDirLightComponent(i, albedo, specular, camDir);
	}
	for(int i=0; i<nAmbientLights; i++) {
		shading += CalcAmbientLightComponent(i, albedo);
	}
	
	color = vec4(shading, material.d);
}