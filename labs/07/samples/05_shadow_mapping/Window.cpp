#include "pch.h"
#include "Window.h"
#include <glm/gtx/io.hpp>

namespace
{
// Угол обзора по вертикали
constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;
// Размер стороны куба
constexpr double CUBE_SIZE = 1;

constexpr double Z_NEAR = 0.1;
constexpr double Z_FAR = 10;

constexpr int ShadowWidth = 1024;
constexpr int ShadowHeight = 1024;

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
	InitFrameBuffer();
	InitShaders();
}

void Window::InitShaders()
{
	m_diffuseLighting.emplace();
	m_diffuseAndSpecularLighting.emplace();
	m_shadowPass.emplace();
	m_drawShadowMap.emplace();
}

void Window::InitFrameBuffer()
{
	// Создаем фреймбуфер для теней
	m_shadowFrameBuffer.Create();
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFrameBuffer);

	// Создаем текстуру для хранения теней
	m_shadowTexture.Create();
	glBindTexture(GL_TEXTURE_2D, m_shadowTexture);

	glTexImage2D(GL_TEXTURE_2D, /* level= */ 0,
		/* internalFormat= */ GL_DEPTH_COMPONENT,
		ShadowWidth, ShadowHeight,
		/* border= */ 0, /* format= */ GL_DEPTH_COMPONENT,
		/* type= */ GL_FLOAT, /*pixels=*/nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Устанавливаем "глубину" на границе текстуры в 1.0 (максимальное значение глубины)
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
		glm::value_ptr(glm::vec4{ 1.f, 1.f, 1.f, 1.f }));

	// Привязываем текстуру к фреймбуферу
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, m_shadowTexture, /* level= */ 0);
	glDrawBuffer(GL_NONE); // Отключаем рисование в буфер цвета в фреймбуфере
	glReadBuffer(GL_NONE); // Отключаем чтение из буфера цвета в фреймбуфере
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Отключили фреймбуфер
}

void Window::Draw(int width, int height)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawShadowToTexture();
	DrawShadowMap();

#if 0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glLightfv(GL_LIGHT0, GL_POSITION,
		glm::value_ptr(m_usePointLight ? m_pointLightPos : m_directedLightDirection));

	SetupCameraMatrix();

	glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(glm::vec4{ 1, 1, 1, 1 }));
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(glm::vec4{1, 1, 1, 1 }));

	//glUseProgram(m_diffuseLighting->GetProgramId());
	glUseProgram(m_useSpecular ? m_diffuseAndSpecularLighting->GetProgramId() : m_diffuseLighting->GetProgramId());
	m_cube.Draw();
#endif
}

void Window::DrawShadowToTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFrameBuffer);
	glViewport(0, 0, ShadowWidth, ShadowHeight);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	const auto lightDir = glm::normalize(
		glm::vec3(m_usePointLight ? m_pointLightPos : m_directedLightDirection));
	const auto lightView = glm::lookAt(
		glm::vec3{ 0.f, 0.f, 3.f }, //-lightDir * 3.0f, // eye
		glm::vec3{ 0.0 },
		glm::vec3{ 0, 1, 0 });
	// std::cout << lightView << "\n";
	const auto lightProj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 20.f);
	const auto cubeModelMatrix = glm::rotate(glm::radians(30.f), glm::vec3{ 1.0, 1.0, 1.0 }) * glm::mat4(1.0f);
	const auto lightSpaceMatrix = lightProj * lightView * cubeModelMatrix;

	m_shadowPass->SetLightSpaceMatrix(lightSpaceMatrix);
	std::cout << lightSpaceMatrix << "\n";
	m_shadowPass->Activate();

	m_cube.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	const auto framebufferSize = GetFramebufferSize();
	glViewport(0, 0, framebufferSize.x, framebufferSize.y);
	glUseProgram(0);
	assert(glGetError() == GL_NO_ERROR);
}

void Window::DrawShadowMap()
{
	m_drawShadowMap->SetDepthTexture(m_shadowTexture);
	m_drawShadowMap->Activate();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.1f, -0.1f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.1f, -0.9f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(0.9f, -0.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(0.9f, -0.9f);

	glEnd();
	glUseProgram(0);
}

void Window::DrawWithShadow()
{

}

void Window::SetupCameraMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&m_cameraMatrix[0][0]);
}

void Window::OnKey(int key, int /*scanCode*/, int action, int /*mods*/)
{
	if (action == GLFW_PRESS && key == ' ')
	{
		m_usePointLight = !m_usePointLight;
		glfwPostEmptyEvent();
	}
	else if (action == GLFW_PRESS && key == 'S')
	{
		m_useSpecular = !m_useSpecular;
		glfwPostEmptyEvent();
	}
}
