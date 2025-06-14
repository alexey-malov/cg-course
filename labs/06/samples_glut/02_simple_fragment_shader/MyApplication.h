#pragma once
#include "../libgl/GLApplication.h"

#include "../libgl/Shaders.h"

class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
private:
	void InitShaders();

	CProgram m_program;
};
