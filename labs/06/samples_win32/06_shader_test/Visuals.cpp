#include "../libgl/libgl.h"

#include "stdafx.h"
#include "Graphics.h"
#include "Visuals.h"
#include "utils.h"

#include <stdexcept>

GLhandleARB g_vertexShader = 0;
GLhandleARB g_fragmentShader = 0;
GLhandleARB g_programObject = 0;
GLuint g_texture = 0;

bool LoadTextures();
void DeleteTextures();

void WriteLog(std::string const& str)
{
	FILE* pFile;
	fopen_s(&pFile, "log.txt", "at");
	if (pFile)
	{
		fwrite(str.c_str(), 1, str.length(), pFile);
		fclose(pFile);
	}
}

bool InitShaders()
{
	if (
		(!GLEW_ARB_shader_objects) || (!GLEW_ARB_vertex_shader) || (!GLEW_ARB_fragment_shader))
	{
		return false;
	}

	g_vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	g_fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	WriteLog("Loading shaders\n");

	try
	{
		LoadShaderSource(g_vertexShader, "shaders/vertexshader.vs");
		LoadShaderSource(g_fragmentShader, "shaders/fragmentshader.fs");

		CompileShader(g_vertexShader);
		CompileShader(g_fragmentShader);

		g_programObject = glCreateProgramObjectARB();
		glAttachObjectARB(g_programObject, g_vertexShader);
		glAttachObjectARB(g_programObject, g_fragmentShader);

		LinkProgram(g_programObject);
	}
	catch (const std::runtime_error&)
	{
		return false;
	}

	return true;
}

void DeleteShaders()
{
	if (
		(!GLEW_ARB_shader_objects) || (!GLEW_ARB_vertex_shader) || (!GLEW_ARB_fragment_shader))
	{
		return;
	}

	glDeleteObjectARB(g_vertexShader);
	glDeleteObjectARB(g_fragmentShader);
	glDeleteObjectARB(g_programObject);
}

bool InitGraphics()
{
	FILE* pFile;
	fopen_s(&pFile, "log.txt", "wt");
	if (pFile)
	{
		fclose(pFile);
	}

	if (!InitShaders())
	{
		return false;
	}

	if (!LoadTextures())
	{
		return false;
	}

	return true;
}

bool LoadTextures()
{
	g_texture = LoadTextureFromRgbBitmapFile("tex.bmp");
	if (!g_texture)
	{
		return false;
	}

	return true;
}

void DeleteTextures()
{
	glDeleteTextures(1, &g_texture);
}

void DeleteGraphics()
{
	DeleteShaders();
	DeleteTextures();
}

void DrawScene()
{
	// просто очищаем буфер рисования
	ClearBuffers(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);

	glBindTexture(GL_TEXTURE_2D, g_texture);

	glUseProgramObjectARB(g_programObject);
	glBegin(GL_TRIANGLES);
	{
		glColor3f(0.9f, 0.7f, 0.1f);
		glTexCoord2f(0, 0);
		glVertex3f(0.5f, 0.7f, 0.0f);

		glColor3f(0.8f, 0.4f, 0.6f);
		glTexCoord2f(1, 0);
		glVertex3f(-0.5f, 0.0f, 0.0f);

		glColor3f(0.5f, 0.3f, 0.9f);
		glTexCoord2f(0, 1);
		glVertex3f(0.5f, -0.7f, 0.0f);
	}
	glEnd();
	// завершающие действия - дожидаемся конца рисования и
	// меняем внеэкранный и экранный буферы местами
	EndDrawing();
}
