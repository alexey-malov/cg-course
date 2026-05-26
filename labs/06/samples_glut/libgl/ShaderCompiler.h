#pragma once

class CShaderCompiler
{
public:
	CShaderCompiler(void);
	~CShaderCompiler(void);
	void CompileShader(GLuint shader);
	void CheckStatus();
private:
	CShaderCompiler(CShaderCompiler const&);
	CShaderCompiler & operator=(CShaderCompiler const&);
	std::vector<GLuint> m_shaders;
};
