#include "pch.h"
#include "ShaderLoader.h"
#include "Shaders.h"

GLuint CShaderLoader::LoadShader(
	GLenum shaderType,
	std::istream& stream,
	GLuint shaderId)
{
	// Копируем данные из файлового потока в строковый поток
	std::stringstream strStream;
	strStream << stream.rdbuf();

	// Получаем строку, содержащую исходный код шейдера
	std::string shaderSource = strStream.str();

	CShader shader(shaderId);
	// Создаем новый шейдер
	// (если в нам был передан нулевой идентификатор шейдера)
	if (!shader)
	{
		shader.Create(shaderType);
	}
	shader.SetSource(shaderSource.c_str());

	// Возвращаем идентификатор шейдерного объекта
	return shader.Detach();
}

GLuint CShaderLoader::LoadShader(GLenum shaderType, const char* fileName, GLuint shaderId)
{
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		throw std::runtime_error("Can't open shader file");
	}
	return LoadShader(shaderType, stream, shaderId);
}

GLuint CShaderLoader::LoadShader(GLenum shaderType, const wchar_t* fileName, GLuint shaderId)
{
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		throw std::runtime_error("Can't open shader file");
	}
	return LoadShader(shaderType, stream, shaderId);
}
