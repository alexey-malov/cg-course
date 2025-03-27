#pragma once
#include "Mesh.h"

class Cube
{
public:
	// Создаем куб с заданной длиной стороны
	explicit Cube(float size = 1);

	void Draw() const;

private:
	float m_size;
	Mesh m_mesh;
};
