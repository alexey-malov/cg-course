#pragma once

#include "Shaders.h"

class CProgramInfo
{
public:
	CProgramInfo(GLuint program);

	// Определяем по идентификатору типа его строковое представление
	static std::string TypeToString(GLenum type);

	// Возвращаем количество uniform-переменных в программе
	GLuint GetActiveUniforms()const;

	// Выводим информацию о заданной uniform-переменной в поток
	void PrintUniformInfo(GLuint index, std::ostream& stream)const;

	void Print(std::ostream& stream)const;

private:
	CProgramInfo(CProgramInfo const&);
	CProgramInfo& operator=(CProgramInfo const&);

	CProgramHandle m_program;
};
