// 01_initialization.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <gl/glew.h>
#include <GL/glut.h>
#include <iostream>

void PrintShaderSupportInfo();

int main(int argc, char* argv[])
{
	// initialize glut
	glutInit(&argc, argv);

	// create window
	glutCreateWindow("Vertex shader initialization");

	// initialize glew library
	glewInit();

	PrintShaderSupportInfo();

	return 0;
}

void PrintShaderSupportInfo()
{
	// check if OpenGL implementation supports GLSL Shaders
	if (GLEW_ARB_shader_objects)
	{
		std::cout << "GLSL shader objects are supported\n";
	}
	else
	{
		std::cout << "GLSL shader objects are not supported\n";
	}
}