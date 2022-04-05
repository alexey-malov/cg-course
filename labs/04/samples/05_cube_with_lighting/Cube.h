#pragma once

enum class CubeSide
{
	NEGATIVE_X,
	POSITIVE_X,
	NEGATIVE_Y,
	POSITIVE_Y,
	NEGATIVE_Z,
	POSITIVE_Z,
};

class Cube
{
public:
	// Создаем куб с заданной длиной стороны
	explicit Cube(float size = 1);
	void Draw() const;
	void SetSideColor(CubeSide side, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);

private:
	float m_size;

	// Цвета сторон куба
	GLubyte m_sideColors[6][4];
};
