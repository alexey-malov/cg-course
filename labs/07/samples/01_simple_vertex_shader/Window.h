#pragma once
#include "BaseWindow.h"
#include "Cube.h"
#include "GLEWInitializer.h"
#include "Shader.h"
#include "ShaderProgram.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);

private:
	struct GLState
	{
		Shader vertexShader{GL_VERTEX_SHADER};
		Shader fragmentShader{GL_FRAGMENT_SHADER};
		ShaderProgram program;
	};

	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

	void OnMouseMove(double x, double y) override;

	// ������� ������ ������ ������ ���������
	void RotateCamera(double xAngleRadians, double yAngleRadians);

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;

	void SetupCameraMatrix();

	std::optional<GLEWInitializer> m_glewInitializer;

	Cube m_cube;
	std::optional<GLState> m_glState;
	// ���������� �� ������ �� ����� ��������
	static constexpr double DISTANCE_TO_ORIGIN = 2;

	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};
	glm::dmat4x4 m_cameraMatrix = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, DISTANCE_TO_ORIGIN },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });
};
