#pragma once

class CShaderCompiler
{
public:
	CShaderCompiler(void);
	~CShaderCompiler(void);
	void CompileShader(GLuint shader);
	void CheckStatus();
private:
	std::vector<GLuint> m_shaders;
};
