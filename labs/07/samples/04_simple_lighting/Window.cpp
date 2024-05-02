#include "pch.h"
#include "Window.h"

namespace
{
// Угол обзора по вертикали
constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;
// Размер стороны куба
constexpr double CUBE_SIZE = 1;

constexpr double Z_NEAR = 0.1;
constexpr double Z_FAR = 10;

// Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
{
	// Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
	const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{ m });
	// Заменяем 3 столбца исходной матрицы
	return {
		glm::dvec4{ normalizedMatrix[0], 0.0 },
		glm::dvec4{ normalizedMatrix[1], 0.0 },
		glm::dvec4{ normalizedMatrix[2], 0.0 },
		m[3]
	};
}

} // namespace

Window::Window(int w, int h, const char* title)
	: BaseWindow(w, h, title)
	, m_cube(CUBE_SIZE)
{
#if 1
	m_cube.SetSideColor(CubeSide::NEGATIVE_X, { 1, 0, 0, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_X, { 0, 1, 0, 1 });
	m_cube.SetSideColor(CubeSide::NEGATIVE_Y, { 0, 0, 1, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_Y, { 1, 1, 0, 1 });
	m_cube.SetSideColor(CubeSide::NEGATIVE_Z, { 0, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_Z, { 1, 0, 1, 1 });
#else
	m_cube.SetSideColor(CubeSide::NEGATIVE_X, { 1, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_X, { 1, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::NEGATIVE_Y, { 1, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_Y, { 1, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::NEGATIVE_Z, { 1, 1, 1, 1 });
	m_cube.SetSideColor(CubeSide::POSITIVE_Z, { 1, 1, 1, 1 });
#endif
	m_cube.SetSpecularColor({ 1, 1, 1, 1 });
	m_cube.SetShininess(4.0f);
}

void Window::OnMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		m_leftButtonPressed = (action & GLFW_PRESS) != 0;
	}
}

void Window::OnMouseMove(double x, double y)
{
	const glm::dvec2 mousePos{ x, y };
	if (m_leftButtonPressed)
	{
		const auto windowSize = GetFramebufferSize();

		const auto mouseDelta = mousePos - m_mousePos;
		const double xAngle = mouseDelta.y * M_PI / windowSize.y;
		const double yAngle = mouseDelta.x * M_PI / windowSize.x;
		RotateCamera(xAngle, yAngle);
	}
	m_mousePos = mousePos;
}

// Вращаем камеру вокруг начала координат
void Window::RotateCamera(double xAngleRadians, double yAngleRadians)
{
	// Извлекаем из 1 и 2 строки матрицы камеры направления осей вращения,
	// совпадающих с экранными осями X и Y.
	// Строго говоря, для этого надо извлекать столбцы их обратной матрицы камеры, но так как
	// матрица камеры ортонормированная, достаточно транспонировать её подматрицу 3*3
	const glm::dvec3 xAxis{
		m_cameraMatrix[0][0], m_cameraMatrix[1][0], m_cameraMatrix[2][0]
	};
	const glm::dvec3 yAxis{
		m_cameraMatrix[0][1], m_cameraMatrix[1][1], m_cameraMatrix[2][1]
	};
	m_cameraMatrix = glm::rotate(m_cameraMatrix, xAngleRadians, xAxis);
	m_cameraMatrix = glm::rotate(m_cameraMatrix, yAngleRadians, yAxis);

	m_cameraMatrix = Orthonormalize(m_cameraMatrix);
}

void Window::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	const auto proj = glm::perspective(FIELD_OF_VIEW, aspect, Z_NEAR, Z_FAR);
	glLoadMatrixd(&proj[0][0]);
	glMatrixMode(GL_MODELVIEW);
}

void Window::OnRunStart()
{
	// Включаем режим отбраковки граней
	glEnable(GL_CULL_FACE);
	// Отбраковываться будут нелицевые стороны граней
	glCullFace(GL_BACK);
	// Сторона примитива считается лицевой, если при ее рисовании
	// обход верших осуществляется против часовой стрелки
	glFrontFace(GL_CCW);

	// Включаем тест глубины для удаления невидимых линий и поверхностей
	glEnable(GL_DEPTH_TEST);

	m_glewInitializer.emplace();
	if (!GLEW_ARB_shader_objects)
	{
		throw std::runtime_error("Shaders are not supported");
	}

	InitShaders();
}

void Window::InitShaders()
{
	m_glState.emplace();
	m_glState->vertexShader.SetSource(R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;

void main()
{
	gl_Position = ftransform();

	vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
	if (gl_LightSource[0].position.w == 0.0)
	{
		LightDir = gl_LightSource[0].position.xyz;
	}
	else
	{
		LightDir = gl_LightSource[0].position.xyz - vertexPos.xyz;
	}
	
	DiffuseColor = gl_Color;
	Normal = normalize(gl_NormalMatrix * gl_Normal);
}
)");
	m_glState->vertexShader.Compile();

	m_glState->fragmentShader.SetSource(R"(
#version 110

varying vec3 Normal;
varying vec3 LightDir;
varying vec4 DiffuseColor;

void main()
{
	vec3 n = normalize(Normal);
	vec3 l = normalize(LightDir);

	float diffFactor = max(dot(n, l), 0.0);
	vec3 diffColor = diffFactor * gl_LightSource[0].diffuse.rgb * DiffuseColor.rgb;
	vec4 diff = vec4(diffColor, gl_LightSource[0].diffuse.a * DiffuseColor.a);

	gl_FragColor = diff;
}
)");
	m_glState->fragmentShader.Compile();
	if (!m_glState->vertexShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile vertex shader: " + m_glState->vertexShader.GetInfoLog());
	}
	if (!m_glState->fragmentShader.IsCompiled())
	{
		throw std::runtime_error("Failed to compile fragment shader: " + m_glState->fragmentShader.GetInfoLog());
	}
	m_glState->program.AttachShader(m_glState->vertexShader);
	m_glState->program.AttachShader(m_glState->fragmentShader);
	m_glState->program.Link();
	if (!m_glState->program.IsLinked())
	{
		throw std::runtime_error("Failed to link program: " + m_glState->program.GetInfoLog());
	}
}

void Window::Draw(int width, int height)
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec4{ 0, 0, 0, 1 }));

	SetupCameraMatrix();

	glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(glm::vec4{ 1, 1, 1, 1 }));

	glUseProgram(m_glState->program);
	m_cube.Draw();
}

void Window::SetupCameraMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&m_cameraMatrix[0][0]);
}
