#version 330 core

uniform vec3 lightPos;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float Ns;
uniform float d;
uniform float ambientLight;

uniform sampler2D dTexture;
uniform int dTextureSet;
uniform sampler2D sTexture;
uniform int sTextureSet;
uniform sampler2D aTexture;
uniform int aTextureSet;
uniform sampler2DShadow shadowTexture;
uniform sampler2D normalTexture;

uniform mat3 mvt;

in vec3 viewPosition;
in vec2 textureC;
//in vec3 fragNormal;
in vec4 lightViewPosition;

out vec4 color;

void main() {
	vec3 albedo = (dTextureSet == 1) ? texture(dTexture, textureC).xyz : Kd;
	vec3 specular = (sTextureSet == 1) ? texture(sTexture, textureC).xyz : Ks;
	vec3 ambient = (aTextureSet == 1) ? texture(aTexture, textureC).xyz : Ka;
	float intensity = 0.9;

	vec3 lightDir = normalize(lightPos - viewPosition);
	vec3 fragNormal = ((texture(normalTexture, textureC).xyz - 0.5)*2);
	fragNormal = vec3(fragNormal.x, fragNormal.z, -fragNormal.y); //implicit rotation of the plane
	fragNormal = mvt * fragNormal;
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));
	
	vec3 p = lightViewPosition.xyz / lightViewPosition.w;
	if (p.x > 1 || p.x < 0 || p.y > 1 || p.y < 0 || p.z > 1.0) { diffuseComponent = 0; }
	else { diffuseComponent *= texture(shadowTexture, p); }

	vec3 camDir = -normalize(viewPosition);
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), Ns);

	//modified blinn-phong
	color = vec4(intensity * 
		diffuseComponent*(albedo + specular * specularComponent)
		+ ambient * ambientLight, d);
	//color = vec4(viewPosition.xy, 0, 1);
}