#pragma once

#include "common_inc.h"

class CSphere
{
public:
	CSphere(float radius, unsigned slices, unsigned stacks);
	~CSphere();

	void Draw() const;

private:
	void DrawSphere() const;

	// Идентификатор дисплейного списка, выполняющего рисование сферы
	mutable GLuint m_displayList;
	float m_radius; // радиус
	unsigned m_slices; // количество меридианов
	unsigned m_stacks; // количество параллелей
};
