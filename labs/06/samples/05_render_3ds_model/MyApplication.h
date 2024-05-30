#pragma once
#include "glapplication.h"
#include "Buffer.h"
#include "RotationController.h"
#include "Model.h"
#include "ModelRenderer.h"


class CMyApplication : public CGLApplication
{
public:
	CMyApplication(const char * title, int width, int height);
	~CMyApplication(void);
protected:
	virtual void OnInit();
	virtual void OnDisplay();
	virtual void OnReshape(int width, int height);
	virtual void OnMotion(int x, int y);
	virtual void OnMouse(int button, int state, int x, int y);
private:
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	CRotationController m_rotationController;

	CModel m_model;
	CModelRenderer m_renderer;
};
