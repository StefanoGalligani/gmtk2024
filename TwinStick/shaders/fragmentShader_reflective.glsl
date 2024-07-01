#version 330 core

uniform vec3 lightPos;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float Ns;
uniform float d;

uniform sampler2D dTexture;
uniform int dTextureSet;
uniform sampler2D sTexture;
uniform int sTextureSet;
uniform sampler2D aTexture;
uniform int aTextureSet;

uniform samplerCube skybox;
uniform int skyboxSet;
uniform sampler2DRect reflectedTexture;
uniform int reflectedTextureSet;

uniform mat3 toWorld;

in vec3 viewPosition;
in vec2 textureC;
in vec3 fragNormal;

out vec4 color;

void main() {
	vec3 albedo = (dTextureSet == 1) ? texture(dTexture, textureC).xyz : Kd;
	vec3 specular = (sTextureSet == 1) ? texture(sTexture, textureC).xyz : Ks;
	vec3 ambient = (aTextureSet == 1) ? texture(aTexture, textureC).xyz : Ka;
	float ambientLight = 1;
	float intensity = 0.9;

	vec3 lightDir = normalize(lightPos - viewPosition);
	float diffuseComponent = max(0, dot(lightDir, normalize(fragNormal)));

	vec3 camDir = -normalize(viewPosition);
	vec3 bounceDir = normalize(lightDir + camDir);
	float specularComponent = pow(max(0, dot(bounceDir, fragNormal)), Ns);

	vec4 textureValue = vec4(0);
	if (reflectedTextureSet == 1) {
		textureValue = texture(reflectedTexture, gl_FragCoord.xy).xyzw;
		ambient = textureValue.xyz*Ka;
	}
	if (textureValue.w < 0.1) {
		vec3 camDirWorld = toWorld * camDir;
		vec3 normalWorld = toWorld * fragNormal;
		vec3 skyDir = -camDirWorld + normalWorld * dot(camDirWorld, normalWorld) * 2;
		ambient = (skyboxSet == 1) ? texture(skybox, skyDir).xyz * Ka : ambient;
	} else {
		ambient = textureValue.xyz*Ka;
	}

	//modified blinn-phong
	color = vec4(intensity * 
		diffuseComponent*(albedo + specular * specularComponent)
		+ ambient * ambientLight, d);
}