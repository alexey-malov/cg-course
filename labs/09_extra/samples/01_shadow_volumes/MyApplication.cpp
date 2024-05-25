#include "StdAfx.h"
#include "MyApplication.h"

const double CMyApplication::FOV = 60;
const double CMyApplication::ZNEAR = 1;

// Величина вытягивания вершин
const float CMyApplication::EXTRUSION_FACTOR = 100;

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height, true, true)
,m_rotationController(width, height)
,m_lightAnimationPhase(0)
,m_torusAnimationPhase(0)
,m_lightPosition(0)
{
	AddEventListener(&m_rotationController);
	m_rotationController.AddEventListener(this);

	m_light.SetAmbientIntensity(0.2f, 0.2f, 0.2f);
	m_light.SetDiffuseIntensity(0.8f, 0.8f, 0.8f);
	m_light.SetSpecularIntensity(0.4f, 0.4f, 0.4f);
	m_light.SetPosition(glm::vec3(3.0f, 3.0f, 2.0f ));

	m_material.SetShininess(30);
	m_material.SetAmbient(static_cast<GLfloat>(0.2), static_cast<GLfloat>(0.1), static_cast<GLfloat>(0.3));
	m_material.SetDiffuse(static_cast<GLfloat >(0.5), static_cast<GLfloat>(0.6), static_cast<GLfloat>(0.7));
	m_material.SetSpecular(static_cast<GLfloat>(0.3), static_cast<GLfloat>(0.3), static_cast<GLfloat>(0.3));
}

void CMyApplication::OnInit()
{
	glClearColor(static_cast<GLclampf>(0.3), static_cast<GLclampf>(0.3), static_cast<GLclampf>(0.3), static_cast<GLclampf>(1));

	glLoadIdentity();
	glm::dmat4x4 modelView = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, 10.0 },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });
	m_rotationController.SetModelViewMatrix(modelView);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void CMyApplication::OnIdle()
{
	m_animationController.Tick();

	static const float LIGHT_ANIMATION_SPEED = 40.0f * static_cast<float>(M_PI) / 180.0f;

	m_lightAnimationPhase = fmodf(
		static_cast<float>(m_lightAnimationPhase + 
		LIGHT_ANIMATION_SPEED * m_animationController.GetTimeDelta() * 0.001), 
		static_cast<float>(2 * M_PI));

	static const float LIGHT_ROTATION_RADIUS = 3;
	m_lightPosition.x = cosf(m_lightAnimationPhase) * LIGHT_ROTATION_RADIUS + 2;
	m_lightPosition.y = sinf(m_lightAnimationPhase) * LIGHT_ROTATION_RADIUS + 2;
	m_lightPosition.z = sin(m_lightAnimationPhase * 2) * LIGHT_ROTATION_RADIUS + 1;

	static const float TORUS_ANIMATION_SPEED = 56;
	m_torusAnimationPhase = fmodf(
		static_cast<float>(m_torusAnimationPhase + 
		TORUS_ANIMATION_SPEED * m_animationController.GetTimeDelta() * 0.001), 
		static_cast<float>(360));

	PostRedisplay();
}

void CMyApplication::DrawLightSource()const
{
	// Рисуем источник света
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(m_lightPosition.x, m_lightPosition.y, m_lightPosition.z);
	glutSolidSphere(0.1, 15, 10);
	glPopMatrix();
}

