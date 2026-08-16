#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <glm/glm.hpp>                   // core types: vec3, mat4, normalize, dot, cross
#include <glm/gtc/matrix_transform.hpp>  // rotate, translate, scale, perspective, ortho, lookAt
#include <glm/gtc/type_ptr.hpp>          // value_ptr
#define GLFW_INCLUDE_NONE
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "OpenGlerrorReporting.h"
#include "shader.h"
#include <imgui.h>

glm::vec3 camPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 worldUp  = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw   = -90.0f;
float pitch =   0.0f;

float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,   0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f,  -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f,  -3.5f),
};

float triangleData[] = {
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	// back (-z)
	 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	// left (-x)
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	// right (+x)
	 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
	// top (+y)
	-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
	// bottom (-y)
	-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
};
glm::vec3 cubeColors[] = {
	glm::vec3(1.0f, 0.3f, 0.3f),   // red
	glm::vec3(0.3f, 1.0f, 0.3f),   // green
	glm::vec3(0.3f, 0.5f, 1.0f),   // blue
	glm::vec3(1.0f, 0.9f, 0.2f),   // yellow
	glm::vec3(0.8f, 0.3f, 1.0f),   // purple
};
unsigned int triangleindecies[] =
{
	 0, 1, 2,   0, 2, 3,
	 4, 5, 6,   4, 6, 7,
	 8, 9,10,   8,10,11,
	12,13,14,  12,14,15,
	16,17,18,  16,18,19,
	20,21,22,  20,22,23,
};

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;   // reversed: screen y grows downward
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    yaw   += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(dir);
}

void processInput(GLFWwindow* window)
{
    float speed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += speed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= speed * camFront;

    glm::vec3 right = glm::normalize(glm::cross(camFront, worldUp));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= speed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += speed * right;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)         camPos += speed * worldUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)    camPos -= speed * worldUp;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "GLFW Init error!\n";
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(640, 480, "test", NULL, NULL);
	if (!window)
	{
		std::cout << "window error\n";
		return 1;
	}

	glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return 1;
	}
	enableReportGlErrors();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#pragma region index buffer
	GLuint indexbuffer = 0;
	glGenBuffers(1, &indexbuffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleindecies), triangleindecies, GL_STATIC_DRAW);
#pragma endregion
#pragma region buffer
	GLuint buffer = 0; 
	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleData), triangleData, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
#pragma endregion

	//glBindVertexArray(0);
	Shader shader;

	shader.loadShaderProgramFromFile(RESOURCES_PATH "shaders/triangle/triangle.vert", RESOURCES_PATH "shaders/triangle/triangle.frag");

	shader.bind();
    std::cout << "id=" << shader.id
          << " model=" << glGetUniformLocation(shader.id, "uModel")
          << " view="  << glGetUniformLocation(shader.id, "uView")
          << " proj="  << glGetUniformLocation(shader.id, "uProj") << "\n";

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		float aspect = (h == 0) ? 1.0f : (float)w / (float)h;
		glViewport(0, 0, w, h);

		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::lookAt(camPos, camPos + camFront, worldUp);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader.id, "uView"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.id, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
	    glUniform3fv(glGetUniformLocation(shader.id, "uLightPos"), 1, glm::value_ptr(lightPos));
	    glUniform3f(glGetUniformLocation(shader.id, "uLightColor"), 1.0f, 1.0f, 1.0f);
	    //glUniform3f(glGetUniformLocation(shader.id, "uObjectColor"), 1.0f, 0.5f, 0.31f);
	    glUniform3fv(glGetUniformLocation(shader.id, "uViewPos"), 1, glm::value_ptr(camPos));

		glBindVertexArray(vao);

		for (int i = 0; i < cubePositions->length() + 1; i++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::rotate(model,
				glm::radians(20.0f * i) + (float)glfwGetTime() * 0.3f,
				glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));

			glUniformMatrix4fv(glGetUniformLocation(shader.id, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
