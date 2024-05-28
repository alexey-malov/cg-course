#include "StdAfx.h"
#include "MyApplication.h"
#include "ModelLoader.h"
#include "Material.h"
#include "DirectLight.h"

// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
,m_rotationController(width, height)
{
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1);
	
	// Включаем свет и задаем его параметры
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	CDirectLight light;
	light.SetSpecularIntensity(0.3f, 0.3f, 0.3f);
	light.SetLight(GL_LIGHT0);

	// Задаем параметры материала
	CMaterial material;
	material.SetSpecular(0.3f, 0.3f, 0.3f);
	material.SetShininess(20);
	material.SetDiffuse(0.8f, 0.8f, 0.2f);
	material.Activate();

	// Загружаем трехмерную модель
	CModelLoader loader;
	loader.Load3dsFile("loader.3DS", m_model);

	// Определяем ограничивающий блой данной модели
	CBoundingBox const& modelBoundingBox = m_model.GetBoundingBox();

	if (modelBoundingBox.IsEmpty())
	{
		throw std::runtime_error("Model is empty. Nothing to render");
	}

	// Центр bounding box-а модели
	CVector3f modelCenter = modelBoundingBox.GetCenter();
	// Длина диагонали bounding box-а
	float modelDiagonal = modelBoundingBox.GetSize().GetLength();
	// Отодвигаем камеру от центра на расстояние, равное диагонали 
	// bounding box-а
	CVector3f cameraPosition = modelCenter + CVector3f(modelDiagonal, 0, 0);

	// Задаем параметры камеры
	glLoadIdentity();
	gluLookAt(
		cameraPosition.x, cameraPosition.y, cameraPosition.z, 
		modelCenter.x, modelCenter.y, modelCenter.z, 
		0, 1, 0);
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3ub(0, 0, 0);
	glEnable(GL_CULL_FACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_renderer.RenderModel(m_model);
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	float zFar = m_model.GetBoundingBox().GetSize().GetLength() * 2;
	float zNear = zFar * (float)0.01;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void CMyApplication::OnMouse(int button, int state, int x, int y)
{
	m_rotationController.OnMouse(button, state, x, y);
}

void CMyApplication::OnMotion(int x, int y)
{
	if (m_rotationController.LeftButtonIsPressed())
	{
		m_rotationController.OnMotion(x, y);
		PostRedisplay();
	}
}
