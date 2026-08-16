#pragma once
#include <glad/glad.h>


struct Shader
{
	GLuint id = 0;

	bool loadShaderProgramFromFile(const char *vertexShaderPath, const char *fragmentShaderPath);
	
	bool loadShaderProgramFromData(const char *vertexShaderData, const char *fragmentShaderData);

	void bind();

	void clear();
};
