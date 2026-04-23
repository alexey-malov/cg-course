#include "pch.h"
#include "ProgramLinker.h"
#include "Shaders.h"

CProgramLinker::CProgramLinker(void)
{
}

CProgramLinker::~CProgramLinker(void)
{
}

void CProgramLinker::LinkProgram(GLuint program)
{
	// Компонуем программу и добавляем ее в массив
	// компонуемых программ
	CProgramHandle prog(program);
	prog.Link();
	m_programs.push_back(program);
}

// Проверяем статус скомпонованности шейдерных программ
void CProgramLinker::CheckStatus()
{
	std::stringstream strm;

	bool hasErrors = false;

	for (size_t i = 0; i < m_programs.size(); ++i)
	{
		CProgramHandle program(m_programs[i]);
		if (program.GetParameter(GL_LINK_STATUS) != GL_TRUE)
		{
			hasErrors = true;
			strm << "Program " << program << " linkage failed: " << 
				program.GetInfoLog() << "\n";
		}
	}

	m_programs.clear();

	if (hasErrors)
	{
		throw std::runtime_error(strm.str());
	}
}
