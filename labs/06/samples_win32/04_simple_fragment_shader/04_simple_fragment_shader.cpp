// 04_simple_fragment_shader.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _USE_MATH_DEFINES
#include "../libgl/libgl.h"
#include <windows.h>
#include <math.h>

GLhandleARB g_program;

void InitApplication()
{
	glewInit();

	// check if shaders
	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("GL_ARB_shader_objects OpenGL extension is not supported.");
	}

	g_program = glCreateProgramObjectARB();

	GLhandleARB vertexShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	LoadShaderSource(vertexShader, "checker.fsh");

	CompileShader(vertexShader);

	glAttachObjectARB(g_program, vertexShader);

	LinkProgram(g_program);

	// we have just linked the program and now can delete vertex shader since it is not needed anymore
	glDeleteObjectARB(vertexShader);
}

void DestroyApplication()
{
	if (g_program)
	{
		glDeleteObjectARB(g_program);
	}
}

void Idle()
{
	glutPostRedisplay();
}

void Draw()
{
	glClearColor(0.3f, 0.4f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgramObjectARB(g_program);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex2f((GLfloat)(-0.8), (GLfloat)(-0.8));

		glTexCoord2f(4, 0);
		glVertex2f((GLfloat)(0.8), (GLfloat)(- 0.8));

		glTexCoord2f(4, 4);
		glVertex2f((GLfloat)(0.8), (GLfloat)(0.8));

		glTexCoord2f(0, 4);
		glVertex2f((GLfloat)(-0.8), (GLfloat)(0.8));
	}
	glEnd();
	glUseProgramObjectARB(NULL);
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Simple fragment shader");
	glutDisplayFunc(Draw);
	glutIdleFunc(Idle);

	try
	{
		InitApplication();
		glutMainLoop();
	}
	catch (std::runtime_error& e)
	{
		std::cout << "Error: \n" << e.what();
	}

	DestroyApplication();
	return 0;
}