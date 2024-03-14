#pragma once

class CProgramLinker
{
public:
	void LinkProgram(GLuint program);
	void CheckStatus();

private:
	std::vector<GLuint> m_programs;
};
