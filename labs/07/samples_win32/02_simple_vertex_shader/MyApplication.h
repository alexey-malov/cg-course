// MyApplication.h: interface for the CMyApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYAPPLICATION_H__09A3E867_4376_437B_A540_73F3B1B233CC__INCLUDED_)
#define AFX_MYAPPLICATION_H__09A3E867_4376_437B_A540_73F3B1B233CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _USE_MATH_DEFINES
#include "Application.h"
#include <cmath>
#include <stdexcept>
#include <string>

class CMyApplication : public CApplication
{
public:
	CMyApplication(int* pArgc, char** argv, int width, int height, const char* title);

	~CMyApplication();

	void Init();

	virtual void OnDraw();

	void OnIdle();

private:
	void InitGLEW();

	void CreateVertexShader();

	void SetShaderSource();

	void CompileShader();

	void CreateProgram();

	void AttachShaders();

	void LinkProgram();

	void ValidateProgram();

	void InitUniforms();

	std::string GetInfoLog(GLhandleARB object);

	void Deinitialize();

private:
	ULONGLONG m_tickCount;
	GLint m_phaseUniformLocation;
	GLint m_amplitudeUniformLocation;
	GLint m_frequencyUniformLocation;
	GLhandleARB m_program;
	GLhandleARB m_vertexShader;
	double m_phase;
};

#endif // !defined(AFX_MYAPPLICATION_H__09A3E867_4376_437B_A540_73F3B1B233CC__INCLUDED_)