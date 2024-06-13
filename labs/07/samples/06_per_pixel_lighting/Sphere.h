#pragma once

class CSphere
{
public:
	CSphere(float radius, unsigned slices, unsigned stacks);
	void Draw()const;
	~CSphere(void);
private:
	void DrawSphere()const;

	// Идентификатор дисплейного списка, выполняющего рисование сферы
	mutable GLuint m_displayList;
	float m_radius;		// радиус
	unsigned m_slices;	// количество меридианов
	unsigned m_stacks;	// количество параллелей
};
