#pragma once
#include "Program.h"

class DiffuseLighting
{
public:
	DiffuseLighting();

	GLuint GetProgramId() const noexcept { return m_program.GetId(); }

private:
	Program m_program;
};
