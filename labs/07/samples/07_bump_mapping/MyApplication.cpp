#include "StdAfx.h"
#include "MyApplication.h"
#include "ShaderLoader.h"
#include "ShaderCompiler.h"
#include "ProgramLinker.h"
#include "TextureLoader.h"
#include "ProgramInfo.h"
#include "Utils3D.h"

// Угол обзора по вертикали
const double CMyApplication::FIELD_OF_VIEW = 60;

const double CMyApplication::ZNEAR = 1;

const double CMyApplication::ZFAR = 5;

CMyApplication::CMyApplication(const char * title, int width, int height)
:CGLApplication(title, width, height)
,m_tangentLocation(-1)
,m_diffuseMapLocation(-1)
,m_normalMapLocation(-1)
,m_rotationController(width, height)
{
	AddEventListener(&m_rotationController);
	m_rotationController.AddEventListener(this);
}

CMyApplication::~CMyApplication(void)
{
}

void CMyApplication::OnInit()
{
	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("GLEW_ARB_shader_objects extension is not available");
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.5, 0.5, 0.5, 1);
	glColor3f(1, 1, 1);
	

	glm::dmat4x4 modelView = glm::lookAt(
		glm::dvec3{ 0, 0, 3 }, 
		glm::dvec3{ 0, 0, 0 }, 
		glm::dvec3{ 0, 1, 0 });
	m_rotationController.SetModelViewMatrix(modelView);

	InitShaders();
	LoadTextures();

	m_light.SetPosition(glm::vec3(0, 0, 5));
	m_light.SetDiffuseIntensity(1, 1, 1, 1);
	m_light.SetSpecularIntensity(1, 1, 1, 1);

	m_material.SetDiffuse(1, 1, 1, 1);
	m_material.SetSpecular(static_cast<GLfloat>(0.6), static_cast<GLfloat>(0.6), static_cast<GLfloat>(0.6), static_cast<GLfloat>(1));
	m_material.SetShininess(10);

	m_material.Activate();
	m_light.SetLight(GL_LIGHT0);
}

void CMyApplication::OnRotationControllerUpdate()
{
	PostRedisplay();
}

void CMyApplication::DrawQuad()
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 tex;
		float normal[3];
	};

	Vertex vertices[4] = 
	{
		{ 
			glm::vec3( - 1, 1, 0 ), // координаты вершины
			glm::vec2( 0, 0 ), // текстурные координаты
			{0, 0, 1},		// нормаль
		},
		{ 
			glm::vec3( - 1, -1, 0 ), // координаты вершины
			glm::vec2( 0, 1 ), // текстурные координаты
			{0, 0, 1},		// нормаль
		},
		{
			glm::vec3( 1, -1, 0 ), // координаты вершины
			glm::vec2( 1, 1 ), // текстурные координаты
			{0, 0, 1},		// нормаль
		},
		{
			glm::vec3( 1, 1, 0 ), // координаты вершины
			glm::vec2( 1, 0 ), // текстурные координаты
			{0, 0, 1},		// нормаль
		},
	};

	// Вычисляем касательную
	glm::vec3 p0(vertices[0].pos);
	glm::vec2 t0(vertices[0].tex);

	glm::vec3 p1(vertices[1].pos);
	glm::vec2 t1(vertices[1].tex);

	glm::vec3 p2(vertices[2].pos);
	glm::vec2 t2(vertices[2].tex);

	glm::vec3 tangent = CalculateTangent(p0, p1, p2, t0, t1, t2);

	glBegin(GL_QUADS);
	for (size_t i = 0; i < 4; ++i)
	{
		Vertex const& v = vertices[i];

		glNormal3fv(v.normal);
		glTexCoord2fv(glm::value_ptr(v.tex));
		glVertexAttrib3fv(m_tangentLocation, glm::value_ptr(tangent));
		glVertex3fv(glm::value_ptr(v.pos));
	}
	glEnd();

}

void CMyApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadMatrixd(&(m_rotationController.GetModelViewMatrix()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	m_diffuseMap.Bind();
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	m_normalMap.Bind();
	glEnable(GL_TEXTURE_2D);



	glUseProgram(m_program);
	glUniform1i(m_diffuseMapLocation, 0);
	glUniform1i(m_normalMapLocation, 1);
	DrawQuad();
	glUseProgram(0);
}

void CMyApplication::LoadTextures()
{
	CTextureLoader loader;
	m_diffuseMap = loader.LoadTexture2D(L"face.jpg");
	m_normalMap = loader.LoadTexture2D(L"face-normal.jpg");
}

void CMyApplication::InitShaders()
{
	// Загружаем шейдеры
	CShaderLoader loader;
	CShader vertexShader =
		loader.LoadShader(GL_VERTEX_SHADER, L"bump.vsh");
	CShader fragmentShader =
		loader.LoadShader(GL_FRAGMENT_SHADER, L"bump.fsh");

	// Создаем программы и присоединяем к ней шейдеры
	m_program.Create();
	m_program.AttachShader(vertexShader);
	m_program.AttachShader(fragmentShader);

	// Компилируем шейдеры
	CShaderCompiler compiler;
	compiler.CompileShader(vertexShader);
	compiler.CompileShader(fragmentShader);
	compiler.CheckStatus();

	// Компонуем программу
	CProgramLinker linker;
	linker.LinkProgram(m_program);
	linker.CheckStatus();

	// Выводим информацию о программе
	CProgramInfo programInfo(m_program);
	programInfo.Print(std::cout);

	m_tangentLocation = m_program.GetAttribLocation("tangent");
	m_diffuseMapLocation = m_program.GetUniformLocation("diffuseMap");
	m_normalMapLocation = m_program.GetUniformLocation("normalMap");
}

void CMyApplication::OnReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, aspect, ZNEAR, ZFAR);
	glMatrixMode(GL_MODELVIEW);
}

