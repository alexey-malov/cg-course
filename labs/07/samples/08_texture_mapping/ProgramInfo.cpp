#include "StdAfx.h"
#include "ProgramInfo.h"

CProgramInfo::CProgramInfo(GLuint program)
	: m_program(program)
{
}

GLuint CProgramInfo::GetActiveUniforms() const
{
	return m_program.GetActiveUniforms();
}

void CProgramInfo::PrintUniformInfo(GLuint index, std::ostream& stream) const
{
	GLint uniformSize = 0;
	GLenum uniformType = 0;

	// Получаем информацию об активной uniform-переменной
	std::string name = m_program.GetActiveUniform(index, &uniformSize, &uniformType);
	// Получаем строковое представление ее типа
	std::string type = TypeToString(uniformType);

	// Выводим тип и имя переменной
	stream << type << " "
		   << " " << name;

	// Если это массив, то выводим его размер
	if (uniformSize != 1)
	{
		stream << "[" << uniformSize << "]";
	}

	// Если это не встроенная переменная, то выводим ее расположение
	if (name.length() > 3 && name.substr(0, 3) != "gl_")
	{
		GLint location = m_program.GetUniformLocation(name.c_str());
		stream << " at " << location;
	}
}

std::string CProgramInfo::TypeToString(GLenum type)
{
	struct TypeMap
	{
		GLenum type;
		const char* typeName;
	};

	TypeMap const typeMap[] = {
		{ GL_FLOAT, "float" },
		{ GL_FLOAT_VEC2, "vec2" },
		{ GL_FLOAT_VEC3, "vec3" },
		{ GL_FLOAT_VEC4, "vec4" },
		{ GL_INT, "int" },
		{ GL_INT_VEC2, "ivec2" },
		{ GL_INT_VEC3, "ivec3" },
		{ GL_INT_VEC4, "ivec4" },
		{ GL_BOOL, "bool" },
		{ GL_BOOL_VEC2, "bvec2" },
		{ GL_BOOL_VEC3, "bvec3" },
		{ GL_BOOL_VEC4, "bvec4" },
		{ GL_FLOAT_MAT2, "mat2" },
		{ GL_FLOAT_MAT3, "mat3" },
		{ GL_FLOAT_MAT4, "mat4" },
		{ GL_FLOAT_MAT2x3, "mat2x3" },
		{ GL_FLOAT_MAT2x4, "mat2x4" },
		{ GL_FLOAT_MAT3x2, "mat3x2" },
		{ GL_FLOAT_MAT3x4, "mat3x4" },
		{ GL_FLOAT_MAT4x2, "mat4x2" },
		{ GL_FLOAT_MAT4x3, "mat4x3" },
		{ GL_SAMPLER_1D, "sampler1D" },
		{ GL_SAMPLER_2D, "sampler2D" },
		{ GL_SAMPLER_3D, "sampler3D" },
		{ GL_SAMPLER_CUBE, "samplerCube" },
		{ GL_SAMPLER_1D_SHADOW, "sampler1DShadow" },
		{ GL_SAMPLER_2D_SHADOW, "sampelr2DShadow" }
	};
	static const unsigned TYPE_MAP_ITEMS_COUNT = sizeof(typeMap) / sizeof(*typeMap);
	// Ищем среди имеющихся типов тип с нужным идентификатором
	for (unsigned int i = 0; i < TYPE_MAP_ITEMS_COUNT; ++i)
	{
		if (type == typeMap[i].type)
		{
			return typeMap[i].typeName;
		}
	}
	throw std::invalid_argument("Unknown variable type");
}

void CProgramInfo::Print(std::ostream& stream) const
{
	stream << "Program id: " << m_program.Get() << "\n";

	// Если в программе есть активные uniform-переменные, выводим информацию о них
	if (m_program.GetActiveUniformMaxLength())
	{
		GLuint const activeUniforms = GetActiveUniforms();
		stream << "  Active uniforms: " << activeUniforms << "\n";
		for (GLuint uniform = 0; uniform < activeUniforms; ++uniform)
		{
			stream << "    ";
			PrintUniformInfo(uniform, std::cout);
			stream << "\n";
		}
	}
}
