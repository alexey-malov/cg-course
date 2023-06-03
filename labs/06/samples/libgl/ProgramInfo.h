#pragma once

#include "Shaders.h"

class CProgramInfo
{
public:
	CProgramInfo(GLuint program);

	CProgramInfo(CProgramInfo const&) = delete;
	CProgramInfo& operator=(CProgramInfo const&) = delete;

	// Определяем по идентификатору типа его строковое представление
	static std::string TypeToString(GLenum type);

	// Возвращаем количество uniform-переменных в программе
	GLuint GetActiveUniforms() const;

	// Возвращаем количество attribute-переменных в программе
	GLuint GetActiveAttributes() const;

	// Выводим информацию о заданной uniform-переменной в поток
	void PrintUniformInfo(GLuint index, std::ostream& stream) const;
	void PrintAttributeInfo(GLuint index, std::ostream& stream) const;

	void Print(std::ostream& stream) const;

	CProgramHandle m_program;
};
