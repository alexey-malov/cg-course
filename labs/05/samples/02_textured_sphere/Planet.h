#pragma once

#include "../libgl/Material.h"
#include "../libgl/Texture.h"
#include "Sphere.h"

class CPlanet
{
public:
	CPlanet(
		std::wstring const& textureName,
		float radius = 1, unsigned slices = 50,
		unsigned stacks = 25);

	// Анимируем планету (вращение вокруг своей оси)
	void Animate(float timeDelta);

	// рисуем планету
	void Draw() const;

	// Угол наклона оси вращения
	void SetInclinationAngle(float inclinationAngle);

	// Скорость вращения вокруг своей оси
	void SetRotationSpeed(float rotationSpeed);

private:
	CSphere m_sphere; // сфера, аппроксимирующая поверхность планеты
	CMaterial m_material; // материал планеты
	float m_rotationAngle; // угол поворота вокруг своекй оси
	float m_inclinationAngle; // угол наклона оси вращения
	float m_rotationSpeed; // Скорость вращения вокруг собсвтенной оси
	mutable CTexture2D m_texture; // текстура планеты
	std::wstring m_textureName; // имя файла с текстурой
};
