
#include "../libgl/GdiPlusInitializer.h"
#include "../libgl/TextureLoader.h"
#include "../libgl/stdafx.h"
#include "Visuals.h"
#include "../libgl/Graphics.h"
#include "../libgl/utils.h"

CGdiPlusInitializer gdi;
CTextureLoader textureLoader;

GLhandleARB g_vertexShader = 0;
GLhandleARB g_fragmentShader = 0;
GLhandleARB g_programObject = 0;
GLuint g_texture = 0;

bool LoadTextures();
void DeleteTextures();

std::string LoadTextFile(const char fileName[])
{
	FILE * pFile;
	fopen_s(&pFile, fileName, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		size_t length = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		char * buf = new char[length];
		fread(buf, 1, length, pFile);
		fclose(pFile);
		std::string shaderSource(buf, length);;
		delete [] buf;
		return shaderSource;
	}
	return "";
}


void WriteLog(std::string const& str)
{
	FILE *pFile;
	fopen_s(&pFile, "log.txt", "at");
	if (pFile)
	{
		fwrite(str.c_str(), 1, str.length(), pFile);
		fclose(pFile);
	}
}

bool LoadShaderSource(GLhandleARB shader, const char fileName[])
{
	WriteLog(std::string("loading shader ") + fileName + std::string("\n"));
	std::string shaderSrc = LoadTextFile(fileName);
	if (!shaderSrc.empty())
	{
		const char* shaderstring = shaderSrc.c_str();
		const char** pshaderstring = &shaderstring;
		glShaderSourceARB(shader, 1, pshaderstring, NULL);
		WriteLog("Ok\n");
		return true;
	}
	WriteLog("Failed\n");
	return false;
}

std::string GetInfoLog(GLhandleARB shader)
{
	GLcharARB buffer[10000];
	GLsizei length;
	glGetInfoLogARB(shader, sizeof(buffer) - 1, &length, buffer);
	return std::string(buffer, length);
}

bool CompileShader(GLhandleARB shader)
{
	WriteLog("Compile shader\n");
	glCompileShaderARB(shader);
	GLint compileStatus;
	glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);
	bool result = (compileStatus == GL_TRUE);

	if (!result)
	{
		WriteLog(GetInfoLog(shader) + "\n");
	}
	else
	{
		WriteLog("\n");
	}
	return result;
}

bool LinkProgram(GLhandleARB program)
{
	glLinkProgramARB(program);
	WriteLog("Linking program\n");
	GLint linkStatus;
	glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

	if (!linkStatus)
	{
		WriteLog(GetInfoLog(program));
	}
	else
	{
		WriteLog("Ok\n");
	}

	return linkStatus == GL_TRUE;
}

bool InitShaders()
{
	if (
		(!GLEW_ARB_shader_objects) ||
		(!GLEW_ARB_vertex_shader) ||
		(!GLEW_ARB_fragment_shader)
		)
	{
		return false;
	}

	g_vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	g_fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	WriteLog("Loading shaders\n");

	if (!LoadShaderSource(g_vertexShader, "shaders/vertexshader.vs"))
	{
		return false;
	}

	if (!LoadShaderSource(g_fragmentShader, "shaders/fragmentshader.fs"))
	{
		return false;
	}

	if (!CompileShader(g_vertexShader))
	{
		return false;
	}
	
	if (!CompileShader(g_fragmentShader))
	{
		return false;
	}
	
	g_programObject = glCreateProgramObjectARB();
	glAttachObjectARB(g_programObject, g_vertexShader);
	glAttachObjectARB(g_programObject, g_fragmentShader);
	if (!LinkProgram(g_programObject))
	{
		return false;
	}

	return true;
}

void DeleteShaders()
{
	if (
		(!GLEW_ARB_shader_objects) ||
		(!GLEW_ARB_vertex_shader) ||
		(!GLEW_ARB_fragment_shader)
		)
	{
		return;
	}

	glDeleteObjectARB(g_vertexShader);
	glDeleteObjectARB(g_fragmentShader);
	glDeleteObjectARB(g_programObject);
}

bool InitGraphics()
{
	FILE *pFile;
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
	g_texture = textureLoader.LoadTexture2D(L"tex.bmp");
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

