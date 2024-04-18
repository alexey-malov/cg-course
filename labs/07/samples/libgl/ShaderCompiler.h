#pragma once

class CShaderCompiler
{
public:
	void CompileShader(GLuint shader);
	void CheckStatus();

private:
	std::vector<GLuint> m_shaders;
};
