// Application.h: interface for the CApplication class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_APPLICATION_H__C9190678_3E75_4876_BE91_DBEE5A88131B__INCLUDED_)
#define AFX_APPLICATION_H__C9190678_3E75_4876_BE91_DBEE5A88131B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl/glew.h>
#include <gl/glut.h>
#include <iostream>

class CApplication
{
public:
	void Run();

	virtual ~CApplication();

protected:
	CApplication(int* pArgc, char** argv, int width, int height, const char* title);

	void EnableIdleHandler();

	// can be overriden
	virtual void OnDraw();

	// can be overriden
	virtual void OnIdle();

private:
	static void DisplayHandler();

	static void IdleHandler();

private:
	static CApplication* m_pApplication;
	int m_argc;
	char** m_argv;
};

#endif // !defined(AFX_APPLICATION_H__C9190678_3E75_4876_BE91_DBEE5A88131B__INCLUDED_)