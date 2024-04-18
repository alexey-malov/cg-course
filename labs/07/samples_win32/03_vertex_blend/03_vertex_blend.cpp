// 03_vertex_blend.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _USE_MATH_DEFINES
#include "../libgl/libgl.h"
#include <math.h>
#include <windows.h>

struct Vertex
{
	float x0, y0, z0;
	float x1, y1, z1;
};

const unsigned NUMBER_OF_VERTICES = 40 * 4;
Vertex g_shapeVertices[NUMBER_OF_VERTICES];

GLhandleARB g_program;

DWORD g_lastTick = GetTickCount();

void InitShape(float radius, Vertex* vertexArray, unsigned const numberOfVertices)
{
	unsigned pointsPerSide = (numberOfVertices + 3) / 4;

	for (unsigned i = 0; i < numberOfVertices; ++i)
	{
		Vertex& v = vertexArray[i];

		float angle = (float)(2 * M_PI * i / numberOfVertices);
		v.x0 = float(radius * cos(angle));
		v.y0 = float(radius * sin(angle));
		v.z0 = 0;

		float radius1 = (float)(radius * 0.7 + 0.6 * radius * cos(6 * angle));
		v.x1 = float(radius1 * cos(angle));
		v.y1 = float(radius1 * sin(angle));
		v.z1 = 0;
	}
}

void Draw()
{
	DWORD currentTick = (DWORD)GetTickCount64();
	float time = (currentTick - g_lastTick) / 1000.0f;
	g_lastTick = currentTick;

	static float workTime = 0;
	workTime += time;
	if (workTime >= 3)
	{
		workTime -= 3;
	}

	float phase = float(0.5f * sin(workTime * M_PI / 1.5f) + 0.5f);

	glClear(GL_COLOR_BUFFER_BIT);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &g_shapeVertices[0]);

	// enable shader program
	glUseProgramObjectARB(g_program);

	// turn on vertex array
	glEnableClientState(GL_VERTEX_ARRAY);

	// get the location of the "vertex2" attribute variable (see blend.vsh shader for details)
	GLint vertex2Attribute = glGetAttribLocationARB(g_program, "vertex2");

	// set pointer to the vertex array
	glVertexAttribPointerARB(vertex2Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &g_shapeVertices[0].x1);

	// get the location of the "phase" uniform variable (see blend.vsh shader for details)
	GLint phaseLocation = glGetUniformLocationARB(g_program, "phase");
	glUniform1fARB(phaseLocation, phase);

	// enable array of vertex2 attributes
	glEnableVertexAttribArrayARB(vertex2Attribute);

	// draw the vertex array
	glDrawArrays(GL_LINE_LOOP, 0, NUMBER_OF_VERTICES);

	// turn off vertex2 array
	glDisableVertexAttribArrayARB(vertex2Attribute);

	// do not use vertex array
	glDisableClientState(GL_VERTEX_ARRAY);

	// do not use program object
	glUseProgramObjectARB(NULL);

	glutSwapBuffers();
}

void InitApplication()
{
	glewInit();

	// check if shaders are supported
	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("GL_ARB_shader_objects OpenGL extension is not supported.");
	}

	g_program = glCreateProgramObjectARB();

	GLhandleARB vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

	LoadShaderSource(vertexShader, "blend.vsh");

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

int main(int argc, char* argv[])
{
	InitShape(0.7f, g_shapeVertices, NUMBER_OF_VERTICES);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Vertex blending");
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
