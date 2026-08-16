#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vFragPos;
out vec3 vNormal;
out vec3 vColor;

void main()
{
	vFragPos = vec3(uModel * vec4(in_pos, 1.0));
	vNormal = mat3(transpose(inverse(uModel))) * in_normal;

	gl_Position = uProj * uView * vec4(vFragPos, 1.0);
    vColor = in_pos + 0.5;
}
