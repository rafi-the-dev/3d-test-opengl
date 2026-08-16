#version 330 core

layout (location = 0) out vec4 out_color;

in vec3 vFragPos;
in vec3 vNormal;
in vec3 vColor;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uObjectColor;
uniform vec3 uViewPos;

void main()
{
	float ambientStrength = 0.15;
	vec3 ambient = ambientStrength * uLightColor;

	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(uLightPos - vFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * uLightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(uViewPos - vFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * uLightColor;

	vec3 result = (ambient + diffuse + specular) * vColor;
	out_color = vec4(result, 1.0);
}
