#pragma once

class CShaderLoader
{
public:
	// Выполняем загрузку исходного кода шейдера из файла
	GLuint LoadShader(
		GLenum shaderType,
		const wchar_t* fileName,
		GLuint shaderId = 0);
	GLuint LoadShader(
		GLenum shaderType,
		const char* fileName,
		GLuint shaderId = 0);

	// Выполняем загрузку исходного кода шейдера из входного потока
	GLuint LoadShader(
		GLenum shaderType,
		std::istream& stream,
		GLuint shaderId = 0);
};
