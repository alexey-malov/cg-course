#include "Planet.h"
#include "../libgl/TextureLoader.h"

CPlanet::CPlanet(std::wstring const& textureName, float radius, unsigned slices, unsigned stacks)
	: m_sphere(radius, slices, stacks)
	, m_textureName(textureName)
	, m_rotationAngle(0)
	, m_inclinationAngle(0)
	, m_rotationSpeed(0)
{
	m_material.SetDiffuse(1, 1, 1);
	m_material.SetShininess(50);
	m_material.SetSpecular(1, 1, 1);
}

// Изменяем угол вращения планеты вокруг своей оси
void CPlanet::Animate(float timeDelta)
{
	m_rotationAngle = fmodf(m_rotationAngle + m_rotationSpeed * timeDelta, 360);
}

void CPlanet::SetInclinationAngle(float inclinationAngle)
{
	m_inclinationAngle = inclinationAngle;
}

void CPlanet::SetRotationSpeed(float rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

void CPlanet::Draw() const
{
	if (!m_texture)
	{
		// Загружаем текстуру при первом вызове метода Draw()
		CTextureLoader loader;
		m_texture.Attach(loader.LoadTexture2D(m_textureName));
	}
	// Активизируем текстуру и материал планеты
	glEnable(GL_TEXTURE_2D);
	m_texture.Bind();
	m_material.Activate();

	// Сохраняем текущую матрицу
	// (предполагается работа с матрицей modelview)
	// и поворачиваем планету
	// (сначала вокруг оси вращения, а затем наклоняем ось)
	glPushMatrix();
	glRotatef(m_inclinationAngle, 1, 0, 0);
	glRotatef(m_rotationAngle, 0, 0, 1);

	// рисуем сферу
	m_sphere.Draw();

	// восстанавливаем текущую матрицу
	glPopMatrix();
}
