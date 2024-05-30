#pragma once
#include "glapplication.h"
#include "Buffer.h"
#include "Buffer.h"
#include "RotationController.h"


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
	void DrawCube()const;
	void InitArrays();
	// Угол обзора по вертикали
	static const double FIELD_OF_VIEW;

	// Расстояние до ближней и дальней плоскостей отсечения
	static const double ZNEAR;
	static const double ZFAR;

	CRotationController m_rotationController;

	CVertexBuffer m_vertices;
	CIndexBuffer m_indices;

	GLubyte const * m_pVertexArray;
	GLubyte const * m_pColorArray;
	GLubyte const * m_pIndexArray;
};
