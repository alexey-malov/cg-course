#pragma once
#include "BaseWindow.h"

class Window : public BaseWindow
{
public:
	using BaseWindow::BaseWindow;

private:
	void OnMouseButton(int button, int action, [[maybe_unused]] int mods);

	void OnMouseMove(double x, double y);

	// Вращаем камеру вокруг начала координат
	void RotateCamera(double xAngleRadians, double yAngleRadians);

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;

	static void SetupProjectionMatrix(int width, int height);

	void SetupCameraMatrix();

	// Размер видимого объема, который должен поместиться в порт просмотра
	static constexpr double FRUSTUM_SIZE = 2;
	// Расстояние от камеры до точки вращения
	static constexpr double DISTANCE_TO_ORIGIN = 2;

	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};
	glm::dmat4x4 m_cameraMatrix = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, DISTANCE_TO_ORIGIN },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });
};