void CMyApplication::DrawScene(bool enableLighting)const
{
	// Активизируем материал
	m_material.Activate(GL_FRONT);

	// Задаем параметры источника света
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	COmniLight light(m_light);
	light.SetPosition(m_lightPosition);
	if (!enableLighting)
	{
		// При выключенном освещении источник света 
		// испускает только фоновый свет
		light.SetDiffuseIntensity(0, 0, 0);
		light.SetSpecularIntensity(0, 0, 0);
	}
	light.SetLight(GL_LIGHT0);

	// Рисуем куб
	m_cube.Draw();

	// Рисуем пару сцепленных торов
	glPushMatrix();
	glTranslatef(-2, -8, -2);
	glRotatef(m_torusAnimationPhase, 1, 0, 0);
	glutSolidTorus(1.5, 4, 30, 60);
	glTranslatef(-4, 0, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidTorus(1.5, 4, 30, 60);
	glPopMatrix();
}

void CMyApplication::DrawShadowVolume()const
{
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);

	// Задаем операции циклического либо нециклического инкремента
	// и декремента в зависимости от наличия расширения GL_EXT_stencil_wrap
	GLenum incrOp = GLEW_EXT_stencil_wrap ? GL_INCR_WRAP_EXT : GL_INCR;
	GLenum decrOp = GLEW_EXT_stencil_wrap ? GL_DECR_WRAP_EXT : GL_DECR;

	// Есть ли поддержка раздельных операций над буфером трафарета
	// для лицевых и нелицевых граней, а также оборачивания при
	// инкременте/декременте?
	if (GLEW_EXT_stencil_two_side && GL_EXT_stencil_wrap)
	{
		// Теневой объем будет нарисован за один этап

		// Включаем двусторонний тест трафарета
		glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

		// Фрагменты нелицевых граней, не прошедшие тест глубины,
		// будут увеличивать значения в буфере трафарета
		glActiveStencilFaceEXT(GL_BACK);
		glStencilOp(GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 0, ~0);

		// Фрагменты лицевых граней, не прошедшие тест глубины,
		// будут уменьшать значения в буфере трафарета
		glActiveStencilFaceEXT(GL_FRONT);
		glStencilOp(GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 0, ~0);

		// Рисуем теневой объем, выключив режим отбраковки граней
		glDisable(GL_CULL_FACE);
		m_cube.DrawShadowVolume(m_lightPosition, EXTRUSION_FACTOR);

		// Включаем режим отбраковки граней
		glEnable(GL_CULL_FACE);
		// Выключаем двусторонний тест трафарета
		glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	}
	else	// Требуемые расширения не поддерживаются
	{
		// Теневой объем будет нарисован в два этапа

		// Рисуем нелицевые грани, увеличивая значения в буфере трафарета
		// при непрохождении теста глубины
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, incrOp, GL_KEEP);
		m_cube.DrawShadowVolume(m_lightPosition, EXTRUSION_FACTOR);

		// Рисуем лицевые грани, уменьшая значения в буфере трафарета
		// при непрохождении теста глубины
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, decrOp, GL_KEEP);
		m_cube.DrawShadowVolume(m_lightPosition, EXTRUSION_FACTOR);
	}
}

void CMyApplication::DrawShadowVolumeEdges()const
{
	glDisable(GL_LIGHTING);
	glColor3f(1, 0, 0);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_cube.DrawShadowVolume(m_lightPosition, EXTRUSION_FACTOR);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);	
}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glLoadMatrixd(&(m_rotationController.GetModelViewMatrix()[0][0]));

	// Рисуем источник света в виде сферы
	DrawLightSource();

	// Рисуем сцену без источников света
	DrawScene(false);

	// Рисуем теневой объем
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	DrawShadowVolume();

	// Рисуем сцену со включенным источником света
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDepthFunc(GL_EQUAL);
	DrawScene(true);
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);
	
	// Рисуем ребра граней теневого объема
	DrawShadowVolumeEdges();
}

void CMyApplication::OnRotationControllerUpdate()
{
	PostRedisplay();
}

void CMyApplication::OnReshape(int width, int height)
{
	// Задаем порт просмотра размером с клиентскую область окна
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон окна
	double aspect = double(width) / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Устанавливаем матрицу проецирования
	gluPerspective(FOV, aspect, ZNEAR, ZNEAR + 1);

	// Получаем коэффициенты матрицы проецирования
	double projectionMatrix[4][4]{};
	glGetDoublev(GL_PROJECTION_MATRIX, &projectionMatrix[0][0]);

	// И корректируем их для случая бесконечно удаленной дальней
	// плоскости отсечения
	projectionMatrix[2][2] = -1;
	projectionMatrix[3][2] = -2 * ZNEAR;

	// Загружаем модифицированную матрицу проецирования
	glLoadMatrixd(&projectionMatrix[0][0]);

	glMatrixMode(GL_MODELVIEW);
}
