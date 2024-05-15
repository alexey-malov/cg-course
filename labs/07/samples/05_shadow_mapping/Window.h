#pragma once
#include "BaseWindow.h"
#include "Cube.h"
#include "DiffuseAndSpecular.h"
#include "DiffuseLighting.h"
#include "FrameBuffer.h"
#include "GLEWInitializer.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);

private:
	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

	void OnMouseMove(double x, double y) override;

	// ¬ращаем камеру вокруг начала координат
	void RotateCamera(double xAngleRadians, double yAngleRadians);

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void InitShaders();
	void InitFrameBuffer();

	void Draw(int width, int height) override;

	void SetupCameraMatrix();

	void OnKey(int /*key*/, int /*scanCode*/, int /*action*/, int /*mods*/) override;

	std::optional<GLEWInitializer> m_glewInitializer;

	Cube m_cube;
	std::optional<DiffuseLighting> m_diffuseLighting;
	std::optional<DiffuseAndSpecular> m_diffuseAndSpecularLighting;
	FrameBuffer m_shadowFrameBuffer;

	// –ассто€ние от камеры до точки вращени€
	static constexpr double DISTANCE_TO_ORIGIN = 2;

	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};
	glm::dmat4x4 m_cameraMatrix = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, DISTANCE_TO_ORIGIN },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });

	bool m_useSpecular = false;
	bool m_usePointLight = true;
	glm::vec4 m_pointLightPos{ 0, 0, 0, 1.0 };
	glm::vec4 m_directedLightDirection{ 0, 0, 1.0, 0.0 };
};
