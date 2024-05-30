#include "StdAfx.h"
#include "ShaderCompiler.h"
#include "Shaders.h"

CShaderCompiler::CShaderCompiler(void)
{
}

CShaderCompiler::~CShaderCompiler(void)
{
}

void CShaderCompiler::CompileShader(GLuint shader)
{
	CShaderHandle sh(shader);
	sh.Compile();
	m_shaders.push_back(shader);
}

void CShaderCompiler::CheckStatus()
{
	std::stringstream strm;

	bool hasErrors = false;

	for (size_t i = 0; i < m_shaders.size(); ++i)
	{
		CShaderHandle shader(m_shaders[i]);
		if (shader.GetParameter(GL_COMPILE_STATUS) != GL_TRUE)
		{
			hasErrors = true;
			strm << "Shader " << shader << " compilation failed: " << shader.GetInfoLog() << "\n";
		}
	}

	m_shaders.clear();

	if (hasErrors)
	{
		throw std::runtime_error(strm.str());
	}
}
