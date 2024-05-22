#pragma once
#include "Program.h"

class DiffuseAndSpecular
{
public:
	DiffuseAndSpecular();

	GLuint GetProgramId() const noexcept { return m_program.GetId(); };

private:
	Program m_program;
};